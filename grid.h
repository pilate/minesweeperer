using namespace std;

class GridPoint {
	public:
		static const char UNKNOWN = 0x0f;
		static const char BOMB = 0x0e;
		static const char NO_TOUCHING = 0x0e;

		GridPoint(int, int);
		GridPoint GetNeighbor(int neighbor);

		int GetNeighborCountByType(int);
		
		void BothClick();
		void LeftClick();
		void RightClick();

		bool EasyClicks();

		char DisplayChar();
		char value();

		int x, y; 

	private:
		HMODULE modHand;
		bool valid;
		int xMax, yMax;
		char* board;
};