#pragma once
#include <raylib.h>
#include "common/constant.h"
#include "common/utils.h"

using namespace std;


class Board {
public:
	int board[COLUMNS][ROWS];
	int player;
	int lastMove;
	int moveCounter;

	Board() {}
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

	int checkWin() const {
		for (int player = -1; player < 2; player += 2) {
			for (int x = 0; x < COLUMNS; x++) {
				if (checkColumnWin(x, player)) {
					return player;
				}
			}

			for (int y = 0; y < ROWS; y++) {
				if (checkRowWin(y, player)) {
					return player;
				}
			}

			for (int x = 0; x < COLUMNS - 3; x++) {
				if (checkMainDiagWin(x, player)) {
					return player;
				}
			}

			for (int x = 0; x < COLUMNS - 3; x++) {
				if (checkAntiDiagWin(x, player)) {
					return player;
				}
			}
		}

		return 0;
	}

	bool checkColumnWin(const int colId, const int player) const {
		return	(board[colId][0] == player && board[colId][1] == player && board[colId][2] == player && board[colId][3] == player) ||
				(board[colId][1] == player && board[colId][2] == player && board[colId][3] == player && board[colId][4] == player) ||
				(board[colId][2] == player && board[colId][3] == player && board[colId][4] == player && board[colId][5] == player);
	}

	bool checkRowWin(const int rowId, const int player) const {
		return	(board[0][rowId] == player && board[1][rowId] == player && board[2][rowId] == player && board[3][rowId] == player) ||
				(board[1][rowId] == player && board[2][rowId] == player && board[3][rowId] == player && board[4][rowId] == player) ||
				(board[2][rowId] == player && board[3][rowId] == player && board[4][rowId] == player && board[5][rowId] == player) ||
				(board[3][rowId] == player && board[4][rowId] == player && board[5][rowId] == player && board[6][rowId] == player);
	}

	bool checkMainDiagWin(const int colId, const int player) const {
		return	(board[colId][0] == player && board[colId + 1][1] == player && board[colId + 2][2] == player && board[colId + 3][3] == player) ||
				(board[colId][1] == player && board[colId + 1][2] == player && board[colId + 2][3] == player && board[colId + 3][4] == player) ||
				(board[colId][2] == player && board[colId + 1][3] == player && board[colId + 2][4] == player && board[colId + 3][5] == player);
	}

	bool checkAntiDiagWin(const int colId, const int player) const {
		return	(board[colId][3] == player && board[colId + 1][2] == player && board[colId + 2][1] == player && board[colId + 3][0] == player) ||
				(board[colId][4] == player && board[colId + 1][3] == player && board[colId + 2][2] == player && board[colId + 3][1] == player) ||
				(board[colId][5] == player && board[colId + 1][4] == player && board[colId + 2][3] == player && board[colId + 3][2] == player);
	}

	static int randomDisc() {
		const double value = random(0, 3);

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
