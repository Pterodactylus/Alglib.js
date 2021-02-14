#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "optimization.h"
#include <iostream>
#include <vector>

using namespace alglib;
//using namespace emscripten;

class ALGLIBjs {
	//std::vector<double> x;
	//std::vector<double> xi;
	//std::vector<val> f;
	//int size = 0;
	//std::vector<val> callbackFn;
	//double* xArray;
	//int xArrayLen;
	//std::vector<int> lowerbound;
	//std::vector<double> lowerboundValue;
	//std::vector<int> upperbound;
	//std::vector<double> upperboundValue;
	//void (*f)(const alglib::real_1d_array &x, alglib::real_1d_array &fi, alglib::real_2d_array &jac, void *ptr);
	
  public:
    ALGLIBjs() {
		
	}
	/*void add_function(val fn){
		this->f.push_back (fn);
		this->x.push_back (0);
		this->xi.push_back (0);
		this->size++;
	}
	void add_callback(val fx){
		this->callbackFn.push_back(fx);
	}
	void add_lowerbound(int index, double value){
		this->lowerbound.push_back(index);
		this->lowerboundValue.push_back(value);
	}
	void add_upperbound(int index, double value){
		this->upperbound.push_back(index);
		this->upperboundValue.push_back(value);
	}
	void reset(){
		this->f.clear();
		this->x.clear();
		this->xi.clear();
		this->size = 0;
		this->callbackFn.clear();
		this->lowerbound.clear();
		this->lowerboundValue.clear();
		this->upperbound.clear();
		this->upperboundValue.clear();
	}*/
	bool solve(){
		
		alglib::real_1d_array x0 = "[1,1]";
		alglib::real_1d_array s = "[1,1]";
		double epsx = 0.00001;
		double diffstep = 0.000001;
		double radius = 0.1;
		double rho = 50.0;
		alglib::ae_int_t maxits = 0;
		alglib::minnsstate state;
		alglib::minnsreport rep;
		alglib::real_1d_array x1;

		//
		// Create optimizer object, choose AGS algorithm and tune its settings:
		// * radius=0.1     good initial value; will be automatically decreased later.
		// * rho=50.0       penalty coefficient for nonlinear constraints. It is your
		//                  responsibility to choose good one - large enough that it
		//                  enforces constraints, but small enough in order to avoid
		//                  extreme slowdown due to ill-conditioning.
		// * epsx=0.000001  stopping conditions
		// * s=[1,1]        all variables have unit scale
		//
		alglib::minnscreatef(2, x0, diffstep, state);
		alglib::minnssetalgoags(state, radius, rho);
		alglib::minnssetcond(state, epsx, maxits);
		alglib::minnssetscale(state, s);

		//
		// Set general nonlinear constraints.
		//
		// This part is more tricky than working with box/linear constraints - you
		// can not "pack" general nonlinear function into double precision array.
		// That's why minnssetnlc() does not accept constraints itself - only
		// constraint COUNTS are passed: first parameter is number of equality
		// constraints, second one is number of inequality constraints.
		//
		// As for constraining functions - these functions are passed as part
		// of problem Jacobian (see below).
		//
		// NOTE: MinNS optimizer supports arbitrary combination of boundary, general
		//       linear and general nonlinear constraints. This example does not
		//       show how to work with general linear constraints, but you can
		//       easily find it in documentation on minnlcsetlc() function.
		//
		alglib::minnssetnlc(state, 1, 1);

		//
		// Optimize and test results.
		//
		// Optimizer object accepts vector function and its Jacobian, with first
		// component (Jacobian row) being target function, and next components
		// (Jacobian rows) being nonlinear equality and inequality constraints
		// (box/linear ones are passed separately by means of minnssetbc() and
		// minnssetlc() calls).
		//
		// Nonlinear equality constraints have form Gi(x)=0, inequality ones
		// have form Hi(x)<=0, so we may have to "normalize" constraints prior
		// to passing them to optimizer (right side is zero, constraints are
		// sorted, multiplied by -1 when needed).
		//
		// So, our vector function has form
		//
		//     {f0,f1,f2} = { 2*|x0|+|x1|,  x0-1, -x1-1 }
		//
		// with Jacobian
		//
		//         [ 2*sign(x0)   sign(x1) ]
		//     J = [     1           0     ]
		//         [     0          -1     ]
		//
		// which means that we have optimization problem
		//
		//     min{f0} subject to f1=0, f2<=0
		//
		// which is essentially same as
		//
		//     min { 2*|x0|+|x1| } subject to x0=1, x1>=-1
		//
		// NOTE: AGS solver used by us can handle nonsmooth and nonconvex
		//       optimization problems. It has convergence guarantees, i.e. it will
		//       converge to stationary point of the function after running for some
		//       time.
		//
		//       However, it is important to remember that "stationary point" is not
		//       equal to "solution". If your problem is convex, everything is OK.
		//       But nonconvex optimization problems may have "flat spots" - large
		//       areas where gradient is exactly zero, but function value is far away
		//       from optimal. Such areas are stationary points too, and optimizer
		//       may be trapped here.
		//
		//       "Flat spots" are nonsmooth equivalent of the saddle points, but with
		//       orders of magnitude worse properties - they may be quite large and
		//       hard to avoid. All nonsmooth optimizers are prone to this kind of the
		//       problem, because it is impossible to automatically distinguish "flat
		//       spot" from true solution.
		//
		//       This note is here to warn you that you should be very careful when
		//       you solve nonsmooth optimization problems. Visual inspection of
		//       results is essential.
		//
		
		
		auto f2 = std::bind(ALGLIBjs::myfunction, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		
		alglib::minnsoptimize(state, f2);
		alglib::minnsresults(state, x1, rep);
		printf("%s\n", x1.tostring(2).c_str()); // EXPECTED: [1.0000,0.0000]
		
		return true;
	}
	static void myfunction (ALGLIBjs *p, const alglib::real_1d_array &x, alglib::real_1d_array &fi, void *ptr)
	{
	  p->nsfunc2_jac(x, fi, ptr);
	}
	virtual void  nsfunc2_jac(const alglib::real_1d_array &x, alglib::real_1d_array &fi, void *ptr)
	{
		//
		// this callback calculates function vector
		//
		//     f0(x0,x1) = 2*|x0|+x1
		//     f1(x0,x1) = x0-1
		//     f2(x0,x1) = -x1-1
		//
		// and Jacobian matrix J
		//
		//         [ df0/dx0   df0/dx1 ]
		//     J = [ df1/dx0   df1/dx1 ]
		//         [ df2/dx0   df2/dx1 ]
		//
		fi[0] = 2*fabs(double(x[0]))+fabs(double(x[1]));
		//jac[0][0] = 2*alglib::sign(x[0]);
		//jac[0][1] = alglib::sign(x[1]);
		fi[1] = x[0]-1;
		//jac[1][0] = 1;
		//jac[1][1] = 0;
		fi[2] = -x[1]-1;
		//jac[2][0] = 0;
		//jac[2][1] = -1;
	}
	/*void setup_x(){
		
	}
	std::string get_report(){
		return "";
	}
	std::string get_message(){
		return "";
	}*/
};

// Binding code
/*EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<ALGLIBjs>("ALGLIBjs")
	.constructor()
	.function("setup_x", &ALGLIBjs::setup_x)
    .function("add_function", &ALGLIBjs::add_function)
	.function("add_callback", &ALGLIBjs::add_callback)
	.function("add_upperbound", &ALGLIBjs::add_upperbound)
	.function("add_lowerbound", &ALGLIBjs::add_lowerbound)
	.function("reset", &ALGLIBjs::reset)
	.function("solve", &ALGLIBjs::solve)
	.function("get_report", &ALGLIBjs::get_report)
	.function("get_message", &ALGLIBjs::get_message)
    ;
}*/

int main () {
  ALGLIBjs* c = new ALGLIBjs();
  c->solve();
}