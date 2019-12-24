#include "GlobalParams.h"

int GlobalParams::nodeInovationCounter = 0;
int GlobalParams::edgeInovationCounter = 0;
const int GlobalParams::MAXNODES = 1000000;
const int GlobalParams::DROPOFF_AGE = 15;

const double  GlobalParams::DISJOIN_COEFFICIENT = 2.0;
const double  GlobalParams::EXCESS_COEFFICIENT = 2.0;
const double  GlobalParams::WEIGHT_DIFF_COEFFICIENT = 0.35;

const double GlobalParams::WEIGHT_RANDOM_STRENGTH = 2.0;
const double GlobalParams::WEIGHT_SHIFT_STRENGTH  = 0.1;

const double GlobalParams::MUTATE_WEIGHT_SHIFT_PROB  = 1;
const double GlobalParams::MUTATE_WEIGHT_RANDOM_PROB = 0.25;
const double GlobalParams::MUTATE_EDGE_PROB          = 0.045;
const double GlobalParams::MUTATE_NODE_PROB		     = 0.015;
const double GlobalParams::MUTATE_EDGE_TOGGLE_PROB   = 0.02;

const double GlobalParams::COMPATIBILITY_THRESHOLD     = 2.50;
const double GlobalParams::MIN_COMPATIBILITY_THRESHOLD = 2.00;
const double GlobalParams::COMPATIBILITY_MODIFIER      = 0.05;

const double GlobalParams::AGE_SIGNIFICANCE = 1.5;