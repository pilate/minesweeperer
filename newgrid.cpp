#include <windows.h>
#include <iostream>

#include "newgrid.h"

typedef void (__stdcall *remoteFunction) (int, int);

Grid::Grid() {
	modHand = GetModuleHandle(NULL);

}

Game::Game() {
	modHand = GetModuleHandle(NULL);

	width = (int*) ((DWORD) modHand + 0x5334);
	height = (int*) ((DWORD) modHand + 0x5338);

	status = (int *) ((DWORD) modHand + 0x5000);
	bombsLeft = (int *) ((DWORD) modHand + 0x5194);

	board = (char *) ((DWORD) modHand + 0x5340);
}

int Game::GetCellValue(int x, int y) {
	if ((x < 1) || (x > *width) || (y < 1) || (y > *height)) {
		return -1;
	}

	char retValue = *(board + (32 * y) + x);

	return retValue & 0x1f;
}

void Game::Start() {
	typedef void (__stdcall *remoteFunction) ();
	remoteFunction StartGame = (remoteFunction) ((DWORD) modHand + 0x367A);

	StartGame();
}

void Game::BothClick(int x, int y) {
	remoteFunction StepBlock = (remoteFunction) ((DWORD) modHand + 0x35B7);

	StepBlock(x, y);
}

void Game::LeftClick(int x, int y) {
	remoteFunction StepSquare = (remoteFunction) ((DWORD) modHand + 0x3512);

	StepSquare(x, y);
}

void Game::RightClick(int x, int y) {
	remoteFunction MakeGuess = (remoteFunction) ((DWORD) modHand + 0x374F);

	MakeGuess(x, y);
}

GridPoint Game::GetPoint(int x, int y) {
	GridPoint gp(x, y);
	return gp;
}

vector <int> Game::GetCellNeighbor(int x, int y, int neighbor) {
	vector <int> xy(2);

	// Top row
	if (neighbor < 4) {
		xy[0] = (x - 2) + neighbor;
		xy[1] = y - 1;

	}
	// Right side
	else if (neighbor == 4) {
		xy[0] = x + 1;
		xy[1] = y;
	}
	// Bottom row
	else if (neighbor < 8) {
		xy[0] = (x + 2) - (neighbor - 4);
		xy[1] = y + 1;
	}
	// Left side
	else if (neighbor == 8) {
		xy[0] = x - 1;
		xy[1] = y;
	}

	return xy;
}

int Game::GetNeighborCountByType(int x, int y, int type) {
	int counter = 0;
	for (int i = 1; i <= 8; i++) {
		vector <int> neighbor = this->GetCellNeighbor(x, y, i);
		if ( (this->GetCellValue(neighbor[0], neighbor[1]) != -1) && (this->GetCellValue(neighbor[0], neighbor[1]) == type)) {
			counter++;
		}
	}
	return counter;
}