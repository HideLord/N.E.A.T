#pragma once
#include "GlobalFunctions.h"
#include <sl.h>
#include <thread>
#include "Neat.h"
#include <iostream>
#include "Agent.h"
#include <string>
#include "NeuralNetRunner.h"
#include <mutex>
class Graph{
	int width, height;
	std::mutex lockAgent;
	void drawAgent() {
		A = *neat.champion;
		lockAgent.lock();
		for (auto &e : A.edges) {
			if (e.isEnabled) {
				slSetForeColor(0, 1, 0, 1);
			}
			else {
				slSetForeColor(1, 0, 0, 0.5);
			}
			if (e.inNode == nullptr) {
				continue;
			}
			if (e.outNode == nullptr) {
				continue;
			}
			slLine(e.inNode->x, e.inNode->y, e.outNode->x, e.outNode->y);
		}
		for (auto &n : A.nodes) {
			if (n.second->typ == NodeType::BIAS)slSetForeColor(1, 1, 1, 0.7);
			if (n.second->typ == NodeType::INPUT)slSetForeColor(0, 1, 0, 0.7);
			if (n.second->typ == NodeType::HIDDEN)slSetForeColor(0, 0, 1, 0.7);
			if (n.second->typ == NodeType::OUTPUT)slSetForeColor(1, 0, 1, 1);
			slCircleFill(n.second->x, n.second->y, n.second->radius, 50);
		}
		slSetForeColor(1, 1, 1, 1);
		slSetFontSize(12);
		for (auto &n : A.nodes) {
			slText(n.second->x, n.second->y+n.second->radius, std::to_string(n.second->storage).c_str());
		}
		lockAgent.unlock();
	}
	void runApp() {
		slWindow(width, height, "Neural net visualization", false);
		slSetFont(slLoadFont("ttf/Arial.ttf"), 24);
		slSetTextAlign(SL_ALIGN_CENTER);

		std::thread listener(&Graph::listen, this);

		while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(17));

			drawAgent();
			slRender();
		}

		listener.join();

		slClose();
	}
	void listen() {
		while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (slGetKey(SL_KEY_KEYPAD_1)) {
				while (slGetKey(SL_KEY_KEYPAD_1));
				std::cout << "edge" << std::endl;
				lockAgent.lock();
				A.mutateEdge(neat.allEdges);
				lockAgent.unlock();
			}
			if (slGetKey(SL_KEY_KEYPAD_2)) {
				while (slGetKey(SL_KEY_KEYPAD_2));
				std::cout << "node" << std::endl;
				lockAgent.lock();
				A.mutateNode(neat.allEdges);
				lockAgent.unlock();
			}
			if (slGetKey(SL_KEY_KEYPAD_3)) {
				while (slGetKey(SL_KEY_KEYPAD_3));
				std::cout << "toggle" << std::endl;
				lockAgent.lock();
				A.mutateEdgeToggle();
				lockAgent.unlock();
			}
			if (slGetKey(SL_KEY_ENTER)) {
				std::cout << "mutate" << std::endl;
				lockAgent.lock();
				A.mutate(neat.allEdges);
				lockAgent.unlock();
			}
			if (slGetKey(SL_KEY_TAB)) {
				while (slGetKey(SL_KEY_TAB));
				std::cout << "running" << std::endl;
				lockAgent.lock();
				auto outputs = NeuralNetRunner::run(A, std::vector<double>(neat.inputSize,1),neat.inputSize,neat.outputSize);
				lockAgent.unlock();
				for (auto o : outputs)std::cout << o << std::endl;
			}
		}
	}
	void epoch(int gens) {
		neat.init();
		neat.epoch(gens);
	}
	Neat neat;
	Agent A;
public:
	Graph(int in, int out, int popSize, int width, int height) : neat(in,out,popSize), width(width), height(height) { 
		neat.drawingWindowTopLeft_x = 0;
		neat.drawingWindowTopLeft_y = 0;
		neat.drawingWindowBottomRight_x = width;
		neat.drawingWindowBottomRight_y = height;
	}
	void graph() {
		std::thread T(&Graph::epoch, this, 1000);
		std::thread app(&Graph::runApp,this);
		app.join();
		T.join();
	}

};

