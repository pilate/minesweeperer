#include <vector>
#include "grid.h"

using namespace std;

class Grid {

	public:
		Grid();

		int* xMax;
		int* yMax;

	private:
		HMODULE modHand;
};

class Game {

	public:
		Game();

		static const char UNKNOWN = 0x0f;
		static const char BOMB = 0x0e;
		static const char NO_TOUCHING = 0x0e;

		void Start();

		void LeftClick(int, int);
		void RightClick(int, int);
		void BothClick(int, int);
		GridPoint GetPoint(int, int);

		int GetCellValue(int, int);
		vector <int> GetCellNeighbor(int, int, int);
		int GetNeighborCountByType(int, int, int type);
		int* status;
		int* bombsLeft;

		int* width;
		int* height;

		char* board;

	private:
		HMODULE modHand;
};