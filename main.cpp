#include <iostream>
#include <fstream>

using namespace std;

struct Box
{
	int w, h, d;
	int x, y, z;
	int offset = 0;
};

int W, H, D;
int *warehouse;
#define IDX(x, y, z) (z * (W*H) + y * W + x)
#define X(i) (i % W)
#define Y(i) (i / W % H)
#define Z(i) (i / (W*H))
#define XYZ(i) X(i), Y(i), Z(i)

bool place(int x1, int y1, int z1, int w, int h, int d, int boxIdx)
{
	for (int x = x1; x < x1 + w; ++x)
	{
		for (int y = y1; y < y1 + h; ++y)
		{
			for (int z = z1; z < z1 + d; ++z)
			{
				if (warehouse[IDX(x, y, z)] != -1)
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
				warehouse[IDX(x, y, z)] = boxIdx;
			}
		}
	}

	return true;
}

// void printWarehouse()
// {
// 	for (int z = 0; z < D; ++z) 
// 	{
// 		for (int y = 0; y < H; ++y)
// 		{
// 			for (int x = 0; x < W; ++x)
// 			{
// 				int value = warehouse[IDX(x, y, z)];
// 				if (value != -1)
// 					cout << value;
// 				else
// 					cout << ".";
// 			}
// 			cout << "\n";
// 		}
// 		cout << "\n";
// 	}

// 	return;
// }

int main()
{
	setlocale(LC_ALL, "Russian");

	ifstream fin("input");

	fin >> W >> H >> D;
	int n;
	fin >> n;
	Box *boxes = new Box[n];
	for (int i = 0; i < n; ++i)
	{
		fin >> boxes[i].w >> boxes[i].h >> boxes[i].d;
	}
	fin.close();

	warehouse = new int[H * W * D];

	bool possibleVariant = true;
	long long variant = 0;
	bool first = true;
	while (true)
	{
		int offsetsLeft = H * W;

		long long currVariant = variant;
		int sum = 0;
		for (int i = n - 1; i >= 0; --i)
		{
			boxes[i].offset = currVariant % (long long)(H * W * D);
			sum += boxes[i].offset;
			currVariant /= (long long)(H * W * D);
		}

		if (sum == 0 && !first)
		{
			cout << "���������� ����������\n";
			return EXIT_SUCCESS;
		}

		first = false;

		for (int i = 0; i < H * W * D; ++i)
		{
			warehouse[i] = -1;
		}
		possibleVariant = true;
		int boxIdx = 0;
		while (boxIdx < n)
		{
			if (place(XYZ(boxes[boxIdx].offset), boxes[boxIdx].w, boxes[boxIdx].h, boxes[boxIdx].d, boxIdx))
			{
				boxes[boxIdx].x = X(boxes[boxIdx].offset);
				boxes[boxIdx].y = Y(boxes[boxIdx].offset);
				boxes[boxIdx].z = Z(boxes[boxIdx].offset);
				++boxIdx;
			}
			else
			{
				possibleVariant = false;
				break;
			}
		}

		if (possibleVariant)
			break;

		variant += 1;
	}

	if (possibleVariant)
	{
		for (int i = 0; i < n; ++i)
		{
			cout << "(" << boxes[i].x << ", " << boxes[i].y << ", " << boxes[i].z << ") ";
			cout << "(" << 
			boxes[i].x + boxes[i].w << ", " << 
			boxes[i].y + boxes[i].h << ", " << 
			boxes[i].z + boxes[i].d << 
			") \n";
		}
	}

	// printWarehouse();

	return EXIT_SUCCESS;
}
