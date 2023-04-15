#include <iostream>
#include <fstream>

using namespace std;

struct Box
{
	int w, h;
	int x, y;
	int offset = 0;
};

int W, H;
int *warehouse;
#define IDX(x, y) y *W + x
#define X(i) i / W
#define Y(i) i % W
#define XY(i) X(i), Y(i)

bool place(int x1, int y1, int w, int h, int boxIdx)
{
	for (int x = x1; x < x1 + w; ++x)
	{
		for (int y = y1; y < y1 + h; ++y)
		{
			if (warehouse[IDX(x, y)] != -1)
			{
				return false;
			}
		}
	}

	for (int x = x1; x < x1 + w; ++x)
	{
		for (int y = y1; y < y1 + h; ++y)
		{
			warehouse[IDX(x, y)] = boxIdx;
		}
	}

	return true;
}

// void printWarehouse()
// {
// 	for (int y = -1; y < H + 1; ++y)
// 	{
// 		for (int x = -1; x < W + 1; ++x)
// 		{
// 			if (y == -1 || x == -1 || y == H || x == W)
// 			{
// 				cout << "+";
// 			}
// 			else
// 			{
// 				int value = warehouse[IDX(x, y)];
// 				if (value != -1)
// 					cout << value;
// 				else
// 					cout << " ";
// 			}
// 		}
// 		cout << '\n';
// 	}
// }

int main()
{
	setlocale(LC_ALL, "Russian");

	ifstream fin("input");

	fin >> W >> H;
	int n;
	fin >> n;
	Box *boxes = new Box[n];
	for (int i = 0; i < n; ++i)
	{
		fin >> boxes[i].w >> boxes[i].h;
	}
	fin.close();

	warehouse = new int[H * W];

	bool possibleVariant = true;
	int variant = 0;
	bool first = true;
	while (true)
	{
		int offsetsLeft = H * W;

		int currVariant = variant;
		int sum = 0;
		for (int i = n - 1; i >= 0; --i)
		{
			boxes[i].offset = currVariant % (H * W);
			sum += boxes[i].offset;
			currVariant /= (H * W);
		}

		if (sum == 0 && !first)
		{
			cout << "Размещение невозможно\n";
			return EXIT_SUCCESS;
		}

		first = false;

		for (int i = 0; i < H * W; ++i)
		{
			warehouse[i] = -1;
		}
		possibleVariant = true;
		int boxIdx = 0;
		while (boxIdx < n)
		{
			if (place(XY(boxes[boxIdx].offset), boxes[boxIdx].w, boxes[boxIdx].h, boxIdx))
			{
				boxes[boxIdx].x = X(boxes[boxIdx].offset);
				boxes[boxIdx].y = Y(boxes[boxIdx].offset);
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
			cout << "(" << boxes[i].x << ", " << boxes[i].y << ") ";
			cout << "(" << boxes[i].x + boxes[i].w << ", " << boxes[i].y + boxes[i].h << ") \n";
		}

		// printWarehouse();
	}

	return EXIT_SUCCESS;
}
