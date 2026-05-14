#pragma once
#include <raylib.h>

#include "cl_random.h"
#include "common/constant.h"

using namespace std;


class Board {
public:
	int board[COLUMNS][ROWS];
	int player;
	int lastMove;
	int moveCounter;

	Board() {
		init();
	}

	~Board() {}

	void init(const bool random = false) {
		for (int x = 0; x < COLUMNS; x++) {
			for (int y = 0; y < ROWS; y++) {
				if (random) {
					board[x][y] = randomDisc();
				} else {
					board[x][y] = 0;
				}
			}
		}

		player = 1;
		lastMove = -1;
		moveCounter = 0;
	}

	void makeMove(const int col) {
		int* column = board[col];

		for (int y = ROWS - 1; y >= 0; y--) {
			if (column[y] == 0) {
				column[y] = player;
				break;
			}
		}

		player = -player;
		lastMove = col;
		moveCounter++;
	}

	void undoMove() {
		if (lastMove == -1) {
			throw runtime_error("error undoing move");
		}

		int* column = board[lastMove];

		for (int y = 0; y < ROWS; y++) {
			if (column[y] != 0) {
				column[y] = 0;
				break;
			}
		}

		player = -player;
		lastMove = -1;
		moveCounter--;
	}

	bool isValidMove(const int col) const {
		return board[col][0] == 0;
	}

	bool isFull() const {
		for (int x = 0; x < COLUMNS; x++) {
			if (board[x][0] == 0) {
				return false;
			}
		}

		return true;
	}

	bool checkWin(const int lastPlayer) const {
		for (int x = 0; x < COLUMNS; x++) {
			if (checkColumnWin(x, lastPlayer)) {
				return true;
			}
		}

		for (int y = 0; y < ROWS; y++) {
			if (checkRowWin(y, lastPlayer)) {
				return true;
			}
		}

		for (int x = 0; x < COLUMNS - 3; x++) {
			if (checkMainDiagWin(x, lastPlayer)) {
				return true;
			}
		}

		for (int x = 0; x < COLUMNS - 3; x++) {
			if (checkAntiDiagWin(x, lastPlayer)) {
				return true;
			}
		}

		return false;
	}

	bool checkColumnWin(const int colId, const int lastPlayer) const {
		return	(board[colId][0] == lastPlayer && board[colId][1] == lastPlayer && board[colId][2] == lastPlayer && board[colId][3] == lastPlayer) ||
				(board[colId][1] == lastPlayer && board[colId][2] == lastPlayer && board[colId][3] == lastPlayer && board[colId][4] == lastPlayer) ||
				(board[colId][2] == lastPlayer && board[colId][3] == lastPlayer && board[colId][4] == lastPlayer && board[colId][5] == lastPlayer);
	}

	bool checkRowWin(const int rowId, const int lastPlayer) const {
		return	(board[0][rowId] == lastPlayer && board[1][rowId] == lastPlayer && board[2][rowId] == lastPlayer && board[3][rowId] == lastPlayer) ||
				(board[1][rowId] == lastPlayer && board[2][rowId] == lastPlayer && board[3][rowId] == lastPlayer && board[4][rowId] == lastPlayer) ||
				(board[2][rowId] == lastPlayer && board[3][rowId] == lastPlayer && board[4][rowId] == lastPlayer && board[5][rowId] == lastPlayer) ||
				(board[3][rowId] == lastPlayer && board[4][rowId] == lastPlayer && board[5][rowId] == lastPlayer && board[6][rowId] == lastPlayer);
	}

	bool checkMainDiagWin(const int colId, const int lastPlayer) const {
		return	(board[colId][0] == lastPlayer && board[colId + 1][1] == lastPlayer && board[colId + 2][2] == lastPlayer && board[colId + 3][3] == lastPlayer) ||
				(board[colId][1] == lastPlayer && board[colId + 1][2] == lastPlayer && board[colId + 2][3] == lastPlayer && board[colId + 3][4] == lastPlayer) ||
				(board[colId][2] == lastPlayer && board[colId + 1][3] == lastPlayer && board[colId + 2][4] == lastPlayer && board[colId + 3][5] == lastPlayer);
	}

	bool checkAntiDiagWin(const int colId, const int lastPlayer) const {
		return	(board[colId][3] == lastPlayer && board[colId + 1][2] == lastPlayer && board[colId + 2][1] == lastPlayer && board[colId + 3][0] == lastPlayer) ||
				(board[colId][4] == lastPlayer && board[colId + 1][3] == lastPlayer && board[colId + 2][2] == lastPlayer && board[colId + 3][1] == lastPlayer) ||
				(board[colId][5] == lastPlayer && board[colId + 1][4] == lastPlayer && board[colId + 2][3] == lastPlayer && board[colId + 3][2] == lastPlayer);
	}

	static int randomDisc() {
		const double value = cl::random(0, 3);

		if (value < 1.0) return -1;
		if (value > 2.0) return 1;
		return 0;
	}

	void draw() {
		DrawRectangleRounded({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, 0.1, 1, STRUCT_COLOR);

		for (int x = 0; x < COLUMNS; x++) {
			for (int y = 0; y < ROWS; y++) {
				DrawCircle(CELL_SIZE / 2 + x * CELL_SIZE, CELL_SIZE / 2 + y * CELL_SIZE, CELL_RADIUS, getCircleColor(board[x][y]));
			}
		}
	}

	static Color getCircleColor(const int value) {
		switch (value) {
		case 1: return GOLD;
		case -1: return DARKRED;
		default: return BG_COLOR;
		}
	}

};
