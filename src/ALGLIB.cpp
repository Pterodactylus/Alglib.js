#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "optimization.h"
#include <iostream>
#include <vector>
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace alglib;
using namespace emscripten;

class ALGLIBjs {
	std::vector<double> x;
	std::vector<double> xi;
	std::vector<val> f;
	std::vector<val> callbackFn;
	std::vector<val> equality_constraint;
	std::vector<val> inequality_constraint;
	std::vector<val> jacobian;
	double* xArray;
	int xArrayLen;
	double* sArray;
	int sArrayLen;
	int minmax;
	int terminationType;
	
  public:
    ALGLIBjs() {
		
	}
	void add_function(val fn){
		this->f.push_back (fn);
	}
	void add_callback(val fx){
		this->callbackFn.push_back(fx);
	}
	void add_equality_constraint(val fn){
		this->equality_constraint.push_back(fn);
	}
	void add_inequality_constraint(val fn){
		this->inequality_constraint.push_back(fn);
	}
	void add_jacobian(val fn){
		this->jacobian.push_back(fn);
	}
	void reset(){
		this->f.clear();
		this->callbackFn.clear();
		this->equality_constraint.clear();
		this->inequality_constraint.clear();
		this->jacobian.clear();
	}
	bool solve(int minmax, int max_iterations, double penalty, double rad, double dx, double stopping_conditions){
		
		this->minmax = minmax;
		//std::cout << this->minmax << std::endl;
		
		double xs[this->sArrayLen];
		for(int i=0; i<this->sArrayLen; i++){
			xs[i] = this->sArray[i];
			//std::cout << xs[i] << std::endl;
		}
		alglib::real_1d_array s;
		s.setcontent(this->xArrayLen,xs);
		
		double xi[this->xArrayLen];
		for(int i=0; i<this->xArrayLen; i++){
			xi[i] = this->xArray[i];
			//std::cout << xi[i] << std::endl;
		}
		
		alglib::real_1d_array x0;
		x0.setcontent(this->xArrayLen,xi);
		//std::cout << this->xArrayLen << std::endl;
		
		double epsx = stopping_conditions;
		double diffstep = dx;
		double radius = rad;
		double rho = penalty;
		alglib::ae_int_t maxits = max_iterations;
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
		if(this->jacobian.size()>0){
			alglib::minnscreate(this->xArrayLen, x0, state);
		}
		else{
			alglib::minnscreatef(this->xArrayLen, x0, diffstep, state);
		}
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
		int equality_constraints = this->equality_constraint.size();
		int general_nonlinear_constraints = this->inequality_constraint.size();
		alglib::minnssetnlc(state, equality_constraints, general_nonlinear_constraints);

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
		
		if(this->jacobian.size()>0){
			auto f2 = std::bind(ALGLIBjs::wrapper_jac, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			alglib::minnsoptimize(state, f2);
		}
		else{
			auto f2 = std::bind(ALGLIBjs::wrapper_vec, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			alglib::minnsoptimize(state, f2);
		}
		alglib::minnsresults(state, x1, rep);
		//std::cout << rep.terminationtype << std::endl;
		this->terminationType = rep.terminationtype;
		//printf("%s\n", x1.tostring(2).c_str());
		
		return true;
	}
	
	static void wrapper_jac (ALGLIBjs *p, const alglib::real_1d_array &x, alglib::real_1d_array &fi, real_2d_array &jac, void *ptr)
	{
	  p->nsfunc2_vec(x, fi, ptr);
	  p->nsfunc2_jac(jac);
	}
	virtual void  nsfunc2_jac(real_2d_array &jac)
	{
		for(int i=0; i<this->jacobian.size(); i++){
			for(int j=0; j<this->xArrayLen; j++){
				jac[i][j] = this->jacobian[i](j).as<double>();
			}
		}
	}
	
	static void wrapper_vec (ALGLIBjs *p, const alglib::real_1d_array &x, alglib::real_1d_array &fi, void *ptr)
	{
	  p->nsfunc2_vec(x, fi, ptr);
	}
	virtual void  nsfunc2_vec(const alglib::real_1d_array &x, alglib::real_1d_array &fi, void *ptr)
	{
		
		int f_counter = 0;
		for(int i=0; i<this->xArrayLen; i++){
			this->xArray[i] = x[i];
		}
		if(this->minmax == 1){fi[0] = this->f[0]().as<double>();}
		else if(this->minmax == 2){fi[0] = -this->f[0]().as<double>();}
		f_counter++;
		
		for(int i=0; i<this->equality_constraint.size(); i++){
			fi[f_counter] = this->equality_constraint[i]().as<double>();
			f_counter++;
		}
		
		for(int i=0; i<this->inequality_constraint.size(); i++){
			fi[f_counter] = this->inequality_constraint[i]().as<double>();
			f_counter++;
		}
	}
	void setup_x(int arrPtr, int length){
		this->xArray = reinterpret_cast<double*>(arrPtr);
		this->xArrayLen = length;
	}
	void set_xs(int arrPtr, int length){
		double* holding = reinterpret_cast<double*>(arrPtr);
		for(int i=0;i<length;i++)
        {
             this->sArray[i]=holding[i];
        }
		this->sArrayLen = length;
	}
	int get_status(){
		return this->terminationType;
	}
};

// Binding code
EMSCRIPTEN_BINDINGS(my_class_example) {
  class_<ALGLIBjs>("ALGLIBjs")
	.constructor()
	.function("setup_x", &ALGLIBjs::setup_x)
	.function("set_xs", &ALGLIBjs::set_xs)
    .function("add_function", &ALGLIBjs::add_function)
	.function("add_jacobian", &ALGLIBjs::add_jacobian)
	.function("add_callback", &ALGLIBjs::add_callback)
	.function("add_equality_constraint", &ALGLIBjs::add_equality_constraint)
	.function("add_inequality_constraint", &ALGLIBjs::add_inequality_constraint)
	.function("reset", &ALGLIBjs::reset)
	.function("solve", &ALGLIBjs::solve)
	.function("get_status", &ALGLIBjs::get_status)
    ;
}