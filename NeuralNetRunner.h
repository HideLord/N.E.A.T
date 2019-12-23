#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "Agent.h"

class NeuralNetRunner {
public:
	/*
		This functions receives the inputs and returns a vector of the outputs
	*/
	static std::vector<double> run(Agent& A, std::vector<double> inputs, int inSize, int outSize) {
		for (auto& n : A.nodes) {
			if (n.second->typ == NodeType::INPUT) {
				n.second->preStorage = n.second->storage = inputs[n.second->inovation];
			}
			if (n.second->typ == NodeType::BIAS) {
				n.second->preStorage = n.second->storage = 1.0;
			}
		}

		for (auto& n : A.nodes) {
			if (n.second->typ != NodeType::INPUT && n.second->typ != NodeType::BIAS) {
				n.second->preStorage = sigmoid(n.second->storage); // Activate the previous storage
				n.second->storage = 0.0; // current storage is reset
			}
		}

		for (auto & e : A.edges) {
			e.outNode->storage += e.inNode->preStorage * e.weight;
		}

		std::vector<double> outputs(outSize);
		for (auto& n : A.nodes) {
			if (n.second->typ == NodeType::OUTPUT) {
				outputs[n.second->inovation - inSize] = n.second->storage;
			}
		}

		return outputs;
	}

	static double sigmoid(double x) {
		return 1.0 / (1 + exp(-x));
	}

	static double relu(double x) {
		return std::max(0.0, x);
	}

	static double leakyRely(double x) {
		return (x > 0 ? x : x*0.01);
	}

	static double leakyRely(double x, double alpha) {
		return (x > 0 ? x : x * alpha);
	}

private:
	NeuralNetRunner();
};