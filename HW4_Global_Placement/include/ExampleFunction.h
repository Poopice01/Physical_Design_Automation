#ifndef EXAMPLEFUNCTION_H
#define EXAMPLEFUNCTION_H

#include "Placement.h"
#include "NumericalOptimizerInterface.h"

class ExampleFunction : public NumericalOptimizerInterface
{
public:
    ExampleFunction(Placement &placement);

    void evaluateFG(const vector<double> &x, double &f, vector<double> &g);
    void evaluateF(const vector<double> &x, double &f);
    void addLambdaWeight() { lambda += 5000; }
    unsigned dimension();

private:
	Placement &placement_;
	int binNumPerSide;
	double targetDensity;
	double lambda;
	double alpha;
	double width_;
	double height_;
	double binW, binH;
	int binNum;
	vector<double> gradient_temp;
	vector<double> dExp;
	vector<double> binDensity;
};

#endif // EXAMPLEFUNCTION_H
