#pragma once

#include "GlobalFunctions.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "GlobalParams.h"
#include "EdgeGene.h"
#include <cassert>


struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2>& p) const {
		return p.first * GlobalParams::MAXNODES + p.second;
	}
};

struct Agent {
	std::vector<EdgeGene> edges;
	std::unordered_map<NodeGene,std::shared_ptr<NodeGene>, NodeHasher> nodes;

	double fitness;
	double adjustedFitness;
	int globalRank;

	void mutate(std::unordered_map<std::pair<int, int>, int, pair_hash>& allEdges) {
		if (random() < GlobalParams::MUTATE_EDGE_PROB) {
			mutateEdge(allEdges);
		}
		if (random() < GlobalParams::MUTATE_NODE_PROB) {
			mutateNode(allEdges);
		}
		if (random() < GlobalParams::MUTATE_WEIGHT_RANDOM_PROB) {
			mutateWeightRandom();
		}
		if (random() < GlobalParams::MUTATE_WEIGHT_SHIFT_PROB) {
			mutateWeightShift();
		}
		if (random() < GlobalParams::MUTATE_EDGE_TOGGLE_PROB) {
			mutateEdgeToggle();
		}
	}

	void mutateEdge(std::unordered_map<std::pair<int, int>, int, pair_hash> & allEdges) {
		std::vector<std::shared_ptr<NodeGene>> possibleInNodes;
		std::vector<std::shared_ptr<NodeGene>> possibleOutNodes;
		for (auto ptr:nodes) {
			switch (ptr.second->typ)
			{
			case NodeType::BIAS:
				possibleInNodes.push_back(ptr.second);
				break;

			case NodeType::INPUT:
				possibleInNodes.push_back(ptr.second);
				break;

			case NodeType::OUTPUT:
				possibleOutNodes.push_back(ptr.second);
				possibleInNodes.push_back(ptr.second);
				break;

			case NodeType::HIDDEN:
				possibleOutNodes.push_back(ptr.second);
				possibleInNodes.push_back(ptr.second);
				break;
			}
		}

		assert(!possibleInNodes.empty(), "Possible in nodes is empty in Mutate Link");
		assert(!possibleOutNodes.empty(), "Possible out nodes is empty in Mutate Link");
		for (int i = 0; i < 100; i++) {
			auto &inNode = getRandomElement(possibleInNodes);
			auto &outNode = getRandomElement(possibleOutNodes);

			bool conExists = false;

			for (auto& e : edges) {
				if (inNode->inovation == e.inNode->inovation &&
					outNode->inovation == e.outNode->inovation) {
					conExists = true;
					break;
				}
			}

			if (conExists)continue;

			if (allEdges.count({ inNode->inovation,outNode->inovation })) {
				EdgeGene e;
				e.inovation = allEdges[{inNode->inovation, outNode->inovation}];
				e.inNode = inNode;
				e.outNode = outNode;
				edges.push_back(e);
				sort(edges.begin(),edges.end());

				break;
			}
			else {
				EdgeGene e;
				e.inovation = GlobalParams::edgeInovationCounter ++;
				e.inNode = inNode;
				e.outNode = outNode;
				edges.push_back(e);
				sort(edges.begin(), edges.end());

				allEdges[{ inNode->inovation, outNode->inovation }] = e.inovation;
				
				break;
			}
		}
	}

	void mutateNode(std::unordered_map<std::pair<int, int>, int, pair_hash>& allEdges) {
		if (edges.empty())return;
		auto &e = getRandomElement(edges);

		std::shared_ptr<NodeGene> newNode (new NodeGene(GlobalParams::nodeInovationCounter++, NodeType::HIDDEN));

		newNode->radius = e.inNode->radius;
		newNode->x = (e.inNode->x + e.outNode->x) / 2.0;
		newNode->y = (e.inNode->y + e.outNode->y) / (2.0 + random(-0.1, 0.1));

		nodes[*newNode] = newNode;

		EdgeGene newEdge;
		newEdge.weight = 1.0;
		newEdge.inovation = GlobalParams::edgeInovationCounter++;
		newEdge.inNode = e.inNode;
		newEdge.outNode = newNode;

		e.inNode = newNode;
		e.inovation = GlobalParams::edgeInovationCounter++;

		allEdges[{e.inNode->inovation, e.outNode->inovation}] = e.inovation;
		allEdges[{newEdge.inNode->inovation, newEdge.outNode->inovation}] = newEdge.inovation;

		edges.push_back(newEdge);
		sort(edges.begin(), edges.end());
	}

