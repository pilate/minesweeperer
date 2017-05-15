#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <iostream>
#include <vector>
#include <dos.h>

//#include "grid.h"
#include "statgrid.h"
#include "newgrid.h"
#include "dllmain.h"

using namespace std;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dReason, LPVOID lpReserved ) {
	if (dReason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) &IterateBoard, NULL, 0, NULL);
		//VirtualTest(hModule);
	}

	return TRUE;
}

void IterateBoard( ) {
	Game game = Game();

	int gameCount = 1;
	int wins = 0;
	int losses = 0;

gamestart:
	game.Start();
	//GridPoint start1(1, 1);
	//start1.LeftClick();

restart:
	if (*game.status == 16) {
		gameCount++;

		if (*game.bombsLeft == 0) {
			wins++;
		}
		else {
			losses++;
		}

		if ( (gameCount % 10000) == 0 ) {
			char temp[100];
			sprintf_s(temp, "Wins: %i, Losses: %i", wins, losses);
			MessageBoxA(NULL, temp, "Test", MB_OK);
		}

		goto gamestart;
	}

	// Click all unknown squares to end game
	if (*game.bombsLeft == 0) {
		for (int l = 1; l <= *game.height; l++) {
			for (int i = 1; i <= *game.width; i++) {
				if (game.GetCellValue(i, l) == game.UNKNOWN) {
					game.LeftClick(i, l);
				}
			}
		}
	}

	// Do guaranteed clicks
	for (int l = 1; l <= *game.height; l++) {
		for (int i = 1; i <= *game.width; i++) {
			
			GridPoint gp = game.GetPoint(i, l);

			// If the board was changed, start over
			bool dorestart = gp.EasyClicks();
			if (dorestart) {
				//Sleep(10);
				goto restart;
			}

		}
	}

	StatGrid grid = StatGrid(*game.width, *game.height);

	// Generate grid of touching bomb counts to make an intelligent statistical choice
	for (int l = 1; l <= *game.height; l++) {
		for (int i = 1; i <= *game.width; i++) {
			if (game.GetCellValue(i, l) == game.UNKNOWN || game.GetCellValue(i, l) == game.BOMB) {
				continue;
			}

			int unknown = game.GetCellValue(i, l) - game.GetNeighborCountByType(i, l, game.BOMB);
			for (int j=1; j < 9; j++) {
				vector <int> test = game.GetCellNeighbor(i, l, j);
				if (game.GetCellValue(test[0], test[1]) == game.UNKNOWN) {
					grid.Add(test[0], test[1], unknown);
				}
			}
		}
	}

	// Click grid element with lowest touching number count
	if (grid.Min() != 0) {
		if (grid.Min() == grid.Max()) {
			ClickRandomUnknown(grid);
			goto restart;
		}
		//Sleep(10);
		vector <int> xy = grid.FindPointByValue(grid.Min());
		game.LeftClick(xy[0], xy[1]);

		goto restart;
	}

	// Worst case
	else {
		if (*game.bombsLeft > 0) {
			ClickRandomUnknown(grid);
		}
	}

	goto restart;
}

int GetUnknownsWithNoStats(StatGrid grid) {
	Game game = Game();

	int counter = 0;
	for (int l = 1; l <= *game.height; l++) {
		for (int i = 1; i <= *game.width; i++) {
			if ( (game.GetCellValue(i, l) == game.UNKNOWN) && (grid.Get(i,l) == 0) ) {
				counter++;
			}
		}
	}
	return counter;
}

void ClickRandomUnknown(StatGrid grid) {
	Game game = Game();

	bool clicked = false;

	/*
	GridPoint topleft(1, 1);
	if ( (topleft.value() == topleft.UNKNOWN) && ( grid.Get(1, 1) == 0) ) {
		topleft.LeftClick();
		return;
	}
	*/

	for (int l = 1; l <= *game.height; l++) {
		for (int i = 1; i <= *game.width; i++) {

			if (game.GetCellValue(i, l) == game.UNKNOWN) {
				if ( (GetUnknownsWithNoStats(grid) > 0) && (grid.Get(i,l) == 0) ) {
					game.LeftClick(i, l);
					return;
				}
				else if ( (GetUnknownsWithNoStats(grid) == 0) && (grid.Get(i,l) == grid.Min()) ) {
					game.LeftClick(i, l);
					return;
				}
			}
		}
	}
}