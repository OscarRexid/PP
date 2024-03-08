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

    const int sizePipes = myapp->Pipes.size();
    std::vector<double> c_t; //pipe transitional coeffecients
    std::vector<double> q; //pipe flows
    std::vector<double> h; //pipe headlosses
    std::vector<double> c; // pipe coefficients
    std::vector<double> A; // cross sectional area

    //Intitials
    for (int i = 0; i < sizePipes; i++) {
        A.push_back(pow((myapp->Pipes[i]->diameter / 2.f), 2)* pi);
        c_t.push_back(frictionfactor(200000,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2 * g * pow(A[i], 2)));
        
        q.push_back(200000 * A[i] * mu / (rho * myapp->Pipes[i]->diameter));

        h.push_back( pow(q[i], 2) / c_t[i]);

        c.push_back( q[i] / h[i]);
    
    
    }

    //Loop until minimizing enough for our margin of error
    int errors = 1;
    int sizeNodes = myapp->Nodes.size();
    while (errors > 0) {
        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(sizeNodes, sizeNodes);
        
        //Inserting all the connections and their coeffecients into the matrix
        for (int i = 0; i <= sizeNodes; i++) {
            //For each node all the pipes will be positive at position (i,i) and then subtracted
            // at position (i,connection node)
            for (int j = 0; j <= sizePipes; j++) {
                if (myapp->Pipes[j]->Node1->getId() == myapp->Nodes[i]->getId()) { 
                    //Node 1 of pipe j is our node
                    M(i, i) += c[j];
                    M(i, myapp->Pipes[j]->Node2->getId()) = -c[j]; // maybe should be -1?
                }
                else if (myapp->Pipes[j]->Node2->getId() == myapp->Nodes[i]->getId()) {
                    //Node 2 of pipe j is our node
                    M(i, i) += c[j];
                    M(i, myapp->Pipes[j]->Node1->getId()) = -c[j]; // maybe should be -1?
                }
            }
            
        }
        std::vector<int> knownQ;
        Eigen::VectorXd H = Eigen::VectorXd::Zero(sizeNodes);
        Eigen::VectorXd Q;
        for (int i = 0; i <= sizeNodes; i++) {
            //check if node is outlet because then Q is unknown
            if (TRUE) {
                knownQ.push_back(i);
                
            }
            //Check if node is known flow otherwise Q=0
            if (TRUE) {
                Q(i) = 0;
            }
            
        }
        Eigen::MatrixXd MQ = M(knownQ, knownQ);
        Eigen::VectorXd H1 = MQ.colPivHouseholderQr().solve(Q);
        std::cout << H1;
        int j = 0;
        for (int i = 0; i <= sizeNodes; i++) {
            //Check if node is outlet then H is known
            if (TRUE) {
                H(i) = H1(j);
                j++;
            }
        }
        std::vector<double> h2;
        for (int i = 0; i <= sizePipes; i++) {
            //relationship between head and headloss
            h2.push_back(H[myapp->Pipes[i]->Node1->getId()] - H[myapp->Pipes[i]->Node2->getId()]);
        }

        std::vector<double> dev;
        std::vector<double> q2;
        std::vector<double> c2;
        std::vector<double> h3;
        errors = 0;

        for (int i = 0; i <= sizeNodes; i++) {



            q2.push_back( h2[i] * c[i]); //recalcualte flow from headloss  q_c
            double Re = reynold(q2[i], myapp->Pipes[i]->diameter, myapp->Pipes[i]->roughness,1000);
            c_t.push_back( frictionfactor(Re,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2 * g * pow(A[i], 2))); // k
            h3.push_back(pow(q2[i], 2) / c_t[i]); //recalcualte headloss from flow


            c2.push_back( q2[i] / h3[i]); // new resitance coefficient


            dev.push_back( (abs(h3[i]) / abs(h2[i])) - 1); //deviation


            std::cout << "\n" << i << " h: " << h2[i] << "  " << h3[i] << "\n";
            std::cout << i << " dev: " << dev[i] * 100 << "%\n";
            if (abs(dev[i]) > myapp->allowedDev) {
                std::cout << i << ": C: " << c[i] << "C2: " << c2[i] << "\n";
                c[i] = (c2[i] + c[i]) / 2; // new restiance coeffecient based on average

                q[i] = c[i] * h2[i]; // new flow

                double Re = reynold(q[i], myapp->Pipes[i]->diameter, myapp->Pipes[i]->roughness, 1000);

                c_t[i] = frictionfactor(Re,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2 * g * pow(A[i], 2));

                h[i] = pow(q[i], 2) / c_t[i];

                c[i] = q[i] / h[i];

                errors++;

            }

        }

    }
    std::cout << "Simulating DEBUG" << "\n";
}