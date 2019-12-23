#pragma once
#include "Agent.h"
#include "NodeGene.h"
#include "EdgeGene.h"
#include <unordered_map>
#include <unordered_set>

class Neat {
public:
	
	std::unordered_map<std::pair<int, int>, int, pair_hash> allEdges;
	std::unordered_set<NodeGene, NodeHasher> allNodes;

	int outputSize;
	int inputSize;
	int popSize;

	int drawingWindowTopLeft_x, drawingWindowTopLeft_y;
	int drawingWindowBottomRight_x, drawingWindowBottomRight_y;

	Neat(int inSize, int outSize, int popSize) {
		this->outputSize = outSize;
		this->inputSize = inSize;
		this->popSize = popSize;

		drawingWindowTopLeft_x = 0.0;
		drawingWindowTopLeft_y = 0.0;
		drawingWindowBottomRight_x = 0.0;
		drawingWindowBottomRight_y = 0.0;

		GlobalParams::nodeInovationCounter = inputSize + outputSize;
		GlobalParams::edgeInovationCounter = 0;
	}

	Agent getStartingAgent() {
		Agent A;
		int nodeInov = 0;

		double height = static_cast<double>(drawingWindowBottomRight_y) - static_cast<double>(drawingWindowTopLeft_y);
		double width = static_cast<double>(drawingWindowBottomRight_x) - static_cast<double>(drawingWindowTopLeft_x);

		double nodeR = height / static_cast<double>(10.0 * std::max(inputSize*2, outputSize*2) + 2.0);

		for (int i = 0; i < inputSize; i++) {
			NodeGene input(nodeInov++, NodeType::INPUT, nodeR * 3, (height-nodeR*2)/(inputSize*2) * (1+i*2), nodeR);
			A.nodes[input] = std::shared_ptr<NodeGene>(new NodeGene(input));
		}


		for (int i = 0; i < outputSize; i++) {
			NodeGene output(nodeInov++, NodeType::OUTPUT, width - nodeR * 3, (height - nodeR * 2) / (outputSize * 2) * (1 + i * 2), nodeR);
			A.nodes[output] = std::shared_ptr<NodeGene>(new NodeGene(output));
		}

		return A;
	}
};