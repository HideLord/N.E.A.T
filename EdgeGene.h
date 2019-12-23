#pragma once
#include <memory>
#include "NodeGene.h"
#include "GlobalParams.h"
struct EdgeGene {
	std::shared_ptr<NodeGene> inNode;
	std::shared_ptr<NodeGene> outNode;
	double weight; // the weight of the edge
	bool isEnabled; // if the edge is working or not
	int inovation;

	bool equals(EdgeGene& other) {
		return other.inNode->inovation == inNode->inovation &&
			other.outNode->inovation == outNode->inovation;
	}
	bool operator<(const EdgeGene& other) const {
		return inovation < other.inovation;
	}

	EdgeGene() {
		inovation = -1;
		isEnabled = 1;
		weight = 0;
	}
};
struct EdgeHasher {
	long long  operator()(const EdgeGene& k) const
	{
		long long key = static_cast<long long>(k.inNode->inovation) * static_cast<long long>(GlobalParams::MAXNODES);
		key += static_cast<long long>(k.outNode->inovation);
		return key;
	}
};