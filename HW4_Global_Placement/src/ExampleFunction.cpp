#include "ExampleFunction.h"
#include <math.h>
#include <stdlib.h>

// minimize W(x, y) + lambda * Σ(Db(x, y) - Mb)^2

ExampleFunction::ExampleFunction(Placement &placement)
	:placement_(placement), binNumPerSide(10), targetDensity(0), lambda(0)
{
	width_ = placement_.boundryRight() - placement_.boundryLeft();
	height_ = placement_.boundryTop() - placement_.boundryBottom();
	alpha = width_ / 600;
	binNum = binNumPerSide * binNumPerSide;
	binW = width_ / binNumPerSide;
	binH = height_ / binNumPerSide;
	gradient_temp.resize(placement_.numModules() * 2);
	dExp.resize(placement_.numModules() * 4);
	for(unsigned i = 0; i < placement_.numModules(); ++i){
		targetDensity += placement_.module(i).area();
	}
	targetDensity /= (width_ * height_);
	binDensity.resize(binNum);
}

void ExampleFunction::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{
	double sx, nsx, sy, nsy; // parameter for LSE WL approach
	double alphaX, alphaY, betaX, betaY, dX, dY, AdX, AdY, NormalizationFactor, Ox, Oy; // parameter for Density smoothing 
	int gsize = g.size();
	g.clear();
	g.assign(gsize, 0);

    // Σ(ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r)) << LSE WL Function
    // calculate ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r) vvv
    for(unsigned int index = 0; index < placement_.numModules(); ++index){
    	if(placement_.module(index).isFixed()){
    		Module current_mod = placement_.module(index);
    		dExp.at(4 * index  ) = exp(current_mod.centerX() / alpha);				//xk
    		dExp.at(4 * index + 1) = exp((-1) * current_mod.centerX()  / alpha);	//-xk
    		dExp.at(4 * index + 2) = exp(current_mod.centerY()  / alpha);			//yk
    		dExp.at(4 * index + 3) = exp((-1) * current_mod.centerY()  / alpha);	//-yk
    	}
    	else{
    		dExp.at(4 * index  ) = exp(x.at(2 * index) / alpha);
    		dExp.at(4 * index + 1) = exp((-1) * x.at(2 * index) / alpha);
    		dExp.at(4 * index + 2) = exp(x.at(2 * index + 1) / alpha);
    		dExp.at(4 * index + 3) = exp((-1) * x.at(2 * index + 1) / alpha);
    	}
    }
    // calculate ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r) ^^^
    // calculate alpha * (Σ(ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r))) vvv

    f = 0; // objective function initializtion

    for(unsigned int i = 0; i < placement_.numNets(); ++i){
    	sx = nsx = sy = nsy = 0;
    	for(unsigned int p = 0; p < placement_.net(i).numPins(); ++p){
    		int current_id = placement_.net(i).pin(p).moduleId();
    		sx += dExp.at(4 * current_id);
    		nsx += dExp.at(4 * current_id + 1);
    		sy += dExp.at(4 * current_id + 2);
    		nsy += dExp.at(4 * current_id + 3);
    	}
    	f += alpha * (log(sx) + log(nsx) + log(sy) + log(nsy));
    	// calculate alpha * (Σ(ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r)))  ^^^
    	// calculate gradient vvv
    	for(unsigned int p = 0; p < placement_.net(i).numPins(); ++p){
    		int current_id = placement_.net(i).pin(p).moduleId();
    		if(!placement_.module(current_id).isFixed()){
    			g.at(2 * current_id) += dExp.at(4 * current_id) / (alpha * sx);
    			g.at(2 * current_id) -= dExp.at(4 * current_id + 1) / (alpha * nsx);
    			g.at(2 * current_id + 1) += dExp.at(4 * current_id + 2) / (alpha * sy);
    			g.at(2 * current_id + 1) -= dExp.at(4 * current_id + 3) / (alpha * nsy);
    		}
    		else
    			g.at(2 * current_id) = g.at(2 * current_id + 1) = 0;
    	}
    	// calculate gradient ^^^
    }
	
	if(lambda == 0) return; // W(x, y) + 0 * Σ(Db(x, y) - Mb)^2

	// Initialize density constraint / density smoothing
	binDensity.clear();
	binDensity.assign(binNum, 0);

	gradient_temp.clear();
	gradient_temp.assign(placement_.numModules() * 2, 0);

	//Compute the block area in each bin to obtain the bin density

	for(int row = 0; row < binNumPerSide; ++row){
		for(int column = 0; column < binNumPerSide; ++column){
			for(unsigned index = 0; index < placement_.numModules(); ++index){
				Module current_mod = placement_.module(index);
				if(!current_mod.isFixed()){
					alphaX = 4 / ((current_mod.width() + 2 * binW) * (current_mod.width() + 4 * binW));
					alphaY = 4 / ((current_mod.height() + 2 * binH) * (current_mod.height() + 4 * binH));
					betaX = 2 / (binW * (current_mod.width() + 4 * binW));
					betaY = 2 / (binH * (current_mod.height() + 4 * binH));
					NormalizationFactor = current_mod.area() / (binW * binH); // C of modulei

					// calculate dX = |Xcurrent - Xbin|
					dX = x.at(2 * index) - ((column + 0.5) * binW + placement_.boundryLeft());
					dY = x.at(2 * index + 1) - ((row + 0.5) * binW + placement_.boundryBottom());
					AdX = fabs(dX);
					AdY = fabs(dY);

					// bell-shaped range check for x direction
					if(AdX <= (current_mod.width() / 2 + binW)){
						Ox = 1 - alphaX * AdX * AdX;
					}
					else if(AdX >= (current_mod.width() / 2 + binW) && AdX <= (current_mod.width() / 2 + 2 * binW)){
						Ox = betaX * (AdX - 2 * current_mod.width() - 2 * binW) * (AdX - 2 * current_mod.width() - 2 * binW);
					}
					else{
						Ox = 0;
					}
					// bell-shaped range check for y direction
					if(AdY <= (current_mod.height() / 2 + binH)){
						Oy = 1 - alphaY * AdY * AdY;
					}
					else if(AdY >= (current_mod.height() / 2 + binH) && AdY <= (current_mod.height() / 2 + 2 * binH)){
						Oy = betaY * (AdY - 2 * current_mod.height() - 2 * binH) * (AdY - 2 * current_mod.height() - 2 * binH);
					}
					else{
						Oy = 0;
					}
					// bell-shape smooth done ^^^
					// calculate gradient x vvv
					if(AdX <= (current_mod.width() / 2 + binW)){
						gradient_temp.at(2 * index) = NormalizationFactor * ((-2) * alphaX * dX) * Oy;
					}
					else if(AdX >= (current_mod.width() / 2 + binW) && AdX <= (current_mod.width() / 2 + 2 * binW)){
						if(dX > 0){
							gradient_temp.at(2 * index) = NormalizationFactor * 2 * betaX * (AdX - 2 * binW - 2* current_mod.width()) * Oy;
						}
						else{
							gradient_temp.at(2 * index) = NormalizationFactor * -2 * betaX * (AdX - 2* binW - 2 * current_mod.width()) * Oy;
						}
					}
					else{
						gradient_temp.at(2 * index) = 0;
					}
					// calculate gradient y vvv
					if(AdY <= (current_mod.height() / 2 + binH)){
						gradient_temp.at(2 * index + 1) = NormalizationFactor * ((-2) * alphaY * dY) * Ox;
					}
					else if(AdY >= (current_mod.height() / 2 + binH) && AdY <= (current_mod.height() / 2 + 2 * binH)){
						if(dY > 0){
							gradient_temp.at(2 * index + 1) = NormalizationFactor * 2 * betaY * (AdY - 2 * binH - 2 * current_mod.height()) * Ox;
						}
						else{
							gradient_temp.at(2 * index + 1) = NormalizationFactor * -2 * betaY * (AdY - 2 * binH - 2 * current_mod.height()) * Ox;
						}
					}
					else{
						gradient_temp.at(2 * index + 1) = 0;
					}
					// calculate gradient done ^^^
					// bin density sum
					binDensity.at(row * binNumPerSide + column) += NormalizationFactor * Ox * Oy;
				}
			}
			// final f function = alpha * (Σ(ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r))) + lambda * (D - M)^2
			f += lambda * (binDensity.at(row * binNumPerSide + column) - targetDensity) * (binDensity.at(row * binNumPerSide + column) - targetDensity);
			for(unsigned index = 0; index < placement_.numModules(); ++index){
				g.at(2 * index) += lambda * 2 * (binDensity.at(row * binNumPerSide + column) - targetDensity) * gradient_temp.at(2 * index);
				g.at(2 * index + 1) += lambda * 2 * (binDensity.at(row * binNumPerSide + column) - targetDensity) * gradient_temp.at(2 * index + 1);
			}
		}
	}
}

