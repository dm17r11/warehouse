#include <iostream>
#include <fstream>
#include <cstring>

#include <chrono>
using namespace std::chrono;

using namespace std;


struct Box
{
	int w, h, d;
	int x, y, z;
	int offset = 0;
};

struct Space
{
	int w, h, d;

	Space()
	{
		this->w = 0;
		this->h = 0;
		this->d = 0;
	}

	Space(int w, int h, int d)
	{
		this->w = w;
		this->h = h;
		this->d = d;
	}

	inline int x(int i)
	{
		return (i % w);
	}

	inline int y(int i)
	{
		return (i / w % h);
	}

	inline int z(int i)
	{
		return (i / (w*h));
	}

	inline int idx(int x, int y, int z)
	{
		return (z * (w*h) + y * w + x);
	}

	inline int v()
	{
		return w * h * d;
	}
};

constexpr int EMPTY = 999;
int *warehouse;
Space whs; // warehouse space
Box *boxes;
#define XYZ(space, i) space.x(i), space.y(i), space.z(i)
#define FREEI(i, boxIdx) (warehouse[i] > boxIdx)
#define FREE(x, y, z, boxIdx) (warehouse[whs.idx(x, y, z)] > boxIdx)

bool place(int x1, int y1, int z1, int w, int h, int d, int boxIdx)
{
	if (x1 + w > whs.w || y1 + h > whs.h || z1 + d > whs.d)
		return false;

	for (int x = x1; x < x1 + w; ++x)
	{
		for (int y = y1; y < y1 + h; ++y)
		{
			for (int z = z1; z < z1 + d; ++z)
			{
				if (!FREE(x, y, z, boxIdx))
				{
					return false;
				}
			}
		}
	}

	for (int x = x1; x < x1 + w; ++x)
	{
		for (int y = y1; y < y1 + h; ++y)
		{
			for (int z = z1; z < z1 + d; ++z)
			{
				warehouse[whs.idx(x, y, z)] = boxIdx;
			}
		}
	}

	return true;
}

void clear(int boxIdx)
{
	for (int i = 0; i < whs.v(); ++i)
		if (warehouse[i] == boxIdx)
			warehouse[i] = EMPTY;
}

void printWarehouse()
{
	cout << "\nWarehouse [\n";
	for (int z = 0; z < whs.d; ++z) 
	{
		for (int y = 0; y < whs.h; ++y)
		{
			for (int x = 0; x < whs.w; ++x)
			{
				int value = warehouse[whs.idx(x, y, z)];
				if (value != EMPTY)
					cout << value;
				else
					cout << ".";
			}
			cout << "\n";
		}

		if (z != whs.d - 1)
			cout << "-\n";
	}
	cout << "]\n";

	return;
}

class Iterator {
public:
	int n;
	int* offsets;
	int* offsetsSizes;
	Space* spaces;
	int totalCalls = 0;
	

	Iterator(int n) {
		this->n = n;
		offsets = new int[n];
		offsetsSizes = new int[n];
		spaces = new Space[n];

		for (int i = 0; i < n; ++i)
		{
			offsets[i] = 0;
			spaces[i] = Space((whs.w + 1 - boxes[i].w), (whs.h + 1 - boxes[i].h), (whs.d + 1 - boxes[i].d));
			offsetsSizes[i] = spaces[i].w * spaces[i].h * spaces[i].d;
		}
	}

	bool go(int i) {
		++totalCalls;

		// if (false)
		// {
		// 	cout << '[' << i << ']' <<  " Offsets: ";
		// 	for (int j = 0; j <= i; ++j)
		// 	{
		// 		cout << offsets[j] << " ";
		// 	}
		// 	cout << "\n";
		// 	printWarehouse();
		// }

		if (canPlace(i))
		{
			if (i == n - 1)
				return true;
			
			int& nextOffset = offsets[i + 1];
			int& nextOffsetSize = offsetsSizes[i + 1];
			for (nextOffset = 0; nextOffset < nextOffsetSize; ++nextOffset)
			{
				// Fast skip:
				int x, y, z;
				getXYZ(i + 1, x, y, z);
				if (!FREE(x, y, z, i + 1))
					continue;
				//

				if (go(i + 1))
					return true;
			}

			clear(i);
			return false;
		}
		else
		{
			return false;
		}
	}

	bool canPlace(int i) {
		if (i == -1)
			return true;

		Box& box = boxes[i];
		getXYZ(i, box.x, box.y, box.z);

		return place(box.x, box.y, box.z, box.w, box.h, box.d, i);
	}

	inline void getXYZ(int i, int& x, int& y, int& z)
	{
		int& offset = offsets[i];
		Space& space = spaces[i];

		x = space.x(offset);
		y = space.y(offset);
		z = space.z(offset);
	}
};

#define CANT "Размещение невозможно\n"
#define VERBOSE (argc > 1 && strcmp(argv[1], "-v") == 0)

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Russian");
	auto start = high_resolution_clock::now();

	ifstream fin("input");

	fin >> whs.w >> whs.h >> whs.d;
	int n;
	fin >> n;
	boxes = new Box[n];
	int boxesV = 0;
	for (int i = 0; i < n; ++i)
	{
		fin >> boxes[i].w >> boxes[i].h >> boxes[i].d;
		boxesV += boxes[i].w * boxes[i].h * boxes[i].d;
	}
	fin.close();

	if (boxesV > whs.w * whs.h * whs.d)
	{
		cout << CANT;
		return EXIT_SUCCESS;
	}

	warehouse = new int[whs.h * whs.w * whs.d];
	for (int i = 0; i < whs.h * whs.w * whs.d; ++i)
	{
		warehouse[i] = EMPTY;
	}

	bool possibleVariant = true;
	long long variant = 0;
	bool first = true;
	Iterator iterator(n);

	bool result = iterator.go(-1);

	if (VERBOSE)
	{
		cout << "Can: " << (result ? "yes" : "no") << "\n";
		cout << "Total calls: " << iterator.totalCalls << "\n";
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(stop - start);
		cout << "Total time: " << duration.count() << "ms" << endl;
	}

	if (!result)
	{
		cout << CANT;
		return EXIT_SUCCESS;
	}


	for (int i = 0; i < n; ++i)
	{
		cout << "(" << boxes[i].x << ", " << boxes[i].y << ", " << boxes[i].z << ") ";
		cout << "(" << 
		boxes[i].x + boxes[i].w << ", " << 
		boxes[i].y + boxes[i].h << ", " << 
		boxes[i].z + boxes[i].d << 
		") \n";
	}

	if (VERBOSE)
		printWarehouse();

	return EXIT_SUCCESS;
}
