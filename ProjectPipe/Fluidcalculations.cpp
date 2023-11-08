

// https://www-jstor-org.db.ub.oru.se/stable/41267962?searchText=anthony+g+collins+finite+element&searchUri=%2Faction%2FdoBasicSearch%3FQuery%3Danthony%2Bg%2Bcollins%2Bfinite%2Belement&ab_segments=0%2Fbasic_search_gsv2%2Fcontrol&refreqid=fastly-default%3A27749ce9afce607a8dffbbd22b7e2486

#include <iostream>
#include<Eigen\Eigen>
#include <math.h>

//Universal Constants
const double pi = 3.14159265358979323846;
const double e = 2.71828182845904523536;
const double g = 9.81672974789503;

//Program variables and constants
double D[6] = { 0.03,0.03,0.03,0.03,0.03,0.03 };
double L[6] = { 10,10,10,10,10,10 };
const double CHW = 130;
const double relativeRough = 0.01;
const double mu = 0.000739;
const double rho = 1000;
int ReyApprox[6] = { 200000,200000,200000,200000,200000,200000 };
double allowedDev = 0.05; //how much the headloss is allowed to deviate

double reynold(double flow, double diameter) {
    double A = pow(diameter / 2, 2) * pi;
    double V = flow / A;
    return rho * V * diameter / mu;

}
double frictionfactor(double Re) {

    //https://link.springer.com/chapter/10.1007/978-3-030-57340-9_37
    double ep_D = relativeRough / D[1];
    double A0 = -0.79638 * log((ep_D / 8.208) + (7.3357 / Re));
    double A1 = Re * ep_D + 9.3120665 * A0;
    double f = (8.128943 + A1) / (8.128943 * A0 - 0.86859209 * A1 * log(A1 / (3.7099535 * Re)));
    return pow(f, 2);
}

int simulate()
{
    //First set up a system of equations for all the nodes to follow continuity criteria
    // Q_n = q_a + q_b ...
    //
    //Then a second system of equations for all nodes is set up
    // Q_n = C_a(H_n-H_a) + C_b(H_n-H_b) ...
    //Flow is assumed away from the nodes


   // double Q[5]; //Node flow
    //double H[5]; //piezometric headloss
    double c_t[6]; //pipe transitional coeffecients
    double q[6]; //pipe flows
    double h[6]; //pipe headlosses
    double c[6]; // pipe coefficients
    double A[6]; // cross sectional area



    for (int i = 0; i <= 5; i++) {

        A[i] = pow((D[i] / 2), 2) * pi;
        c_t[i] = frictionfactor(ReyApprox[i]) * L[i] / (D[i] * 2 * g * pow(A[i], 2));

    }


    for (int i = 0; i <= 5; i++) {
        q[i] = ReyApprox[i] * A[i] * mu / (rho * D[i]);

        h[i] = pow(q[i], 2) / c_t[i];

        c[i] = q[i] / h[i];
    }
    int errors = 1;
    int attempts = 0;
    while (errors > 0) {


        //Create Matrix
        Eigen::MatrixXd M = Eigen::MatrixXd::Zero(5, 5); // matrix for  Q*M=H and solving for all the headloss
        M(0, 0) = c[0] + c[3];
        M(0, 1) = -c[0];
        M(0, 3) = -c[3];

        M(1, 0) = -c[0];
        M(1, 1) = c[0] + c[3];
        M(1, 2) = -c[1];

        M(2, 1) = -c[1];
        M(2, 2) = c[1] + c[2] + c[5];
        M(2, 3) = -c[4];
        M(2, 4) = -c[5];

        M(3, 0) = -c[3];
        M(3, 2) = -c[2];
        M(3, 3) = c[2] + c[3] + c[4];
        M(3, 4) = -c[4];

        M(4, 2) = -c[2];
        M(4, 3) = -c[4];
        M(4, 4) = c[4] + c[5];

        std::cout << M << "\n \n";
        // std::cout << "\n";

        //https://se.mathworks.com/matlabcentral/answers/612881-how-to-solve-ax-b-with-some-knowns-in-x-and-b
        //Slicing out parts where H is known

        /*
        Eigen::MatrixXd M11 = M(1, Eigen::placeholders::all);
        Eigen::MatrixXd M12 = M(1, Eigen::placeholders::all);
        Eigen::MatrixXd M21 = M(1, Eigen::placeholders::all);
        Eigen::MatrixXd M22 = M(1, Eigen::placeholders::all);
        */

        //trying something spicy

        Eigen::MatrixXd MH = M({ 1,2,3,4 }, { 1,2,3,4 });
        std::cout << MH << "\n \n";
        Eigen::VectorXd b(4);
        b << 0.04416313748240706, 0.025236078561375466, 0, 0.0378541178420632;
        //std::cout << b;
        Eigen::VectorXd H1 = MH.colPivHouseholderQr().solve(b);
        std::cout << H1;
        Eigen::VectorXd H = Eigen::VectorXd::Zero(5);
        H(0) = 30;
        H({ 1,2,3,4 }) = H1;

        // relationsship between piezometric head and headloss
        double h2[6];
        h2[0] = H(0) - H(1);
        h2[1] = H(1) - H(2);
        h2[2] = H(2) - H(3);
        h2[3] = H(0) - H(3);
        h2[4] = H(3) - H(4);
        h2[5] = H(2) - H(4);

        double dev[6];
        double q2[6];
        double c2[6];
        double h3[6];
        errors = 0;
        for (int i = 0; i <= 5; i++) {



            q2[i] = h2[i] * c[i]; //recalcualte flow from headloss  q_c
            double Re = reynold(q2[i], D[i]);
            c_t[i] = frictionfactor(Re) * L[i] / (D[i] * 2 * g * pow(A[i], 2)); // k
            h3[i] = pow(q2[i], 2) / c_t[i]; //recalcualte headloss from flow


            c2[i] = q2[i] / h3[i]; // new resitance coefficient


            dev[i] = (abs(h3[i]) / abs(h2[i])) - 1; //deviation


            std::cout << "\n" << i << " h: " << h2[i] << "  " << h3[i] << "\n";
            std::cout << i << " dev: " << dev[i] * 100 << "%\n";
            if (abs(dev[i]) > allowedDev) {
                std::cout << i << ": C: " << c[i] << "C2: " << c2[i] << "\n";
                c[i] = (c2[i] + c[i]) / 2; // new restiance coeffecient based on average

                q[i] = c[i] * h2[i]; // new flow

                double Re = reynold(q[i], D[i]);

                c_t[i] = frictionfactor(Re) * L[i] / (D[i] * 2 * g * pow(A[i], 2));

                h[i] = pow(q[i], 2) / c_t[i];

                c[i] = q[i] / h[i];

                errors++;

            }

        }
        std::cout << " errors: " << errors << "\n";
        attempts++;
        std::cout << "attempts: " << attempts << "\n";
    }

    std::cout << "Hello World!\n";
}