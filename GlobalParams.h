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
	static const int  DROPOFF_AGE;

	static const double DISJOIN_COEFFICIENT, EXCESS_COEFFICIENT, WEIGHT_DIFF_COEFFICIENT;
	static const double WEIGHT_RANDOM_STRENGTH;
	static const double WEIGHT_SHIFT_STRENGTH;

	static const double MUTATE_WEIGHT_SHIFT_PROB;
	static const double MUTATE_WEIGHT_RANDOM_PROB;
	static const double MUTATE_EDGE_PROB;
	static const double MUTATE_NODE_PROB;
	static const double MUTATE_EDGE_TOGGLE_PROB;

	static const double COMPATIBILITY_THRESHOLD;
	static const double COMPATIBILITY_MODIFIER;
private:
	GlobalParams() {}
};

