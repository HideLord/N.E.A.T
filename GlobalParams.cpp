#include "GlobalParams.h"

int GlobalParams::nodeInovationCounter = 0;
int GlobalParams::edgeInovationCounter = 0;
const int GlobalParams::MAXNODES = 1000000;

const double  GlobalParams::C1 = 1.0;
const double  GlobalParams::C2 = 1.0;
const double  GlobalParams::C3 = 1.0;

const double GlobalParams::WEIGHT_RANDOM_STRENGTH = 1.0;
const double GlobalParams::WEIGHT_SHIFT_STRENGTH  = 0.3;

const double GlobalParams::MUTATE_WEIGHT_SHIFT_PROB  = 0.5;
const double GlobalParams::MUTATE_WEIGHT_RANDOM_PROB = 0.5;
const double GlobalParams::MUTATE_EDGE_PROB          = 0.05;
const double GlobalParams::MUTATE_NODE_PROB		     = 0.01;
const double GlobalParams::MUTATE_EDGE_TOGGLE_PROB   = 0.02;