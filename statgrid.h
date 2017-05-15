#ifndef STATGRID_H
#define STATGRID_H
#endif

#include <vector>

using namespace std;

class StatGrid {

	public:
		static const char UNKNOWN = 0x0f;
		static const char BOMB = 0x0e;

		StatGrid(int, int);

		vector <int> FindPointByValue(int);

		void ShowGrid();
		void Reset();
		void Add(int, int, int);
		int Get(int, int);
		int Min();
		int Max();

		int xMax, yMax;

	private:
		int size;
		vector <int> board;
};