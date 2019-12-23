#pragma once
#include "Agent.h"
#include <numeric>

class Species {
	Species() {
		generationsWithoutImprovement = 0;
	}

	std::vector<Agent> agents;
	
	int generationsWithoutImprovement; // generations since last improvement

	// Gets the best agents at the front, the worst at the back
	void sort() {
		std::sort(agents.begin(), agents.end(), [](Agent& A, Agent& B) {
			if (A.fitness == B.fitness)
				return A.nodes.size() < B.nodes.size();
			return A.fitness > B.fitness;
		});
	}

	int performTournament(int n) {
		//Generate all indice and then shuffle them
		std::vector<int> indice(agents.size());
		std::iota(indice.begin(), indice.end(), 0);
		shuffle(indice);

		//select n participants
		indice.resize(n);

		//select the best one
		int bestIndex = indice[0];
		for (int i = 1; i < n; i++)
			if (agents[bestIndex].fitness < agents[indice[i]].fitness)
				bestIndex = indice[i];
		return bestIndex;
	}

	// Create a set of indice which will be the parents
	// n - the size of the set;
	// k - the size of the tournament;
	std::vector<int> tournamentSelection(int n, int k) {
		std::vector<int> parentIndice(n);
		for (int i = 0; i < n; i++)
			parentIndice[i] = performTournament(k);
		return parentIndice;
	}

	Agent getNewAgent(int parentIndexA, int parentIndexB) {
		return Agent::crossover(agents[parentIndexA], agents[parentIndexB]);
	}

	bool isStagnant() {
		return generationsWithoutImprovement > GlobalParams::MAX_GENERATIONS_WITHOUT_IMPROVEMENT;
	}

	void trim(int leave) {
		sort();
		agents.resize(leave);
	}

	void kill() {
		agents.clear();
	}

	void add(Agent& agent) {
		agents.push_back(agent);
		sort();
	}

	void shareFitness() {
		for (auto& agent : agents) {
			agent.fitness /= agents.size();
		}
	}

	double distance(Agent& pending) {
		return agents[0].distance(pending);
	}

};