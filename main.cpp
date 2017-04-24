// Percolation.cpp

#include "stdafx.h"
#include <random>
#include <iostream>
#include <fstream>
#include "time.h"

using namespace std;

struct  possibleDirections {
	int x;
	int y;
	int direction;
};

// function for drawing the triangular lattice for the background
void drawLattice(int L)
{

	ofstream lattice_file;
	lattice_file.open("lattice.txt"); // file that contains points for drawing the whole lattice
	lattice_file << "# x \t y \n";

	for (int i = 0; i < L; i = i + 1)
	{
		lattice_file << 0 << "\t" << i << endl;
		lattice_file << L - 1 << "\t" << i << endl;
		lattice_file << endl;
		lattice_file << endl;
	}

	for (int i = 2; i < L - 2; i = i + 2)
	{
		lattice_file << i << "\t" << 0 << endl;
		lattice_file << L - 1 << "\t" << L - 1 - i << endl;
		lattice_file << endl;
		lattice_file << endl;

		lattice_file << i << "\t" << 0 << endl;
		lattice_file << 0 << "\t" << i << endl;
		lattice_file << endl;
		lattice_file << endl;
	}

	for (int i = L - 1; i >= 0; i = i - 2)
	{
		lattice_file << i << "\t" << L - 1 << endl;
		lattice_file << 0 << "\t" << L - 1 - i << endl;
		lattice_file << endl;
		lattice_file << endl;

		lattice_file << i << "\t" << L - 1 << endl;
		lattice_file << L - 1 << "\t" << i << endl;
		lattice_file << endl;
		lattice_file << endl;

	}
	lattice_file.close();
}

bool check_direction(int **lattice, int L, int x0, int y0, int row_start, int a0, int direction)
{
	bool free = true;
	int a = a0;
	int x = x0;
	int y = y0;
	int k = 0;
	int kx = 0;
	int ky = 0;

	switch (direction)
	{
		// north east direction
	case 1:
		while (k <= a)
		{
			if (x + kx >= L || y + ky >= L)
				a++;
			if (x + kx >= L && y + ky >= L)
			{
				x = 0;
				y = 0;
				kx = 0;
				ky = 0;
			}
			else if (x + kx >= L)
			{
				x = 0;
				kx = 0;
				ky--;
			}
			else if (y + ky >= L)
			{
				y = 0;
				ky = 0;
				kx--;
			}
			if (lattice[x + kx][y + ky] == 1)
			{
				return false;
			}
			kx++; ky++;
			k++;
		}
		break;

	case 2:
		while (k <= 2 * a + row_start)
		{
			if (x + kx >= L)
			{
				x = row_start;
				kx = 0;
			}
			if (lattice[x + kx][y] == 1)
			{
				return false;
			}
			kx = kx + 2;
			k = k + 2;
		}
		break;

	case 3:
		while (k <= a)
		{
			if (x + kx >= L || y - ky < 0)
				a++;
			if (x + kx >= L && y - ky < 0)
			{
				x = 0;
				y = L - 1;
				kx = 0;
				ky = 0;
			}
			else if (x + kx >= L)
			{
				x = 0;
				kx = 0;
				ky--;
			}
			else if (y - ky < 0)
			{
				y = L - 1;
				ky = 0;
				kx--;
			}
			if (lattice[x + kx][y - ky] == 1)
			{
				return false;
			}
			kx++; ky++;
			k++;
		}
		break;
	}
	return free;
}

void write_in_file(ofstream& pos_file, int **lattice, int L, int x0, int y0, int row_start, int a0, int direction)
{
	int x = x0;
	int y = y0;
	int a = a0;
	int k = 0, kx = 0, ky = 0;

	switch (direction)
	{
	case 1:															// north east direction
		if (free)
			while (k <= a)
			{
				if (x + kx >= L || y + ky >= L)
					a++;
				if (x + kx >= L && y + ky >= L)
				{
					x = 0;
					y = 0;
					kx = 0;
					ky = 0;
					pos_file << endl;
					pos_file << endl;
				}
				else if (x + kx >= L)
				{
					x = 0;
					kx = 0;
					ky--;
					pos_file << endl;
					pos_file << endl;
				}
				else if (y + ky >= L)
				{
					y = 0;
					ky = 0;
					kx--;
					pos_file << endl;
					pos_file << endl;
				}
				lattice[x + kx][y + ky] = 1;
				pos_file << x + kx << "\t" << y + ky << endl;
				kx++; ky++;
				k++;
			}

		pos_file << endl;
		pos_file << endl;
		break;


	case 2:															// horizontal direction 
		if (free)
		{
			while (k <= 2 * a + row_start)
			{
				if (x + kx >= L)
				{
					if (row_start == 1)
					{
						pos_file << L - 1 << "\t" << y << endl;
						pos_file << endl;
						pos_file << endl;
						pos_file << 0 << "\t" << y << endl;
					}
					else
					{
						a0++;
						pos_file << endl;
						pos_file << endl;
					}

					x = row_start;
					kx = 0;

				}
				lattice[x + kx][y] = 1;
				pos_file << x + kx << "\t" << y << endl;
				kx = kx + 2;
				k = k + 2;
			}
		}

		pos_file << endl;
		pos_file << endl;

		break;

	case 3:																// south east direction 
		if (free)
			while (k <= a)
			{
				if (x + kx >= L || y - ky < 0)
					a++;
				if (x + kx >= L && y - ky < 0)
				{
					x = 0;
					y = L - 1;
					kx = 0;
					ky = 0;
					pos_file << endl;
					pos_file << endl;
				}
				else if (x + kx >= L)
				{
					x = 0;
					kx = 0;
					ky--;
					pos_file << endl;
					pos_file << endl;
				}
				else if (y - ky < 0)
				{
					y = L - 1;
					ky = 0;
					kx--;
					pos_file << endl;
					pos_file << endl;
				}
				lattice[x + kx][y - ky] = 1;
				pos_file << x + kx << "\t" << y - ky << endl;
				kx++; ky++;
				k++;
			}

		pos_file << endl;
		pos_file << endl;
		break;
	}
}

