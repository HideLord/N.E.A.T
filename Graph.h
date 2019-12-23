#pragma once
#include "GlobalFunctions.h"
#include <sl.h>
#include <thread>
#include "Neat.h"
#include <iostream>
#include "Agent.h"
class Graph{
	int width, height;
	void drawAgent() {
		
		for (auto e : A.edges) {
			if (e.isEnabled) {
				slSetForeColor(0, 1, 0, 1);
			}
			else {
				slSetForeColor(1, 0, 0, 0.5);
			}
			slLine(e.inNode->x, e.inNode->y, e.outNode->x, e.outNode->y);
		}
		for (auto n : A.nodes) {
			slSetForeColor(1, 1, 1, 0.7);
			slCircleFill(n.second->x, n.second->y, n.second->radius, 50);
		}
	}
	void runApp() {
		slWindow(width, height, "Neural net visualization", false);

		std::thread listener(&Graph::listen, this);

		while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			drawAgent();
			slRender();
		}

		listener.join();

		slClose();
	}
	void listen() {
		while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			if (slGetKey(SL_KEY_KEYPAD_1)) {
				while (slGetKey(SL_KEY_KEYPAD_1));
				std::cout << "edge" << std::endl;
				A.mutateEdge(neat.allEdges);
			}
			if (slGetKey(SL_KEY_KEYPAD_2)) {
				while (slGetKey(SL_KEY_KEYPAD_2));
				std::cout << "node" << std::endl;
				A.mutateNode(neat.allEdges);
			}
			if (slGetKey(SL_KEY_KEYPAD_3)) {
				while (slGetKey(SL_KEY_KEYPAD_3));
				std::cout << "toggle" << std::endl;
				A.mutateEdgeToggle();
			}
			if (slGetKey(SL_KEY_ENTER)) {
			//	while (slGetKey(SL_KEY_ENTER));
				std::cout << "mutate" << std::endl;
				A.mutate(neat.allEdges);
			}
		}
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
		A = neat.getStartingAgent();
		std::thread app(&Graph::runApp,this);
		app.join();
	}

};

