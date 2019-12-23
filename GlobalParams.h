#pragma once
#include <cstdlib>
#include <ctime>
#include <random>
#include <vector>
#include <chrono>

class GlobalParams
{
public:
	static int		  nodeInovationCounter;
	static int		  edgeInovationCounter;
	static const int  MAXNODES;

	static const double C1, C2, C3;
	static const double WEIGHT_RANDOM_STRENGTH;
	static const double WEIGHT_SHIFT_STRENGTH;

	static const double MUTATE_WEIGHT_SHIFT_PROB;
	static const double MUTATE_WEIGHT_RANDOM_PROB;
	static const double MUTATE_EDGE_PROB;
	static const double MUTATE_NODE_PROB;
	static const double MUTATE_EDGE_TOGGLE_PROB;
private:
	GlobalParams() {}
};