void ExampleFunction::evaluateF(const vector<double> &x, double &f){
	double sx, nsx, sy, nsy; // parameter for LSE WL approach
	double alphaX, alphaY, betaX, betaY, dX, dY, AdX, AdY, NormalizationFactor, Ox, Oy; // parameter for Density smoothing 
	

    // Σ(ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r)) << LSE WL Function
    // calculate ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r) vvv
    for(unsigned int index = 0; index < placement_.numModules(); ++index){
    	if(placement_.module(index).isFixed()){
    		Module current_mod = placement_.module(index);
    		dExp.at(4 * index  ) = exp(current_mod.centerX() / alpha);				//xk
    		dExp.at(4 * index + 1) = exp((-1) * current_mod.centerX()  / alpha);	//-xk
    		dExp.at(4 * index + 2) = exp(current_mod.centerY()  / alpha);			//yk
    		dExp.at(4 * index + 3) = exp((-1) * current_mod.centerY()  / alpha);	//-yk
    	}
    	else{
    		dExp.at(4 * index  ) = exp(x.at(2 * index) / alpha);
    		dExp.at(4 * index + 1) = exp((-1) * x.at(2 * index) / alpha);
    		dExp.at(4 * index + 2) = exp(x.at(2 * index + 1) / alpha);
    		dExp.at(4 * index + 3) = exp((-1) * x.at(2 * index + 1) / alpha);
    	}
    }
    // calculate ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r) ^^^
    // calculate alpha * (Σ(ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r))) vvv

    f = 0; // objective function initializtion

    for(unsigned int i = 0; i < placement_.numNets(); ++i){
    	sx = nsx = sy = nsy = 0;
    	for(unsigned int p = 0; p < placement_.net(i).numPins(); ++p){
    		int current_id = placement_.net(i).pin(p).moduleId();
    		sx += dExp.at(4 * current_id);
    		nsx += dExp.at(4 * current_id + 1);
    		sy += dExp.at(4 * current_id + 2);
    		nsy += dExp.at(4 * current_id + 3);
    	}
    	f += alpha * (log(sx) + log(nsx) + log(sy) + log(nsy));
    	// calculate alpha * (Σ(ln(Σexp(Xk/r)) + ln(Σexp(-Xk/r) + ln(Σexp(Yk/r) + ln(Σexp(-Yk/r)))  ^^^
    	// calculate gradient vvv
    }
	
	if(lambda == 0) return; // W(x, y) + 0 * Σ(Db(x, y) - Mb)^2

	// Initialize density constraint / density smoothing
	binDensity.clear();
	binDensity.assign(binNum, 0);

	//Compute the block area in each bin to obtain the bin density

	for(int row = 0; row < binNumPerSide; ++row){
		for(int column = 0; column < binNumPerSide; ++column){
			for(unsigned index = 0; index < placement_.numModules(); ++index){
				Module current_mod = placement_.module(index);
				if(!current_mod.isFixed()){
					alphaX = 4 / ((current_mod.width() + 2 * binW) * (current_mod.width() + 4 * binW));
					alphaY = 4 / ((current_mod.height() + 2 * binH) * (current_mod.height() + 4 * binH));
					betaX = 2 / (binW * (current_mod.width() + 4 * binW));
					betaY = 2 / (binH * (current_mod.height() + 4 * binH));
					NormalizationFactor = current_mod.area() / (binW * binH); // C of modulei

					// calculate dX = |Xcurrent - Xbin|
					dX = x.at(2 * index) - ((column + 0.5) * binW + placement_.boundryLeft());
					dY = x.at(2 * index + 1) - ((row + 0.5) * binW + placement_.boundryBottom());
					AdX = fabs(dX);
					AdY = fabs(dY);

					// bell-shaped range check for x direction
					if(AdX <= (current_mod.width() / 2 + binW / 2)){
						Ox = 1 - alphaX * AdX * AdX;
					}
					else if(AdX >= (current_mod.width() / 2 + binW / 2) && AdX <= (current_mod.width() / 2 + binW)){
						Ox = betaX * (AdX - current_mod.width() - binW / 2) * (AdX - current_mod.width() - binW / 2);
					}
					else{
						Ox = 0;
					}
					// bell-shaped range check for y direction
					if(AdY <= (current_mod.height() / 2 + binH / 2)){
						Oy = 1 - alphaY * AdY * AdY;
					}
					else if(AdY >= (current_mod.height() / 2 + binW / 2) && AdY <= (current_mod.height() / 2 + binH)){
						Oy = betaY * (AdY - current_mod.height() - binH / 2) * (AdY - current_mod.height() - binH / 2);
					}
					else{
						Oy = 0;
					}
					// bell-shape smooth done ^^^
					// bin density sum
					binDensity.at(row * binNumPerSide + column) += NormalizationFactor * Ox * Oy;
				}
			}
			f += lambda * (binDensity.at(row * binNumPerSide + column) - targetDensity) * (binDensity.at(row * binNumPerSide + column) - targetDensity);
		}
	}   
}

unsigned ExampleFunction::dimension()
{
    return placement_.numModules() * 2; // num_blocks*2
    // each two dimension represent the X and Y dimensions of each block
}
