#include <windows.h>
#include <iostream>

#include "grid.h"

typedef void (__stdcall *remoteFunction) (int x, int y);

GridPoint::GridPoint(int x, int y) {
	this->modHand = GetModuleHandle(NULL);
	DWORD BoardOffset = (DWORD) this->modHand + 0x5340;

	DWORD xMaxOffset = (DWORD) this->modHand + 0x5334;
	DWORD yMaxOffset = (DWORD) this->modHand + 0x5338;

	this->xMax = *((int*)xMaxOffset);
	this->yMax = *((int*)yMaxOffset);

	if ((x < 1) || (x > this->xMax) || (y < 1) || (y > this->yMax)) {
		this->valid = false;
	}
	else {
		this->valid = true;
	}

	this->board = (char*)BoardOffset;
	this->x = x;
	this->y = y;
}

char GridPoint::value() {
	if (this->valid == false) {
		return -1;
	}

	char retValue = *(this->board + (32 * this->y) + this->x);
	return retValue & 0x1f;
}

char GridPoint::DisplayChar() {
	char byte = this->value();
	char retn; 

	// Un-clicked tile
	if (byte == UNKNOWN) {
		retn = 0x58;
	}
	// Blank clicked tile
	else if (byte == NO_TOUCHING) {
		retn = 0x30;
	}
	// Number  0x30 = '0' ascii, byte is number on tile
	else {
		retn = 0x30 + byte;
	}

	return retn;
}

/*

Neighbor layout:
	123
    8 4
	765

*/ 
GridPoint GridPoint::GetNeighbor(int neighbor) {
	int x, y;

	// Top row
	if (neighbor < 4) {
		y = this->y - 1;
		x = (this->x - 2) + neighbor;
	}
	// Right side
	else if (neighbor == 4) {
		y = this->y;
		x = this->x + 1;
	}
	// Bottom row
	else if (neighbor < 8) {
		y = this->y + 1;
		x = (this->x + 2) - (neighbor - 4);
	}
	// Left side
	else if (neighbor == 8) {
		y = this->y;
		x = this->x - 1;
	}

	GridPoint gp(x, y);
	return gp;
}

bool GridPoint::EasyClicks() {
	bool clicked = false;

	// If the number of UNKNOWN neighbors is the same as (tile number - known bombs)
	if (this->GetNeighborCountByType(UNKNOWN) == (this->value() - this->GetNeighborCountByType(BOMB))) {
		// Mark them all as bombs
		for (int i = 1; i <= 8; i++) {
			GridPoint neighbor = this->GetNeighbor(i);
			if (neighbor.value() == UNKNOWN) {
				clicked = true;
				neighbor.RightClick();
			}
		}
	}

	if ( (this->value() == this->GetNeighborCountByType(BOMB) ) && (this->GetNeighborCountByType(UNKNOWN) > 0) ) {
		clicked = true;
		this->BothClick();
	}

	return clicked;
}

void GridPoint::BothClick() {
	// Define remote function
	remoteFunction StepBlock = (remoteFunction) ((DWORD) this->modHand + 0x35B7);

	StepBlock(this->x, this->y);
}

void GridPoint::LeftClick() {
	// Define remote function
	remoteFunction StepSquare = (remoteFunction) ((DWORD) this->modHand + 0x3512);

	StepSquare(this->x, this->y);
}

void GridPoint::RightClick() {
	// Define remote function
	remoteFunction MakeGuess = (remoteFunction) ((DWORD) modHand + 0x374F);

	MakeGuess(this->x, this->y);
}

int GridPoint::GetNeighborCountByType(int inType) {
	int counter = 0;
	for (int i = 1; i <= 8; i++) {
		GridPoint neighbor = this->GetNeighbor(i);
		if (neighbor.value() == inType) {
			counter++;
		}
	}
	return counter;
}
