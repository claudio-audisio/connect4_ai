#define CNN_USE_OMP

#include <iostream>
#include <ostream>

#include "agent/mcts.h"
#include "board/board.h"
#include "common/constant.h"
#include "cl_random.h"

#include <raylib.h>
#include <thread>

using namespace std;

Agent agent;
Board board;
MCTS mctsOne(&agent, ITERATIONS, MCTS_C_PUCT);
MCTS mctsMinusOne(&agent, ITERATIONS, MCTS_C_PUCT);
int draws = 0;
int winOne = 0;
int winMinusOne = 0;

void init() {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "connect4");
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
	SetTargetFPS(FPS);
}

int randomMove() {
	int move = cl::randomInt(0, 6);

	while (!board.isValidMove(move)) {
		move = cl::randomInt(0, 6);
	}

	return move;
}

void putWinner(const int winner) {
	switch (winner) {
		case 0: draws++; break;
		case 1: winOne++; break;
		case -1: winMinusOne++; break;
		default: break;
	}
}

void printWinners() {
	cout << winOne << " \t" << draws << "\t" << winMinusOne << endl;
}

int main() {
	init();

	while (!WindowShouldClose()) {
		const int move = board.player == 1 ? mctsOne.bestMove(board) : mctsMinusOne.bestMove(board);
		board.makeMove(move);

		BeginDrawing();
		ClearBackground(BG_COLOR);

		board.draw();

		EndDrawing();

		if (board.checkWin(-board.player)) {
			putWinner(-board.player);
			printWinners();
			board.init();
			//this_thread::sleep_for(chrono::seconds(5));
		}

		if (board.isFull()) {
			putWinner(0);
			printWinners();
			board.init();
			//this_thread::sleep_for(chrono::seconds(5));
		}

		if (winOne + winMinusOne + draws >= 100) {
			break;
		}
	}

	CloseWindow();

	return 0;
}