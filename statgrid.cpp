#include <windows.h>
#include <iostream>

#include "statgrid.h"


StatGrid::StatGrid(int xMax, int yMax) {
	this->xMax = xMax;
	this->yMax = yMax;
	this->size = xMax * yMax;

	this->Reset();
}

void StatGrid::Reset() {
	int boardSize = xMax * yMax;
	vector <int> board(boardSize, 0);
	this->board = board;
}

vector <int> StatGrid::FindPointByValue(int value) {
	vector <int> xy(2);
	
	for (int i = 0; i < (int) this->board.size(); i++) {
		if (this->board[i] == value) {
			xy[0] = (i % this->xMax) + 1;
			xy[1] = (i / this->xMax) + 1;

			return xy;
		}
	}
	return xy;
}


int StatGrid::Min() {
	int min = 10;

	for (int i = 0; i < (int) this->board.size(); i++) {
		if (this->board[i] == 0) {
			continue;
		}

		if (this->board[i] < min) {
			min = this->board[i];
		}
	}
	if (min == 10) {
		min = 0;
	}

	return min;
}

int StatGrid::Get(int x, int y) {
	y = y - 1;
	x = x - 1;

	return this->board[(y * this->xMax) + x];
}

int StatGrid::Max() {
	int max = 0;

	for (int i = 0; i < (int) this->board.size(); i++) {
		if (this->board[i] > max) {
			max = this->board[i];
		}
	}
	return max;
}

void StatGrid::Add(int x, int y, int amt) {
	y = y - 1;
	x = x - 1;

	this->board[(y * this->xMax) + x] += amt;
}

void StatGrid::ShowGrid() {
	char temp[1000];
	memset(temp, NULL, sizeof(temp));
	
	
	int counter = 0;
	for (int i=0; i < this->size; i++) {
		int thisSquare = this->board[i];

		temp[counter++] = 48 + this->board[i];
		if (((i+1) % this->xMax) == 0) {
			temp[counter++] = 0x0a;
		}
	}

	MessageBoxA(NULL, temp, "Test", MB_OK);
}