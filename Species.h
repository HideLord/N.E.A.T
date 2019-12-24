#pragma once
#include "Agent.h"
#include <numeric>
#include <iostream>
class Species {
public:
	Species() {
		generationsWithoutImprovement = 0;
		speciesAge = 0;
		prevBestFitness = -1e6;
		averageFitness = 0;
	}

	std::vector< std::shared_ptr< Agent > > agents;
	
	int generationsWithoutImprovement; // generations since last improvement
	int speciesAge;
	double prevBestFitness;
	double averageFitness;

	// Gets the best agents at the front, the worst at the back
	void sort() {
		std::cout << "Size: " << agents.size() << std::endl;
		if (agents.size() < 2) {
			return;
		}
		std::sort(agents.begin(), agents.end(), [](const std::shared_ptr< Agent >& A, const std::shared_ptr< Agent >& B) {
			if (A->fitness == B->fitness)
				return A->nodes.size() < B->nodes.size();
			return A->fitness > B->fitness;
		});
	}

	int performTournament(int n) const {
		//Generate all indice and then shuffle them
		std::vector<int> indice(agents.size());
		std::iota(indice.begin(), indice.end(), 0);
		shuffle(indice);

		//select n participants
		indice.resize(n);

		//select the best one
		int bestIndex = indice[0];
		for (int i = 1; i < n; i++)
			if (agents[bestIndex]->fitness < agents[indice[i]]->fitness)
				bestIndex = indice[i];
		return bestIndex;
	}

	// Create a set of indice which will be the parents
	// n - the size of the set;
	// k - the size of the tournament;
	std::vector<int> tournamentSelection(int n, int k) const {
		std::vector<int> parentIndice(n);
		for (int i = 0; i < n; i++)
			parentIndice[i] = performTournament(k);
		return parentIndice;
	}

	std::shared_ptr< Agent > getNewAgent(int parentIndexA, int parentIndexB) {
		return Agent::crossover(agents[parentIndexA], agents[parentIndexB]);
	}

	bool isStagnant() const {
		return generationsWithoutImprovement > GlobalParams::DROPOFF_AGE;
	}

	void trim(int leave) {
		agents.resize(leave);
	}

	void kill() {
		agents.clear();
	}

	void add(std::shared_ptr<Agent>& agent) {
		agents.push_back(agent);
	}

	inline int size() const {
		return agents.size();
	}

	double getAverageFitness() const {
		if (size() == 0)return 0.0;
		double fitness = 0;
		for (auto& agent : agents) {
			fitness += agent->globalRank;
		}
		if(speciesAge < 10)fitness *= GlobalParams::AGE_SIGNIFICANCE;
		return fitness / static_cast<double>(size());
	}

	std::shared_ptr< Agent > getSpeciesChild() {
		if (size() == 0) {
			throw;
		}
		if (size() == 1) {
			auto child = getNewAgent(0, 0);
			return child;
		}
		int parentA = performTournament(std::max(1,size() / 2));
		int parentB = performTournament(std::max(1,size() / 2));
		auto child = getNewAgent(parentA, parentB);
		return child;
	}

	std::shared_ptr< Agent > getSpeciesChildRandom() {
		if (size() == 0) {
			throw;
		}
		if (size() == 1) {
			auto child = getNewAgent(0, 0);
			return child;
		}
		int parentA = rand() % size();
		int parentB = rand() % size();
		auto child = getNewAgent(parentA, parentB);
		return child;
	}

	double distance(std::shared_ptr<Agent> pending) {
		if (agents.empty()) {
			return 0;
		}
		return agents[0]->distance(pending);
	}

};