	void mutateWeightShift() {
		if (edges.empty())return;
		EdgeGene& edge = getRandomElement(edges);
		edge.weight += random(-1, 1) * GlobalParams::WEIGHT_SHIFT_STRENGTH;
	}

	void mutateWeightRandom() {
		if (edges.empty())return;
		EdgeGene& edge = getRandomElement(edges);
		edge.weight = random(-1, 1) * GlobalParams::WEIGHT_RANDOM_STRENGTH;
	}

	void mutateEdgeToggle() {
		if (edges.empty())return;
		EdgeGene& edge = getRandomElement(edges);
		edge.isEnabled = !edge.isEnabled;
	}

	double distance(std::shared_ptr< Agent > B) {
		Agent& A = *this;

		int indA = 0;
		int indB = 0;

		double disjoint = 0.0;
		double excess = 0.0;
		double weight_diff = 0.0;
		double similar = 0.0;

		while (indA < static_cast<int>(A.edges.size()) && indB < static_cast<int>(B->edges.size())) {
			if (A.edges[indA].inovation == B->edges[indB].inovation) {
				similar+=1.0;
				weight_diff += fabs(A.edges[indA].weight - B->edges[indB].weight);
				indA++;
				indB++;
			}
			else if (A.edges[indA].inovation > B->edges[indB].inovation) {
				indB++;
				disjoint += 1;
			}
			else {
				indA++;
				disjoint += 1;
			}
		}
		excess += static_cast<double>(A.edges.size()) - static_cast<double>(indA);
		excess += static_cast<double>(B->edges.size()) - static_cast<double>(indB);

		double N = static_cast<double>(std::max(A.edges.size(), B->edges.size()));
		return GlobalParams::DISJOIN_COEFFICIENT * disjoint / N + GlobalParams::EXCESS_COEFFICIENT * excess / N + GlobalParams::WEIGHT_DIFF_COEFFICIENT * weight_diff;
	}

	static std::shared_ptr<Agent> crossover(std::shared_ptr<Agent> A, std::shared_ptr<Agent> B) { // A is the better Agent
		std::shared_ptr< Agent > child(new Agent());

		int indA = 0;
		int indB = 0;

		while (indA < (int)A->edges.size() && indB < (int)B->edges.size()) {
			if (A->edges[indA].inovation == B->edges[indB].inovation) {
				if (rand() & 1) { // shared
					child->edges.push_back(A->edges[indA]);
				}
				else {
					child->edges.push_back(B->edges[indB]);
				}
				indA++;
				indB++;
			}
			else if (A->edges[indA].inovation > B->edges[indB].inovation) {
				if (A->fitness > B->fitness)
					child->edges.push_back(B->edges[indB]);  // disjoint from B
				indB++;
			}
			else {
				if (A->fitness > B->fitness)
					child->edges.push_back(A->edges[indA]); // disjoint from A
				indA++;
			}
		}
		if (A->fitness > B->fitness) {
			while (indA < static_cast<int>(A->edges.size())) {
				child->edges.push_back(A->edges[indA]); // excess from A
				indA++;
			}
		}
		else {
			while (indB < static_cast<int>(B->edges.size())) {
				child->edges.push_back(B->edges[indB]); // excess from A
				indB++;
			}
		}
		for (auto &e : child->edges) {
			if (child->nodes.count(*e.inNode)) {
				e.inNode = child->nodes[*e.inNode]; // The node already exists
			}
			else {
				child->nodes[*e.inNode] = std::shared_ptr<NodeGene>(new NodeGene(*e.inNode));
				e.inNode = child->nodes[*e.inNode]; // The node is created
			}
			if (child->nodes.count(*e.outNode)) {
				e.outNode = child->nodes[*e.outNode];
			}
			else {
				child->nodes[*e.outNode] = std::shared_ptr<NodeGene>(new NodeGene(*e.outNode));
				e.outNode = child->nodes[*e.outNode];
			}
		}
		return child;
	}

	Agent() {
		fitness = 0.0;
		adjustedFitness = 0.0;
		globalRank = 0;
	}
};


