#pragma once
enum class NodeType {
	HIDDEN,
	INPUT,
	OUTPUT,
	BIAS
};

struct NodeGene {
	int inovation;
	NodeType typ;
	double storage;

	double x, y, radius; // for drawing the neural net

	NodeGene(int inovation, NodeType typ) : typ(typ), inovation(inovation) {
		this->storage = 0.0;
		x = 0.0;
		y = 0.0;
		radius = 0.0;
	}

	NodeGene(int inovation, NodeType typ, double x, double y, double radius)
		: typ(typ), inovation(inovation), x(x), y(y), radius(radius) {
		this->storage = 0.0;
	}

	bool operator==(const NodeGene& node)const {
		return node.inovation == inovation;
	}
};
struct NodeHasher {
	std::size_t operator()(const NodeGene& k) const
	{
		return k.inovation;
	}
};