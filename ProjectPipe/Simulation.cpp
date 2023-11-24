#pragma once
#include "Simulation.hpp"
#include <iostream>
#include<Eigen\Eigen>
#include <math.h>

double reynold(double flow, double diameter,double rho, double mu) {
    double A = pow(diameter / 2, 2) * 3.1415;
    double V = flow / A;
    return rho * V * diameter / mu;

}
double simulation::frictionfactor(double Re,int i) {

    //https://link.springer.com/chapter/10.1007/978-3-030-57340-9_37
    double ep_D = myapp->Pipes[i]->roughness / myapp->Pipes[i]->diameter;
    double A0 = -0.79638 * log((ep_D / 8.208) + (7.3357 / Re));
    double A1 = Re * ep_D + 9.3120665 * A0;
    double f = (8.128943 + A1) / (8.128943 * A0 - 0.86859209 * A1 * log(A1 / (3.7099535 * Re)));
    return pow(f, 2);
}

simulation::simulation(App *app) {
    myapp = app;
}
void simulation::run() {

    const double mu = 1;
    const double rho = 1;

    const int size = myapp->Pipes.size();
    std::vector<double> c_t; //pipe transitional coeffecients
    std::vector<double> q; //pipe flows
    std::vector<double> h; //pipe headlosses
    std::vector<double> c; // pipe coefficients
    std::vector<double> A; // cross sectional area

    for (int i = 0; i < size; i++) {
        A.push_back(pow((myapp->Pipes[i]->diameter / 2.f), 2)* pi);
        c_t.push_back(frictionfactor(200000,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2 * g * pow(A[i], 2)));
        
        q.push_back(200000 * A[i] * mu / (rho * myapp->Pipes[i]->diameter));

        h.push_back( pow(q[i], 2) / c_t[i]);

        c.push_back( q[i] / h[i]);
    
    
    }
    std::cout << "Simulating DEBUG" << "\n";
}