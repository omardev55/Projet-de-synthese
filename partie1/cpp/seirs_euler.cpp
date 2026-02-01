#include <iostream>
#include <iomanip>

int main() {
    double rho = 1.0 / 365;
    double beta = 0.5;
    double sigma = 1.0 / 3;
    double gamma = 1.0 / 7;

    double S = 0.999, E = 0.0, I = 0.001, R = 0.0;
    double t = 0.0;
    double h = 0.1;
    double t_end = 730.0;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "time,S,E,I,R\n";
    std::cout << t << "," << S << "," << E << "," << I << "," << R << "\n";

    while (t < t_end) {
        double dS = rho * R - beta * I * S;
        double dE = beta * I * S - sigma * E;
        double dI = sigma * E - gamma * I;
        double dR = gamma * I - rho * R;

        S += h * dS;
        E += h * dE;
        I += h * dI;
        R += h * dR;
        t += h;

        std::cout << t << "," << S << "," << E << "," << I << "," << R << "\n";
    }
    return 0;
}
