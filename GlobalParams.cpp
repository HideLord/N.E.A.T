#include "GlobalParams.h"

int GlobalParams::nodeInovationCounter = 0;
int GlobalParams::edgeInovationCounter = 0;
const int GlobalParams::MAXNODES = 1000000;
const int GlobalParams::DROPOFF_AGE = 15;

const double  GlobalParams::DISJOIN_COEFFICIENT = 2.0;
const double  GlobalParams::EXCESS_COEFFICIENT = 2.0;
const double  GlobalParams::WEIGHT_DIFF_COEFFICIENT = 1.0;

const double GlobalParams::WEIGHT_RANDOM_STRENGTH = 1.0;
const double GlobalParams::WEIGHT_SHIFT_STRENGTH  = 0.3;

const double GlobalParams::MUTATE_WEIGHT_SHIFT_PROB  = 0.5;
const double GlobalParams::MUTATE_WEIGHT_RANDOM_PROB = 0.5;
const double GlobalParams::MUTATE_EDGE_PROB          = 0.5;
const double GlobalParams::MUTATE_NODE_PROB		     = 0.1;
const double GlobalParams::MUTATE_EDGE_TOGGLE_PROB   = 0.02;

const double GlobalParams::COMPATIBILITY_THRESHOLD = 6.0;
const double GlobalParams::COMPATIBILITY_MODIFIER  = 0.3;