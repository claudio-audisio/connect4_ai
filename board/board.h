#pragma once
#include "common/constant.h"
#include "common/utils.h"


class Board {
public:
	int board[7][6];

	Board() {}
	~Board() {}

	void init(bool random = false) {
		for (int x = 0; x < COLUMNS; x++) {
			for (int y = 0; y < ROWS; y++) {
				if (random) {
					board[x][y] = randomDisc();
				} else {
					board[x][y] = 0;
				}
			}
		}
	}

	void draw() {
		DrawRectangleRounded({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, 0.1, 1, STRUCT_COLOR);

		for (int x = 0; x < COLUMNS; x++) {
			for (int y = 0; y < ROWS; y++) {
				DrawCircle(CELL_SIZE / 2 + x * CELL_SIZE, CELL_SIZE / 2 + y * CELL_SIZE, CELL_RADIUS, getCircleColor(board[x][y]));
			}
		}
	}

	Color getCircleColor(const int value) {
		switch (value) {
		case 1: return GOLD;
		case -1: return DARKRED;
		default: return BG_COLOR;
		}
	}

	int putDisc(const int colId, const int color) {
		int* column = board[colId];

		for (int y = ROWS - 1; y >= 0; y--) {
			if (column[y] == 0) {
				column[y] = color;
				return y;
			}
		}

		return -1;
	}

	bool checkEnd() {
		for (int x = 0; x < COLUMNS; x++) {
			if (board[x][0] == 0) {
				return false;
			}
		}

		return true;
	}

	bool checkColumnWin(const int colId, const int color) const {
		return	(board[colId][0] == color && board[colId][1] == color && board[colId][2] == color && board[colId][3] == color) ||
				(board[colId][1] == color && board[colId][2] == color && board[colId][3] == color && board[colId][4] == color) ||
				(board[colId][2] == color && board[colId][3] == color && board[colId][4] == color && board[colId][5] == color);
	}

	bool checkRowWin(const int rowId, const int color) const {
		return	(board[0][rowId] == color && board[1][rowId] == color && board[2][rowId] == color && board[3][rowId] == color) ||
				(board[1][rowId] == color && board[2][rowId] == color && board[3][rowId] == color && board[4][rowId] == color) ||
				(board[2][rowId] == color && board[3][rowId] == color && board[4][rowId] == color && board[5][rowId] == color) ||
				(board[3][rowId] == color && board[4][rowId] == color && board[5][rowId] == color && board[6][rowId] == color);
	}

	bool checkMainDiagWin(const int colId, const int color) const {
		return	(board[colId][0] == color && board[colId + 1][1] == color && board[colId + 2][2] == color && board[colId + 3][3] == color) ||
				(board[colId][1] == color && board[colId + 1][2] == color && board[colId + 2][3] == color && board[colId + 3][4] == color) ||
				(board[colId][2] == color && board[colId + 1][3] == color && board[colId + 2][4] == color && board[colId + 3][5] == color);
	}

	bool checkAntiDiagWin(const int colId, const int color) const {
		return	(board[colId][3] == color && board[colId + 1][2] == color && board[colId + 2][1] == color && board[colId + 3][0] == color) ||
				(board[colId][4] == color && board[colId + 1][3] == color && board[colId + 2][2] == color && board[colId + 3][1] == color) ||
				(board[colId][5] == color && board[colId + 1][4] == color && board[colId + 2][3] == color && board[colId + 3][2] == color);
	}

	bool checkWin(const int color) const {
		for (int x = 0; x < COLUMNS; x++) {
			if (checkColumnWin(x, color)) {
				return true;
			}
		}

		for (int y = 0; y < ROWS; y++) {
			if (checkRowWin(y, color)) {
				return true;
			}
		}

		for (int x = 0; x < COLUMNS - 3; x++) {
			if (checkMainDiagWin(x, color)) {
				return true;
			}
		}

		for (int x = 0; x < COLUMNS - 3; x++) {
			if (checkAntiDiagWin(x, color)) {
				return true;
			}
		}

		return false;
	}

	int randomDisc() {
		const double value = random(0, 3);

		if (value < 1.0) return -1;
		if (value > 2.0) return 1;
		return 0;
	}

};
