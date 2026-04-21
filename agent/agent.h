#pragma once
#include <array>

#include "tiny_dnn/tiny_dnn.h"
#include "board/board.h"

using namespace std;

class Agent {
	tiny_dnn::network<tiny_dnn::sequential> policyNet;
	tiny_dnn::network<tiny_dnn::sequential> valueNet;
	mt19937 rng;

public:
	Agent() {}

	void init() {
		initPolicyNet();
		initValueNet();
	}

	array<float, 7> policy(const Board& board, const bool isRoot = false) {
		const auto input = toInput(board);
		auto out = policyNet.predict(input);     // 7 values after softmax

		// apply Dirichlet noise at root only
		if (isRoot) {
			const auto noise  = dirichletNoise(7, /*alpha=*/0.3f);  // alpha=0.3 for Connect Four
			const float epsilon = 0.25f;   // 25% noise, 75% network prior

			for (int col = 0; col < 7; ++col) {
				out[col] = (1 - epsilon) * out[col] + epsilon * noise[col];
			}
		}

		array<float, 7> probs;
		float sum = 0.0f;

		for (int col = 0; col < 7; ++col) {
			probs[col] = board.isValidMove(col) ? out[col] : 0.0f;
			sum += probs[col];
		}

		// renormalize after masking
		if (sum > 0.0f) {
			for (auto& p : probs) {
				p /= sum;
			}
		} else {
			// all columns somehow masked — fallback to uniform over legal moves
			for (int col = 0; col < 7; ++col) {
				probs[col] = board.isValidMove(col) ? 1.0f/7 : 0.0f;
			}
		}

		return probs;
	}

	float value(const Board& board) {
		const auto input = toInput(board);
		const auto out = valueNet.predict(input);      // single tanh output
		return out[0];
	}

private:
	static tiny_dnn::vec_t toInput(const Board& board) {
		tiny_dnn::vec_t input(3 * ROWS * COLUMNS, 0.0f);
		const int player = board.player;

		for (int row = 0; row < ROWS; ++row) {
			for (int column = 0; column < COLUMNS; ++column) {
				const int index = row * COLUMNS + column;

				if (board.board[column][row] == player) {
					input[index] = 1.0f;                      // plane 0: my pieces
				} else if (board.board[column][row] == -player) {
					input[42 + index] = 1.0f;                 // plane 1: opponent pieces
				}
			}
		}

		// plane 2: turn indicator
		const float turnVal = (player == 1) ? 1.0f : 0.0f;

		for (int i = 84; i < 126; ++i) {
			input[i] = turnVal;
		}

		return input;
	}

	void initPolicyNet() {
		policyNet << tiny_dnn::fully_connected_layer(126, 128)
				  << tiny_dnn::relu_layer()
				  << tiny_dnn::fully_connected_layer(128, 64)
				  << tiny_dnn::relu_layer()
				  << tiny_dnn::fully_connected_layer(64, 7)
				  << tiny_dnn::softmax_layer();
	}

	void initValueNet() {
		valueNet << tiny_dnn::fully_connected_layer(126, 128)
				 << tiny_dnn::relu_layer()
				 << tiny_dnn::fully_connected_layer(128, 64)
				 << tiny_dnn::relu_layer()
				 << tiny_dnn::fully_connected_layer(64, 1)
				 << tiny_dnn::tanh_layer();
	}

	std::vector<float> dirichletNoise(const int n, const float alpha) {
		std::gamma_distribution<float> gamma(alpha, 1.0f);
		std::vector<float> noise(n);
		float sum = 0.0f;

		for (auto& x : noise) {
			x = gamma(rng); sum += x;
		}

		for (auto& x : noise) {
			x /= sum;
		}

		return noise;
	}

};
