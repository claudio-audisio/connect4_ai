#pragma once
#include "agent.h"
#include "board/board.h"


struct mctsNode {
	Board board;
	int move;
	mctsNode* parent;
	vector<mctsNode*> children;
	int visits = 0;
	float wins = 0;
	float prior = 0;

	mctsNode(const Board& board, const int move, mctsNode* parent) :
		board(board), move(move), parent(parent) {
	}

	~mctsNode() {
		for (const auto child : children) {
			delete child;
		}
	}

	bool isTerminal() const {
		return board.checkWin() != 0 || board.isFull();
	}

	bool isToExpand() const {
		return children.empty();
	}

	float ucb1(const float C = MCTS_C_UCB1) const {
		if (visits == 0) {
			return numeric_limits<float>::max(); // always expand unvisited nodes first
		}

		return wins / visits + C * sqrt(log(parent->visits) / visits);
	}

	float puct(const float C = MCTS_C_PUCT) const {
		const float Q = visits > 0 ? wins / visits : 0.0f;
		const float U = C * prior * sqrt(parent->visits) / (1.0f + visits);
		return Q + U;
	}

	mctsNode* bestChild() const {
		mctsNode* best = nullptr;
		float bestScore = -numeric_limits<float>::max();

		for (const auto& child : children) {
			const float score = child->puct(MCTS_C_PUCT);

			if (score > bestScore) {
				bestScore = score;
				best = child;
			}
		}

		return best;
	}

};

class MCTS {
	Agent* agent;
	int iterations;
	float C;

public:
	explicit MCTS(Agent* agent, const int iterations = ITERATIONS, const float C = MCTS_C_PUCT, const unsigned seed = 42)
		: agent(agent), iterations(iterations), C(C) {
	}

	// Returns the best column to play given the current game state
	int bestMove(const Board& board) const {
		const auto root = new mctsNode(board, -1, nullptr);

		for (int i = 0; i < iterations; ++i) {
			mctsNode* node = select(root);
			const float result = simulate(node->board);
			backPropagate(node, result);
		}

		const int best = selectBestMove(root, board.moveCounter);
		delete root;

		return best;
	}

private:
    // ── 1. SELECTION ──────────────────────────────────────────────────────
    // Walk down the tree using UCB1 until we find a node with untried moves
    // or a terminal node.
    mctsNode* select(mctsNode* node) const {
        while (!node->isTerminal()) {
        	if (node->isToExpand()) {
        		return expand(node);
        	}

            node = node->bestChild();   // fully expanded → go deeper
        }

        return node;   // terminal node reached
    }

	static bool isRoot(const mctsNode* node) {
	    return node->parent == nullptr;
    }

    // ── 2. EXPANSION ──────────────────────────────────────────────────────
	// assigns policy priors at expansion
    mctsNode* expand(mctsNode* node) const {
    	// get policy priors from network for this position
    	const auto priors = agent->policy(node->board, isRoot(node));

    	for (int col = 0; col < COLUMNS; ++col) {
    		if (!node->board.isValidMove(col)) {
    			continue;
    		}

    		Board childState = node->board;
    		childState.makeMove(col);

    		const auto child = new mctsNode(childState, col, node);
    		child->prior = priors[col];
    		node->children.push_back(child);
    	}

    	// return the best child by PUCT to continue the iteration
    	return node->bestChild();
    }

    // ── 3. SIMULATION (rollout) ────────────────────────────────────────────
    float simulate(const Board& board) const {
    	// check for immediate terminal first
    	const int winner = board.checkWin();

    	if (winner != 0) {
    		const int rootPlayer = -board.player;
    		return (winner == rootPlayer) ? 1.0f : 0.0f;
    	}

    	if (board.isFull()) {
    		return 0.5f;
    	}

    	// ask the value head — result is from currentPlayer's perspective
    	// but simulate() should return from the perspective of the player
    	// who just moved (same convention as before)
    	const float v = agent->value(board);          // (+1) = good for currentPlayer
    	// convert: if currentPlayer is next to move, the one who just moved
    	// is -currentPlayer, so we flip the sign
    	return (v + 1.0f) / 2.0f;             // map (-1,+1) → (0,1) for backprop
    }

    // ── 4. BACKPROPAGATION ────────────────────────────────────────────────
    // Walk back to root, incrementing visits and updating win scores.
	static void backPropagate(mctsNode* node, float result) {
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

	static int selectBestMove(const mctsNode* root, const int moves) {
    	if (moves > BEST_MOVE_THS) {
    		// greedy — pick most visited (what you do now)
    		return mostVisitedChild(root);
    	}
    	// sample proportional to visit counts
    	std::vector<float> visits;
    	for (const auto& child : root->children) {
    		visits.push_back(static_cast<float>(child->visits));
    	}

    	return sampleFromDistribution(visits, root->visits);
    }

	static int mostVisitedChild(const mctsNode* root) {
    	// Pick the child with most visits (most robust, ignores noise)
    	const mctsNode* best = nullptr;
    	int mostVisits = -1;

    	for (const auto& child : root->children) {
    		if (child->visits > mostVisits) {
    			mostVisits = child->visits;
    			best = child;
    		}
    	}

    	return best ? best->move : 0;
    }

	static int sampleFromDistribution(const vector<float>& visits, const int totalVisits) {
    	const double rnd = random(0, 1) * static_cast<double>(totalVisits);
    	double sum = 0;

    	for (int i = 0; i < visits.size(); ++i) {
    		sum += visits[i];

    		if (rnd < sum) {
    			return i;
    		}
    	}

    	// fallback
    	for (int i = 0; i < visits.size(); ++i) {
    		if (visits[i] > 0.0f) {
    			return i;
    		}
    	}

    	return 0;
    }
	
};