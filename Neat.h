#pragma once
#include "Agent.h"
#include "NodeGene.h"
#include "EdgeGene.h"
#include <unordered_map>
#include <unordered_set>
#include "Species.h"
#include "NeuralNetRunner.h"

class Neat {
public:
	
	std::unordered_map< std::pair<int, int>, int, pair_hash > allEdges;
	std::unordered_set< NodeGene, NodeHasher > allNodes;

	std::vector< Species > allSpecies;

	std::shared_ptr<Agent> champion;

	int outputSize;
	int inputSize;
	int popSize;

	int drawingWindowTopLeft_x, drawingWindowTopLeft_y;
	int drawingWindowBottomRight_x, drawingWindowBottomRight_y;

	double compatibilityThreshold;
	double bestFitness;

	int bestRank = 0;

	Neat(int inSize, int outSize, int popSize) {
		this->outputSize = outSize;
		this->inputSize = inSize;
		this->popSize = popSize;

		this->compatibilityThreshold = GlobalParams::COMPATIBILITY_THRESHOLD;

		this->drawingWindowTopLeft_x = 0.0;
		this->drawingWindowTopLeft_y = 0.0;
		this->drawingWindowBottomRight_x = 0.0;
		this->drawingWindowBottomRight_y = 0.0;

		bestFitness = 0.0;

		GlobalParams::nodeInovationCounter = inputSize + outputSize + 2; // inputs+outputs+biases
		GlobalParams::edgeInovationCounter = (inputSize + 1) * outputSize + 1; // every input and bias -> every output
	}

	void init() {
		for (int i = 0; i < popSize; i++) {
			auto newAgent = std::shared_ptr<Agent>(new Agent(getStartingAgentConnected()));
			addToSpecies(newAgent);
			evaluate(newAgent);
		}
	}

	void addToSpecies(std::shared_ptr< Agent >& agent) {
		if (!allSpecies.empty()) {
			for (auto& s : allSpecies) {
				if (s.size())
					if (s.distance(agent) < compatibilityThreshold) {
						s.add(agent);
						return;
					}
			}
		}

		allSpecies.push_back(Species());
		allSpecies.back().add(agent);
	}

	void evaluate(std::shared_ptr< Agent > agent) {
		for (int i = 0; i < 20; i++) // let the network stabalize
			NeuralNetRunner::run(*agent, { 0,0 }, inputSize, outputSize);
		std::vector<std::pair<std::vector<double>, double>> inputs{ { {0,0},0 },{ {1,0},1 },{ {0,1},1 },{ {1,1},0 } };
		shuffle(inputs);
		for (auto input : inputs) {
			auto outputs = NeuralNetRunner::run(*agent, input.first, inputSize, outputSize);
			agent->fitness -= fabs(outputs[0] - input.second);
		}
		agent->fitness = -1.0 / agent->fitness;
		
		if (agent->fitness > bestFitness) {
			bestFitness = agent->fitness;
			champion = agent;
		}
	}

	std::shared_ptr<Agent> getChampion() {
		using namespace std;
		std::shared_ptr<Agent> champ = allSpecies[0].agents[0];
		for (auto& s : allSpecies) {
			if (champ->fitness < s.agents[0]->fitness)champ = s.agents[0];
		}
		return champ;
	}

	void rankGlobaly() {
		std::vector< std::shared_ptr<Agent> > rank;
		for (auto& s : allSpecies) rank.insert(rank.end(), s.agents.begin(), s.agents.end());
		std::sort(rank.begin(), rank.end(), [](const std::shared_ptr< Agent > & A, const std::shared_ptr< Agent > & B) {
			if (A->fitness == B->fitness)
				return A->nodes.size() < B->nodes.size();
			return A->fitness > B->fitness;
		});
		for (int i = 0; i < static_cast<int>(rank.size()); i++) {
			rank[i]->globalRank = (rank.size() - i);
		}
		bestRank = rank.size();
	}

	void removeStaleSpecies() {
		std::vector<Species> survivors;
		for (auto& s : allSpecies) {
			if (s.agents[0]->fitness > s.prevBestFitness) {
				s.generationsWithoutImprovement = 0;
				s.prevBestFitness = s.agents[0]->fitness;
			}
			else {
				s.generationsWithoutImprovement++;
			}
			if (!s.isStagnant() || s.agents[0]->globalRank == bestRank)survivors.push_back(s);
		}
		allSpecies = survivors;
	}

	double totalAverageFitness() {
		double sum = 0;
		for (auto& s : allSpecies)
			sum += s.averageFitness;
		return sum;
	}

	void removeWeakSpecies() {
		std::vector<Species> survivors;

		double sum = totalAverageFitness();

		for (auto& s : allSpecies) {
			int breed = floor(s.averageFitness / sum * popSize);	
			if (breed >= 1) survivors.push_back(s);
		}

		allSpecies = survivors;
	}

	void runGeneration(int generation) {
		for (auto& s : allSpecies) {
			s.speciesAge++;
			s.sort();
			s.trim(ceil(s.size() / 2.0));
		}
		rankGlobaly();
		removeStaleSpecies();
		rankGlobaly();
		for (auto& s : allSpecies) 
			s.averageFitness = s.getAverageFitness();
		removeWeakSpecies();
		double sum = totalAverageFitness();
		std::vector< std::shared_ptr<Agent> > children;
		for (auto& s : allSpecies) {
			int breed = floor(s.averageFitness / sum * popSize) - 1;
			for (int i = 0; i < breed; i++) {
				children.push_back(s.getSpeciesChild());
				children.back()->mutate(allEdges);
				evaluate(children.back());
			}
		}
		for (auto& s : allSpecies) 
			s.trim(1);
		while (children.size() + allSpecies.size() < popSize) {
			Species& s = getRandomElement(allSpecies);
			children.push_back(s.getSpeciesChild());
			children.back()->mutate(allEdges);
			evaluate(children.back());
		}

		for (auto& c : children) 
			addToSpecies(c);

		std::cout << "num species: " << allSpecies.size() << std::endl;
	}

	void update(int generation) {
		compatibilityThreshold -= GlobalParams::COMPATIBILITY_MODIFIER;
		compatibilityThreshold = std::max(compatibilityThreshold, GlobalParams::MIN_COMPATIBILITY_THRESHOLD);
	}

	void epoch(int numGenerations) {
		for (int i = 0; i < numGenerations; i++) {
			update(i);
			runGeneration(i + 1);
			std::cout << bestFitness << std::endl;
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