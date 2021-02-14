# ALGLIB.js
Alglib.js a javascript port of the ALGLIB optimization tecniques. It supports Non-Linear Constrained Gradient Optimization problems. It is built off the ALGLIB numerical analysis and data processing library. The AGS solver used by us can handle nonsmooth and nonconvex optimization problems. It has convergence guarantees, i.e. it will converge to stationary point of the function after running for some time.

## Website
Visit our website at https://pterodactylus.github.io/Alglib.js/

## Installation
You can install Alglib.js by including the Alglib.js file in your HTML or js code.

```HTML
<script src="https://cdn.jsdelivr.net/gh/Pterodactylus/Alglib.js@master/Alglib-v1.0.0.js"></script>
```

## Basic Example
Alglib.js takes a vector of residual equations that are all equal to zero when the problem is solved. The equations can be non-linear. Here is a basic example.

```html
<script type="module">
	import {Alglib} from 'https://cdn.jsdelivr.net/gh/Pterodactylus/Alglib.js@master/Alglib-v1.0.0.js'

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
	//solver.add_callback(c1) //Add the callback to the solver.
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
2. The `add_function(fxn_handle)` method takes a function that has input of an array of number equal in length to the total number of functions. Each of the function should return a residule. The residuals returned should equal zero at the solution point i.e. F(x) = 0.
3. The `add_equality_constraint(fxn_handle)` method takes a function that has input of an array of number equal in length to the total number of functions. This callback function is run every time before a function evaluation. You can use it to print intermediate results.
4. The `add_inequality_constraint(fxn_handle)` method takes a function that has input of an array of number equal in length to the total number of functions. This callback function is run every time before a function evaluation. You can use it to print intermediate results.
5. The `solve(min_max, x_guess, x_scale(optional))` function requires an array `x_guess = [x1, x2, etc.. ]` that defines the optimizer starting point.