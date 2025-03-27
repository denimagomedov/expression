#include "expression.hpp"
#include <iostream>
#include <complex>

using namespace std;

int main() {
    Expression<double> x("x");
    auto f = pow(x, 2.0) + sin(x);
    
    cout << "f(x) = " << f.toString() << endl;
    
    auto df = f.derivative("x");
    cout << "f'(x) = " << df.toString() << endl;
    
    map<string, double> vars = {{"x", 1.5}};
    cout << "f(1.5) = " << f.evaluate(vars) << endl;
    cout << "f'(1.5) = " << df.evaluate(vars) << endl;
    
    Expression<complex<double>> z("z");
    auto g = exp(z) + pow(z, complex<double>(2.0, 0.0));
    
    cout << "\ng(z) = " << g.toString() << endl;
    
    auto dg = g.derivative("z");
    cout << "g'(z) = " << dg.toString() << endl;
    
    map<string, complex<double>> cvars = {{"z", complex<double>(1.0, 1.0)}};
    cout << "g(1+i) = " << g.evaluate(cvars) << endl;
    cout << "g'(1+i) = " << dg.evaluate(cvars) << endl;
    
    return 0;
}
