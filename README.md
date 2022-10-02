# ALGLIB.js
Alglib.js a javascript port of the ALGLIB optimization tecniques. It supports Non-Linear Constrained Gradient Optimization problems. It is built off the ALGLIB numerical analysis and data processing library. The AGS solver used by us can handle nonsmooth and nonconvex optimization problems. It has convergence guarantees, i.e. it will converge to stationary point of the function after running for some time.

## Website
Visit our website at https://pterodactylus.github.io/Alglib.js/

## Examples
1. [Basic Constrained Minimization Example](https://pterodactylus.github.io/Alglib.js/example.html)
2. [Garden Fence Area Maximization](https://pterodactylus.github.io/Alglib.js/garden_fence.html)
3. [Rosenbrock Function](https://pterodactylus.github.io/Alglib.js/rosenbrock.html)
4. [Ellipse Rectangle Area Maximization](https://pterodactylus.github.io/Alglib.js/ellipse.html)
5. [Cubic Spline Fitting](https://pterodactylus.github.io/Alglib.js/curve_fitting.html)

## Installation
You can install Alglib.js by including the Alglib.js file in your HTML or js code.

```HTML
<script src="https://cdn.jsdelivr.net/gh/Pterodactylus/Alglib.js@master/Alglib-v1.1.0.js"></script>
```

## Getting Started
This is an example of the solution of constrained optimization using [Alglib.js](https://github.com/Pterodactylus/Alglib.js)

```js
    import {Alglib} from 'https://cdn.jsdelivr.net/gh/Pterodactylus/Alglib.js@master/Alglib-v1.1.0.js'
    
    var solver = new Alglib()
      solver.promise.then(function(){
      solver.add_function((x) => 5.0*(x[0]-1)*x[0] + x[1]*x[1] + 1.0) //x is an array of fxn inputs x[0], x[1]...
      solver.solve("min", [1,1]) //Solve the equation
      console.log(solver.get_results())
    })
```

## Advanced Example
Alglib.js maximizes or minimizes a function, F(x) subject to nonlinear equality constraints having the form Gi(x)=0, inequality ones have form Hi(x)<=0.
We may have to re-arrange constraints prior to passing them to optimizer.
Here is a basic example.

```html
<script type="module">
	import {Alglib} from 'https://cdn.jsdelivr.net/gh/Pterodactylus/Alglib.js@master/Alglib-v1.1.0.js'

	var fn1 = function(x){
		//The function to be optimized
		return (2*Math.abs(x[0])+Math.abs(x[1]));
	}
	
	var feq1 = function(x){
		//The equality constraint must return Gi(x)=0
		return x[0]-1;
	}
	
	var fneq1 = function(x){
		//The inequality constraint must always have the form Hi(x)<=0
		return -x[1]-1;
	}
	
	
	let solver = new Alglib()
	solver.add_function(fn1) //Add the first equation to the solver.
	solver.add_equality_constraint(feq1)
	solver.add_less_than_or_equal_to_constraint(fneq1)
	//solver.add_greater_than_or_equal_to_constraint(fneq1)
	
	solver.promise.then(function(result) { 
		var x_guess = [0.5,0.5] //Guess the initial values of the solution.
		//var x_scale = [100,100] //Set the scale of the values in the function only positive values here.
		var s = solver.solve("min", x_guess) //Solve the equation
		//console.log(solver.get_results())
		//console.log(solver.get_status())
		//console.log(solver.get_report())
		document.getElementById("demo").value = solver.get_report()
		solver.remove() //required to free the memory in C++
	})
</script>
```

## Reference
The Alglib class starts an instance of the Alglib optimizer.

1. The `Alglib()` constructor method takes no inputs and creates a new Alglib instance.
2. The `add_function(fxn_handle)` method takes a function that has input of an array of number equal in length to the total number of parameters/variables. The function should return a single finite value to be optimized. You can only define a single optimization function.
3. The `add_equality_constraint(fxn_handle)` method takes a function that has input of an array of numbers equal in length to the total number of parameters/variables. The constraint must be of the form Gi(x)=0. You can define as many of these as you want.
4. The `add_less_than_or_equal_to_constraint(fxn_handle)` method takes a function that has input of an array of numbers equal in length to the total number of parameters/variables. The constraint must be of the form Hi(x)<=0. You can define as many of these as you want.
5. The `add_jacobian(fxn_handle)` optional method takes a function that has input of an array of numbers equal in length to the total number of parameters/variables. The order in which you add the jacobian function corresponds to the order of the optimization then the order of the equality constraints then the order of the inequality constraints. A second parameter of the funtion handle specifies the variable that you are returning the derivative for e.g. f1/x2 would be the first fxn added back with an input var of 2.
6. The `solve(min_max, x_guess, [x_scale], [max_iterations], [penalty], [radius], [diffstep], [stop_threshold])` function requires a string `min_max = "min" or "max"` an array `x_guess = [x1, x2, etc.. ]` that defines the optimizer starting point. The otimizer counts how many x_guess variables there are and bases the number of variables to optimize on that. x_scale is an optional scaling parameter to help the optimizer converge more quickly.