int main()
{
	double concentration = 0; // concentration of needles on lattice
	double needles_mass = 0;
	const int L = 50 + 1; // a square lattice of size LxL (can only be even number, cause we need odd number for our lattice)
	int a = 5;  // length of a linear segments 
	double p; // given probability

	int **lattice; // the parameter is a a pointer to a pointer 
	lattice = new int *[L];
	for (int i = 0; i < L; i++)
		lattice[i] = new int[L];

	for (int i = 0; i < L; i++)
		for (int j = 0; j < L; j++)
		{
			lattice[i][j] = 0;
		}

	srand(time(NULL));

	ofstream pos_file;
	pos_file.open("position.txt"); // file which contains position of all the needles
	pos_file << "# x \t y \n";

	// drawLattice(L);

	///////////////// Main loop
	int free;
	while (concentration < 0.2)
	{
		int y = (rand() % (L));
		int x = (rand() % (L));

		// Check if x position corresponds to lattice 
		/*

		0 *   *   * for even y is only even x allowed
		1   *   *   for odd y only odd x allowed
		2 *   *   *
		3   *   *
		*/
		int row_start;

		if (y % 2 == 0) // even 
			row_start = 0;
		else
			row_start = 1;

		if (y % 2 != 0 && x == 0)
			x = 1;

		if (y % 2 != 0 && x % 2 == 0)
		{
				if (x + 1 >= L)
					x = row_start;
				else
					x++;
		}

		if (y % 2 == 0 && x % 2 != 0)
		{
			if (x + 1 >= L)
				x = row_start;
			else
				x++;
		}
		
		int direction = 1 + (rand() % (3));												// Choose direction randomly
		free = check_direction(lattice, L, x, y, row_start, a, direction);				// If direction is free then go to case otherwise just pass 
		if (free)
		{
			needles_mass += a;
			write_in_file(pos_file, lattice, L, x, y, row_start, a, direction);
		}
		concentration = needles_mass / (L * L);
	}

	/// Part two after filling half of table

	vector<possibleDirections> posDir;

	int idx = 0;
	int row_start;
	for (int i = 0; i < L; i = i++)
	{
		for (int j = 0; j < L; j++)
		{
			if ((j % 2 > 0 && i % 2 > 0) || (j % 2 == 0 && i % 2 == 0))
			{

				if (j % 2 == 0) // even 
					row_start = 0;
				else
					row_start = 1;
				if (check_direction(lattice, L, i, j, row_start, a, 1))
				{
					//Push back new subject created with default constructor
					posDir.push_back(possibleDirections());
					posDir[idx].x = i;
					posDir[idx].y = j;
					posDir[idx].direction = 1;
					idx++;
				}
				if (check_direction(lattice, L, i, j, row_start, a, 2))
				{
					//Push back new subject created with default constructor
					posDir.push_back(possibleDirections());
					posDir[idx].x = i;
					posDir[idx].y = j;
					posDir[idx].direction = 2;
					idx++;
				}
				if (check_direction(lattice, L, i, j, row_start, a, 3))
				{
					//Push back new subject created with default constructor
					posDir.push_back(possibleDirections());
					posDir[idx].x = i;
					posDir[idx].y = j;
					posDir[idx].direction = 3;
					idx++;
				}
			}
		}
	}

	/*for (int i = 0; i < posDir.size(); i++)
	{
		cout << posDir[i].x << " " << posDir[i].y << " " << posDir[i].direction << endl;
	}*/
	
	int rand_idx, x, y, direction;
	//while (!posDir.empty())
	//{
	cout << "Start" << endl;
		rand_idx = (rand() % (posDir.size()));
		x = posDir[rand_idx].x;
		y = posDir[rand_idx].y;
		direction = posDir[rand_idx].direction;
		if (y % 2 == 0) 
			row_start = 0;
		else
			row_start = 1;
		direction = posDir[rand_idx].direction;
		write_in_file(pos_file, lattice, L, x, y, row_start, a, direction);
		/*posDir.erase(posDir.begin() + rand_idx);
		for (int i = 0; i < posDir.size(); i++)
		{
			x = posDir[i].x;
			y = posDir[i].y;
			direction = posDir[i].direction;
			if (check_direction(lattice, L, x, y, row_start, a, direction) == false)
			{
				posDir.erase(posDir.begin() + rand_idx);
			}
		}
		*/

	//}/*
		/*
		cout << "New: " << endl;
		for (int i = 0; i < posDir.size(); i++)
		{
			cout << posDir[i].x << " " << posDir[i].y << " " << posDir[i].direction << endl;
		}
		*/
	pos_file.close();
}
