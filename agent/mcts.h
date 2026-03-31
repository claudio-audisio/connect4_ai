#pragma once
#include "board/board.h"


struct mctsNode {
	Board board;
	int move;
	mctsNode* parent;
	vector<mctsNode*> children;
	vector<int> untriedMoves;
	int visits = 0;
	float wins = 0;

	mctsNode(const Board& board, const int move, mctsNode* parent) :
		board(board), move(move), parent(parent) {

		// put valid moves to try
		for (int col = 0; col < COLUMNS; col++) {
			if (board.isValidMove(col)) {
				untriedMoves.push_back(col);
			}
		}
	}

	~mctsNode() {
		for (const auto child : children) {
			delete child;
		}
	}

	bool isFullyExpanded() const { return untriedMoves.empty(); }

	bool isTerminal() const {
		return board.checkWin() != 0 || board.isFull();
	}

	float ucb1(const float C = MCTS_C) const {
		if (visits == 0) {
			return numeric_limits<float>::max(); // always expand unvisited nodes first
		}

		return wins / visits + C * sqrt(log(parent->visits) / visits);
	}

	mctsNode* bestChild() const {
		mctsNode* best = nullptr;
		float bestScore = -numeric_limits<float>::max();

		for (const auto& child : children) {
			const float score = child->ucb1(MCTS_C);

			if (score > bestScore) {
				bestScore = score;
				best = child;
			}
		}

		return best;
	}

};


class MCTS {
public:
	explicit MCTS(const int iterations = ITERATIONS, const float C = MCTS_C, const unsigned seed = 42)
		: iterations(iterations), C(C), rng(seed) {
		cout << "using seed " << seed << endl;
	}

	// Returns the best column to play given the current game state
	int bestMove(const Board& board) {
		const auto root = new mctsNode(board, -1, nullptr);

		for (int i = 0; i < iterations; ++i) {
			mctsNode* node = select(root);
			const float result = simulate(node->board);
			backPropagate(node, result);
		}

		// Pick the child with most visits (most robust, ignores noise)
		const mctsNode* best = nullptr;
		int mostVisits = -1;

		for (const auto& child : root->children) {
			if (child->visits > mostVisits) {
				mostVisits = child->visits;
				best = child;
			}
		}

		delete root;

		return best ? best->move : 0;
	}

private:
    int iterations;
    float C;
    mt19937 rng;

    // ── 1. SELECTION ──────────────────────────────────────────────────────
    // Walk down the tree using UCB1 until we find a node with untried moves
    // or a terminal node.
    mctsNode* select(mctsNode* node) {
        while (!node->isTerminal()) {
            if (!node->isFullyExpanded())
                return expand(node);          // still moves to try → expand
            else
                node = node->bestChild();   // fully expanded → go deeper
        }

        return node;   // terminal node reached
    }

    // ── 2. EXPANSION ──────────────────────────────────────────────────────
    // Pick a random untried move, create a child node for it.
    mctsNode* expand(mctsNode* node) {
        // pick a random untried move
        uniform_int_distribution<int> dist(0, node->untriedMoves.size() - 1);
        const int idx  = dist(rng);
        const int col  = node->untriedMoves[idx];
        node->untriedMoves.erase(node->untriedMoves.begin() + idx);

        // build child state
        Board childState = node->board;
        childState.makeMove(col);

        // attach to tree
        node->children.push_back(new mctsNode(childState, col, node));
        return node->children.back();
    }

    // ── 3. SIMULATION (rollout) ────────────────────────────────────────────
    // Play random moves until the game ends.
    // Returns 1.0 if the player who just moved at `startState` won,
    //         0.0 if loss, 0.5 if draw.
    float simulate(Board board) {
        // remember who we are evaluating for
        // (the player who made the move that created this node)
        const int rootPlayer = -board.player; // currentPlayer is NEXT to move

        while (true) {
            const int winner = board.checkWin();

        	if (winner != 0) {
        		return (winner == rootPlayer) ? 1.0f : 0.0f;
        	}

            if (board.isFull()) {
            	return 0.5f;
            }

            // pick a random legal move
            vector<int> legal;

        	for (int col = 0; col < COLUMNS; ++col) {

            	if (board.isValidMove(col)) {
            		legal.push_back(col);
            	}
            }

            uniform_int_distribution<int> dist(0, legal.size() - 1);
            board.makeMove(legal[dist(rng)]);
        }
    }

    // ── 4. BACKPROPAGATION ────────────────────────────────────────────────
    // Walk back to root, incrementing visits and updating win scores.
    void backPropagate(mctsNode* node, float result) {
        while (node != nullptr) {
            node->visits++;
            // `result` is from the perspective of the player who created
            // this node (the one who just moved into it). As we go up the
            // tree, perspectives flip every level.
            node->wins += result;
            result = 1.0f - result;   // flip perspective for parent
            node = node->parent;
        }
    }
	
};