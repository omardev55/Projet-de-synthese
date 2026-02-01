#include <iostream>
#include <iomanip>
#include <array>

std::array<double,4> deriv(double S, double E, double I, double R) {
    double rho = 1.0/365, beta=0.5, sigma=1.0/3, gamma=1.0/7;
    return {
        rho * R - beta * I * S,
        beta * I * S - sigma * E,
        sigma * E - gamma * I,
        gamma * I - rho * R
    };
}

int main() {
    std::array<double,4> y = {0.999, 0.0, 0.001, 0.0};
    double t = 0.0, h = 0.1, t_end = 730.0;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "time,S,E,I,R\n";
    std::cout << t << "," << y[0] << "," << y[1] << "," << y[2] << "," << y[3] << "\n";

    while (t < t_end) {
        auto k1 = deriv(y[0], y[1], y[2], y[3]);
        std::array<double,4> y2;
        for(int i=0; i<4; i++) y2[i] = y[i] + 0.5 * h * k1[i];

        auto k2 = deriv(y2[0], y2[1], y2[2], y2[3]);
        std::array<double,4> y3;
        for(int i=0; i<4; i++) y3[i] = y[i] + 0.5 * h * k2[i];

        auto k3 = deriv(y3[0], y3[1], y3[2], y3[3]);
        std::array<double,4> y4;
        for(int i=0; i<4; i++) y4[i] = y[i] + h * k3[i];

        auto k4 = deriv(y4[0], y4[1], y4[2], y4[3]);

        for(int i=0; i<4; i++) {
            y[i] += (h/6.0) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
        }
        t += h;

        std::cout << t << "," << y[0] << "," << y[1] << "," << y[2] << "," << y[3] << "\n";
    }
    return 0;
}
