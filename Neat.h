#pragma once
#include "Agent.h"
#include "NodeGene.h"
#include "EdgeGene.h"
#include <unordered_map>
#include <unordered_set>
#include "Species.h"

class Neat {
public:
	
	std::unordered_map< std::pair<int, int>, int, pair_hash > allEdges;
	std::unordered_set< NodeGene, NodeHasher > allNodes;

	std::vector< std::shared_ptr<Agent> > allAgents;
	std::vector< Species > allSpecies;

	std::shared_ptr<Agent> champion;

	int outputSize;
	int inputSize;
	int popSize;

	int drawingWindowTopLeft_x, drawingWindowTopLeft_y;
	int drawingWindowBottomRight_x, drawingWindowBottomRight_y;

	double compatibilityThreshold;

	Neat(int inSize, int outSize, int popSize) {
		this->outputSize = outSize;
		this->inputSize = inSize;
		this->popSize = popSize;

		this->compatibilityThreshold = GlobalParams::COMPATIBILITY_THRESHOLD;

		this->drawingWindowTopLeft_x = 0.0;
		this->drawingWindowTopLeft_y = 0.0;
		this->drawingWindowBottomRight_x = 0.0;
		this->drawingWindowBottomRight_y = 0.0;

		GlobalParams::nodeInovationCounter = inputSize + outputSize + 1; // inputs+outputs+biases
		GlobalParams::edgeInovationCounter = (inputSize + 1) * outputSize; // every input and bias -> every output
	}

	void speciate() {
		for (auto& agent : allAgents) {
			bool placed = false;

			for(auto& s: allSpecies){
				if (s.distance(agent) < compatibilityThreshold) {
					placed = true;
					s.add(agent);
					break;
				}
			}

			if (placed)continue;

			allSpecies.push_back(Species());
			allSpecies.back().add(agent);
		}
	}

	void runGeneration(int generation) {
		
	}

	void epoch(int numGenerations) {
		for (int i = 0; i < numGenerations; i++) {
			runGeneration(i + 1);
		}
	}

	Agent getStartingAgentDisconnected() {
		Agent A;
		int nodeInov = 0;

		double height = static_cast<double>(drawingWindowBottomRight_y) - static_cast<double>(drawingWindowTopLeft_y);
		double width = static_cast<double>(drawingWindowBottomRight_x) - static_cast<double>(drawingWindowTopLeft_x);

		double nodeR = height / static_cast<double>(10.0 * std::max(inputSize*2, outputSize*2) + 2.0);

		// Input nodes
		for (int i = 0; i < inputSize; i++) {
			NodeGene input(nodeInov++, NodeType::INPUT, nodeR * 3, (height-nodeR*2)/(inputSize*2) * (1+i*2), nodeR);
			A.nodes[input] = std::shared_ptr<NodeGene>(new NodeGene(input));
		}

		// Output nodes
		for (int i = 0; i < outputSize; i++) {
			NodeGene output(nodeInov++, NodeType::OUTPUT, width - nodeR * 3, (height - nodeR * 2) / (outputSize * 2) * (1 + i * 2), nodeR);
			A.nodes[output] = std::shared_ptr<NodeGene>(new NodeGene(output));
		}

		// Bias node
		NodeGene bias(nodeInov++, NodeType::BIAS, width / 2, height - nodeR * 2, nodeR);
		A.nodes[bias] = std::shared_ptr<NodeGene>(new NodeGene(bias));

		return A;
	}

	Agent getStartingAgentConnected() {
		Agent A;
		int nodeInov = 0;
		int edgeInov = 0;

		double height = static_cast<double>(drawingWindowBottomRight_y) - static_cast<double>(drawingWindowTopLeft_y);
		double width = static_cast<double>(drawingWindowBottomRight_x) - static_cast<double>(drawingWindowTopLeft_x);

		double nodeR = height / static_cast<double>(10.0 * std::max(inputSize * 2, outputSize * 2) + 2.0);

		std::vector< std::shared_ptr<NodeGene> > ins; // possible "from" nodes
		std::vector< std::shared_ptr<NodeGene> > outs; // possible "to" nodes
		// Input nodes
		for (int i = 0; i < inputSize; i++) {
			NodeGene input(nodeInov++, NodeType::INPUT, nodeR * 3, (height - nodeR * 2) / (inputSize * 2) * (1 + i * 2), nodeR);
			ins.push_back(std::shared_ptr<NodeGene>(new NodeGene(input)));
			A.nodes[input] = ins.back();
		}

		// Output nodes
		for (int i = 0; i < outputSize; i++) {
			NodeGene output(nodeInov++, NodeType::OUTPUT, width - nodeR * 3, (height - nodeR * 2) / (outputSize * 2) * (1 + i * 2), nodeR);
			outs.push_back(std::shared_ptr<NodeGene>(new NodeGene(output)));
			A.nodes[output] = outs.back();
		}

		// Bias node
		NodeGene bias(nodeInov++, NodeType::BIAS, width / 2, height - nodeR * 2, nodeR);
		ins.push_back(std::shared_ptr<NodeGene>(new NodeGene(bias)));
		A.nodes[bias] = ins.back();

		for (auto& in : ins) {
			for (auto& out : outs) {
				EdgeGene edge;
				edge.inNode = in;
				edge.outNode = out;
				edge.weight = random(-2, 2);
				edge.inovation = edgeInov++;
				A.edges.push_back(edge);
				allEdges[{in->inovation, out->inovation}] = edge.inovation;
			}
		}

		return A;
	}
};