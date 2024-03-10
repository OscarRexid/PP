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
    const double rho = 1000.f;

    const int sizePipes = myapp->Pipes.size();
    int sizeNodes = myapp->Nodes.size();
    std::vector<double> c_t; //pipe transitional coeffecients
    std::vector<double> q; //pipe flows
    std::vector<double> h; //pipe headlosses
    std::vector<double> c; // pipe coefficients
    std::vector<double> A; // cross sectional area
    Eigen::VectorXd H = Eigen::VectorXd::Zero(sizeNodes);

    //Intitials
    for (int i = 0; i < sizePipes; i++) {
        A.push_back(pow((myapp->Pipes[i]->diameter / 2.f), 2)* pi);
        c_t.push_back(frictionfactor(200000.f,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2 * g * pow(A[i], 2)));
        
        q.push_back(200000.f * A[i] * myapp->Pipes[i]->roughness / (rho * myapp->Pipes[i]->diameter));

        h.push_back( pow(q[i], 2) / c_t[i]);
        std::cout << h[i] << "\n";

        c.push_back( q[i] / h[i]);
    
    
    }

    //Loop until minimizing enough for our margin of error
    int errors = 1;
    int attempts = 0;
    while (errors > 0) {
        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(sizeNodes, sizeNodes);
        
        //Inserting all the connections and their coeffecients into the matrix
        for (int i = 0; i < sizeNodes; i++) {
            //For each node all the pipes will be positive at position (i,i) and then subtracted
            // at position (i,connection node)
            for (int j = 0; j < sizePipes; j++) {
                if (myapp->Pipes[j]->Node1->getId() == myapp->Nodes[i]->getId()) { 
                    //Node 1 of pipe j is our node
                    M(i, i) += c[j];
                    M(i, myapp->Pipes[j]->Node2->getId()-1) = -c[j]; // maybe should be -1?
                }
                else if (myapp->Pipes[j]->Node2->getId() == myapp->Nodes[i]->getId()) {
                    //Node 2 of pipe j is our node
                    M(i, i) += c[j];
                    M(i, myapp->Pipes[j]->Node1->getId()-1) = -c[j]; // maybe should be -1?
                }
            }
            
        }
        std::vector<int> knownQ;
        Eigen::VectorXd Q = Eigen::VectorXd::Zero(sizeNodes);
        for (int i = 0; i < sizeNodes; i++) {
            //check if node is outlet because then Q is unknown
            if (!(myapp->Nodes[i]->flowTypeVar == Node::flowType::output)) {
                knownQ.push_back(i);
            }
            //Check if node is known flow otherwise Q=0
            if (myapp->Nodes[i]->flowTypeVar == Node::flowType::input) {
                Q(i) = myapp->Nodes[i]->flowNode->flow;
            }
            
        }
        for (int i = 0; i < knownQ.size(); i++) {
            std::cout << "Q:" << knownQ[i] << "\n";
        }
       
        Eigen::VectorXd Q1 = Q(knownQ); // Remove unknown flows(outlets)
        for (int i = 0; i < knownQ.size(); i++) {
            std::cout << "Q1:" << Q1[i] << "\n";
        }
        Eigen::MatrixXd MQ = M(knownQ, knownQ); //same as above but for coeffecient matrix
        Eigen::VectorXd H1 = MQ.colPivHouseholderQr().solve(Q1); // Solve matrix
        for (int i = 0; i < H1.size(); i++) {
            std::cout <<i << " H1: " << H1[i] << "\n";
        }

        int j = 0;
        for (int i = 0; i < sizeNodes; i++) {
            //Check if node is outlet then H is known
            if (myapp->Nodes[i]->flowTypeVar == Node::flowType::output) {
                H(i) = myapp->Nodes[i]->height;
            }
            //else if (myapp->Nodes[i]->flowTypeVar == Node::flowType::input) {

            //}
            else {
                H(i) = H1(j) + myapp->Nodes[i]->height;
                j++;
            }
            std::cout << i << " H: " << H[i] << "\n";
        }
        std::vector<double> h2;
        for (int i = 0; i < sizePipes; i++) {
            //relationship between head and headloss
            h2.push_back(H[myapp->Pipes[i]->Node1->getId()-1] - H[myapp->Pipes[i]->Node2->getId()-1]);
            std::cout << "h2: " << h2[i] << "\n";
        }

        std::vector<double> dev;
        std::vector<double> q2;
        std::vector<double> c2;
        std::vector<double> h3;
        errors = 0;

        for (int i = 0; i < sizePipes; i++) {

            q2.push_back( h2[i] * c[i]); //recalcualte flow from headloss  q_c
            double Re = reynold(abs(q2[i]), myapp->Pipes[i]->diameter, myapp->Pipes[i]->roughness,1000);
            c_t.push_back( frictionfactor(Re,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2.f * g * pow(A[i], 2))); // k
            if (q2[i] < 0) {
                h3.push_back(-1.f*pow(q2[i], 2) / c_t[i]); //recalcualte headloss from flow
            }
            else {
                h3.push_back(pow(q2[i], 2) / c_t[i]); //recalcualte headloss from flow
            }
            c2.push_back( q2[i] / h3[i]); // new resitance coefficient

            dev.push_back( (abs(h3[i]) / abs(h2[i])) - 1); //deviation

            std::cout << "\n" << i << " q: " << q[i] << "  " << q2[i] << "\n";
            std::cout << "\n" << i << " h: " << h2[i] << "  " << h3[i] << "\n";
            std::cout << i << " dev: " << dev[i] * 100 << "%\n";
            if (abs(dev[i]) > myapp->allowedDev) {
                std::cout << i << ": C: " << c[i] << "C2: " << c2[i] << "\n";
                c[i] = (c2[i] + c[i]) / 2.f; // new restiance coeffecient based on average

                q[i] = c[i] * h2[i]; // new flow
                std::cout << "q: " << q[i] << "\n";

                double Re = reynold(abs(q[i]), myapp->Pipes[i]->diameter,1000.f, myapp->Pipes[i]->roughness);
                std::cout << "Re: " << Re << "\n";
                c_t[i] = frictionfactor(Re,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2.f * g * pow(A[i], 2));
                std::cout << "c_t:" << c_t[i] << "\n";
                if (q[i] < 0) {
                    h[i] = -1.f*pow(q[i], 2) / c_t[i];
                }
                else {
                    h[i] = pow(q[i], 2) / c_t[i];
                }
                std::cout << "h:" << h[i] << "\n";

                c[i] = q[i] / h[i];
                std::cout << "c:" << c[i] << "\n";
                errors++;

            }

        }
        std::cout << "Errors: " << errors << "\n";
        attempts++;
        std::cout << "Attempts:" << attempts << "\n";
    }
    
    

    std::cout << "Finished simulating" << "\n";

    //Storing results for post-processing
    for (int i = 0; i < sizePipes; i++) {
        myapp->Pipes[i]->storeResults(q[i], H[myapp->Pipes[i]->Node1->getId() - 1], H[myapp->Pipes[i]->Node2->getId() - 1]);
    }
}