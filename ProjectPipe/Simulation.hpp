#pragma once
#include "app.hpp"
#include <vector>
class simulation {
public:
	const double pi = 3.1415;
	const double g = 9.81;

	App *myapp;

	void run();
	double frictionfactor(double Re, int i);

	simulation(App *app);
	//~simulation();
};