#pragma once
#include "Simulation.hpp"
#include <iostream>
#include<Eigen\Eigen>
#include <math.h>

double reynold(double flow, double diameter,double rho, double mu) {
    double A = pow((diameter / 2.f), 2) * 3.1415;
    double V = flow / A;
    return rho * V * diameter / mu;

}
double simulation::frictionfactor(double Re,int i) {

    //https://link.springer.com/chapter/10.1007/978-3-030-57340-9_37
    double ep_D = myapp->Pipes[i]->roughness / myapp->Pipes[i]->diameter;
    double A0 = -0.79638 * log((ep_D / 8.208) + (7.3357 / Re));
    double A1 = Re *ep_D+ 9.3120665 * A0;
    double f = (8.128943 + A1) / (8.128943 * A0 - 0.86859209 * A1 * log(A1 / (3.7099535 * Re)));
    return pow(f, 2);
}

simulation::simulation(App *app) {
    myapp = app;
}
void simulation::run() {

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
        c_t.push_back(((myapp->Pipes[i]->diameter * 2.f * g * pow(A[i], 2)) / (frictionfactor(200000.f, i) * myapp->Pipes[i]->length)));
        if (myapp->Pipes[i]->Node1->KValue > 0) {
            c_t[i] += (2 * g * pow(A[i], 2)) / myapp->Pipes[i]->Node1->KValue;
        }
        

        //c_t.push_back((frictionfactor(200000.f,i) * myapp->Pipes[i]->length / (myapp->Pipes[i]->diameter * 2 * g * pow(A[i], 2))) + myapp->Pipes[i]->Node1->KValue/(2 * g * pow(A[i], 2)));
        
        q.push_back(200000.f * A[i] * 0.001 / (rho * myapp->Pipes[i]->diameter));
        
        
        
       
 
        std::cout << "q: " << q[i] << "\n";
        std::cout << "c_t: " << c_t[i] << "\n";
        
        h.push_back( (pow(q[i], 2) / c_t[i]) );

       /* if (myapp->Pipes[i]->afterBooster) {
            if (myapp->Pipes[i]->boosterNode == 1) {
                h[i] += myapp->Pipes[i]->Node1->boosterNode->headGained;
            }
            else {
                h[i] += myapp->Pipes[i]->Node2->boosterNode->headGained;
            }

        } */
        
        std::cout << "h: " << h[i] << "\n";

        c.push_back( q[i] / h[i]);
   
    
    }

    //Loop until minimizing enough for our margin of error
    int errors = 1;
    int attempts = 0;
    while (errors > 0 && attempts < 100) {
        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(sizeNodes, sizeNodes);
       // Eigen::MatrixXd Mbooster = Eigen::MatrixXd::Zero(sizeNodes, sizeNodes);


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
            
            //Check if node is known flow otherwise Q=0
            if (myapp->Nodes[i]->flowTypeVar == Node::flowType::input) {
                Q(i) = myapp->Nodes[i]->flowNode->flow;
                
            }
            if (myapp->Nodes[i]->flowTypeVar == Node::flowType::connection && myapp->Nodes[i]->connectionTypeVar == Node::connectionType::booster) {
                
                /* This is a potential real solution to boosters but it corrupts the heap and I dont understand why right now.
                * Alternative solution is adding Head and booster node and head gain to the pipe before but it feels a little dirty
                * 
                double head = myapp->Nodes[i]->boosterNode->headGained;

                Q(i) += c[myapp->Nodes[i]->boosterNode->outletPipe->getId() - 1]* head;
                int j = 0;
                if (myapp->Nodes[i]->boosterNode->outletPipe->getId() == myapp->Nodes[i]->connectedPipes[0]->getId()) {
                    j = 1;
                }
                if ( myapp->Nodes[i]->boosterNode->outletPipe->boosterNode == 1) {
                    if (j == 0) {
                        Q(myapp->Nodes[i]->boosterNode->outletPipe->Node2->getId()) -= c[myapp->Nodes[i]->connectedPipes[0]->getId() - 1] * head;
                    }
                    else if (j == 1) {
                        Q(myapp->Nodes[i]->boosterNode->outletPipe->Node2->getId()) -= c[myapp->Nodes[i]->connectedPipes[1]->getId() - 1] * head;
                    }
                   
                }
                else {
                    if (j == 0) {
                        Q(myapp->Nodes[i]->boosterNode->outletPipe->Node1->getId()) -= c[myapp->Nodes[i]->connectedPipes[0]->getId() - 1] * head;
                    }
                    else if (j == 1) {
                        Q(myapp->Nodes[i]->boosterNode->outletPipe->Node1->getId()) -= c[myapp->Nodes[i]->connectedPipes[1]->getId() - 1] * head;
                    }

                }*/



            }

            /*
            if (myapp->Nodes[i]->flowTypeVar == Node::flowType::connection && myapp->Nodes[i]->connectionTypeVar == Node::connectionType::booster) {
                
                if (myapp->Nodes[i]->boosterNode->outletPipe->getId() == myapp->Nodes[i]->connectedPipes[0]->getId()) {
                    int j = myapp->Nodes[i]->connectedPipes[0]->getId() - 1;
                    int k = myapp->Nodes[i]->connectedPipes[1]->getId() - 1;
                    Q[i] += c[j] * myapp->Nodes[i]->boosterNode->headGained;
                    Q[i] -= c[k] * myapp->Nodes[i]->boosterNode->headGained;
                }
                else {
                    int j = myapp->Nodes[i]->connectedPipes[1]->getId() - 1;
                    int k = myapp->Nodes[i]->connectedPipes[0]->getId() - 1;
                    Q[i] += c[j] * myapp->Nodes[i]->boosterNode->headGained;
                    Q[i] -= c[k] * myapp->Nodes[i]->boosterNode->headGained;
                }
            
            }*/
          
            //check if node is outlet because then Q is the estimated guess of the pipe connected to it
            if ((myapp->Nodes[i]->flowTypeVar == Node::flowType::output)) {

                // will need to normalize the direction here to check what the direction of the q is indicating as q I think is node1 to node2(check this later) 
                //so we need to check if we are node 1 or 2
                    Q[i] = -1.f * abs(q[myapp->Nodes[i]->connectedPipes[0]->getId() - 1]);

                

              
            }
            knownQ.push_back(i);
           
            
        }
        for (int i = 0; i < Q.size(); i++) {
            std::cout << "Q:" << Q[i] << "\n";
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

        for (int i = 0; i < sizeNodes; i++) {
            //Check if node is outlet then H is known
            if (myapp->Nodes[i]->flowTypeVar == Node::flowType::output) {
                H(i) = H1(i) + myapp->Nodes[i]->height; // fix
            }
            //else if (myapp->Nodes[i]->flowTypeVar == Node::flowType::input) {

            //}
            else if (myapp->Nodes[i]->flowTypeVar == Node::flowType::connection && myapp->Nodes[i]->connectionTypeVar == Node::connectionType::booster) {
                H(i) = H1(i) + myapp->Nodes[i]->height;// +myapp->Nodes[i]->boosterNode->headGained;
            }
            else {
                H(i) = H1(i) + myapp->Nodes[i]->height;
            }
            std::cout << i << " H: " << H[i] << "\n";
        }
        std::vector<double> h2;
        for (int i = 0; i < sizePipes; i++) {
            //relationship between head and headloss
            h2.push_back(H[myapp->Pipes[i]->Node1->getId() - 1] - H[myapp->Pipes[i]->Node2->getId() - 1]);
            /*if (myapp->Pipes[i]->afterBooster) {
                if (myapp->Pipes[i]->boosterNode == 1) {
                    h2[i] += myapp->Pipes[i]->Node1->boosterNode->headGained;
                }
                else {
                    h2[i] += myapp->Pipes[i]->Node2->boosterNode->headGained;
                }

            }
            */
            std::cout << "h2: " << h2[i] << "\n";
        }

        std::vector<double> dev;
        std::vector<double> q2;
        std::vector<double> c2;
        std::vector<double> h3;
        errors = 0;

        for (int i = 0; i < sizePipes; i++) {

            q2.push_back( h2[i] * c[i]); //recalcualte flow from headloss  q_c
            double k = 1.f;
            if (q2[i] < 0) {
                k = -1.f;
            }
            /*
            if (myapp->Pipes[i]->afterBooster) {
                if (myapp->Pipes[i]->boosterNode == 1) {
                    q2[i] += k*c[i]*  myapp->Pipes[i]->Node1->boosterNode->headGained;
                }
                else {
                    q2[i] += k*c[i] * myapp->Pipes[i]->Node2->boosterNode->headGained;
                }

            }*/
            
            

            double Re = reynold(abs(q2[i]), myapp->Pipes[i]->diameter, 1000.f, 0.001);
            if (myapp->Pipes[i]->Node1->KValue > 0) {
                c_t[i] = (((myapp->Pipes[i]->diameter * 2.f * g * pow(A[i], 2)) / (frictionfactor(Re, i) * myapp->Pipes[i]->length)) + (2 * g * pow(A[i], 2)) / myapp->Pipes[i]->Node1->KValue);
            }
            else {
                c_t[i] = (((myapp->Pipes[i]->diameter * 2.f * g * pow(A[i], 2)) / (frictionfactor(Re, i) * myapp->Pipes[i]->length)));
            }
            if (q2[i] < 0) {
                h3.push_back(-1.f*pow(q2[i], 2) / c_t[i]); //recalcualte headloss from flow
            }
            else {
                h3.push_back(pow(q2[i], 2) / c_t[i]); //recalcualte headloss from flow
            }
            /*
            if (myapp->Pipes[i]->afterBooster) {
                if (myapp->Pipes[i]->boosterNode == 1) {
                    h3[i] -= c_t[i] * myapp->Pipes[i]->Node1->boosterNode->headGained;
                }
                else {
                    h3[i] -= c_t[i] * myapp->Pipes[i]->Node2->boosterNode->headGained;
                }

            }*/
            
          
           

            c2.push_back( q2[i] / h3[i]); // new resitance coefficient

            dev.push_back( (abs(h3[i]) / abs(h2[i])) - 1); //deviation

            std::cout << "\n" << i << " q: " << q[i] << "  " << q2[i] << "\n";
            std::cout << "\n" << i << " h: " << h2[i] << "  " << h3[i] << "\n";
            std::cout << i << " dev: " << dev[i] * 100 << "%\n";
            if (abs(dev[i]) > myapp->allowedDev) {
                std::cout << i << ": C: " << c[i] << "C2: " << c2[i] << "\n";
                c[i] = (c2[i] + c[i]) / 2.f; // new restiance coeffecient based on average
                
                q[i] = c[i] * h2[i]; // new flow
                double k = 1.f;
                if (q[i] < 0) {
                    k = -1.f;
                }
                /*
                if (myapp->Pipes[i]->afterBooster) {
                    if (myapp->Pipes[i]->boosterNode == 1) {
                        q[i] += k * c[i] * myapp->Pipes[i]->Node1->boosterNode->headGained;
                    }
                    else {
                        q[i] += k * c[i] * myapp->Pipes[i]->Node2->boosterNode->headGained;
                    }

                }*/
                
                std::cout << "q: " << q[i] << "\n";

                double Re = reynold(abs(q[i]), myapp->Pipes[i]->diameter,1000.f, 0.001);
                std::cout << "Re: " << Re << "\n";
                if (myapp->Pipes[i]->Node1->KValue > 0) {
                    c_t[i] = (((myapp->Pipes[i]->diameter * 2.f * g * pow(A[i], 2)) / (frictionfactor(Re, i) * myapp->Pipes[i]->length)) + (2 * g * pow(A[i], 2)) / myapp->Pipes[i]->Node1->KValue);
                }
                else {
                    c_t[i] = (((myapp->Pipes[i]->diameter * 2.f * g * pow(A[i], 2)) / (frictionfactor(Re, i) * myapp->Pipes[i]->length)));
                }
                std::cout << "c_t:" << c_t[i] << "\n";
                std::cout << "c(Kvalue): " << myapp->Pipes[i]->Node1->KValue / (2 * g * pow(A[i], 2)) << "\n";
                if (q[i] < 0) {
                    h[i] = -1.f*pow(q[i], 2) / c_t[i];
                }
                else {
                    h[i] = pow(q[i], 2) / c_t[i];
                }
                /*
                if (myapp->Pipes[i]->afterBooster) {
                    if (myapp->Pipes[i]->boosterNode == 1) {
                        h[i] += myapp->Pipes[i]->Node1->boosterNode->headGained;
                    }
                    else {
                        h[i] += myapp->Pipes[i]->Node2->boosterNode->headGained;
                    }

                }
               */
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