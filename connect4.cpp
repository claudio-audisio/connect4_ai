#include <iostream>
#include <ostream>
#include <raylib.h>
#include <thread>

#include "board/board.h"
#include "common/constant.h"

using namespace std;


Board board;

void init() {
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "connect4");
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
	SetTargetFPS(FPS);
	board.init();
}


int main() {
	init();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BG_COLOR);

		board.draw();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}