//Ceres Helper JS

export class Alglib {
	constructor() {
		this.loaded = false
		this.fxn = []
		this.equality_constraint = []
		this.inequality_constraint = []
		this.callback = []
		this.stat = [];
		this.jacobian = [];
		this.countEvals = 0
		
		// Create example data to test float_multiply_array
		this.varLength = 0
		this.maxLength = 1000
		this.data = new Float64Array(this.maxLength);
		
		this.promise = new Promise(function(resolve, reject){
			ALGLIBModule().then(function(Module){
				this.instance = new Module.ALGLIBjs

				// Get data byte size, allocate memory on Emscripten heap, and get pointer
				let nDataBytes = this.data.length * this.data.BYTES_PER_ELEMENT;
				let dataPtr = Module._malloc(nDataBytes);

				// Copy data to Emscripten heap (directly accessed from Module.HEAPU8)
				this.dataHeap = new Float64Array(Module.HEAPF64.buffer, dataPtr, nDataBytes);
				this.dataHeap.set(new Float64Array(this.data.buffer));
				this.loaded = true
				resolve()
			}.bind(this))
		}.bind(this))
	}
	// Method
	add_function(fn) {
		this.fxn.push(fn)
	}
	// Method
	add_greater_than_or_equal_to_constraint(fn) {
		let f = function(x){
			return -fn(x);
		}
		this.equality_constraint.push(f)
	}
	// Method
	add_less_than_or_equal_to_constraint(fn) {
		this.equality_constraint.push(fn)
	}
	// Method
	add_equality_constraint(fn) {
		this.equality_constraint.push(fn)
	}
	// Method
	add_inequality_constraint(fn) {
		this.inequality_constraint.push(fn)
	}
	add_jacobian(fn){
		this.jacobian.push(fn)
	}
	// Method
	add_callback(fn) {
		this.callback.push(fn)
	}
	reset(){
		this.instance.reset();
		this.fxn = []
		this.equality_constraint = []
		this.inequality_constraint = []
		this.callback = []
		this.jacobian = []
		
	}
	//Method
	load_fxns(){
		for(let i = 0; i < this.fxn.length; i++){
			let newfunc = function f(){
				let x = new Float64Array(this.dataHeap.buffer, this.dataHeap.byteOffset, this.varLength);
				if(this.stat.length<10000){
					let str = ""
					for(let j=0; j< x.length; j++){
						str += x[j].toExponential(5)+"\t"
					}
					this.stat.push(str)
				}
				this.countEvals++
				return this.fxn[i](x)
			}
			this.instance.add_function(newfunc.bind(this));
		}
		for(let i = 0; i < this.jacobian.length; i++){
			let newfunc = function f(j){
				let x = new Float64Array(this.dataHeap.buffer, this.dataHeap.byteOffset, this.varLength);
				this.countEvals++
				return this.jacobian[i](x,j)
			}
			this.instance.add_jacobian(newfunc.bind(this));
		}
		for(let i = 0; i < this.equality_constraint.length; i++){
			let newfunc = function f(){
				let x = new Float64Array(this.dataHeap.buffer, this.dataHeap.byteOffset, this.varLength);
				this.countEvals++
				return this.equality_constraint[i](x)
			}
			this.instance.add_equality_constraint(newfunc.bind(this));
		}
		for(let i = 0; i < this.inequality_constraint.length; i++){
			let newfunc = function f(){
				let x = new Float64Array(this.dataHeap.buffer, this.dataHeap.byteOffset, this.varLength);
				this.countEvals++
				return this.inequality_constraint[i](x)
			}
			this.instance.add_inequality_constraint(newfunc.bind(this));
		}
		for(let i = 0; i < this.callback.length; i++){
			let newfunc = function f(evaluate_jacobians, new_evaluation_point){
				let x = new Float64Array(this.dataHeap.buffer, this.dataHeap.byteOffset, this.varLength);
				this.countEvals++
				return this.callback[i](x, evaluate_jacobians, new_evaluation_point);
			}
			this.instance.add_callback(newfunc.bind(this));
		}
	}
	solve(mode, xi, xs=[], max_iterations=50000, penalty=50.0, radius=0.1, diffstep=0.000001, stop_threshold=0.00001) {
		if(this.loaded == true){
			const t0 = performance.now();
			if(this.jacobian.length>0){
				let jacobian_rows = 1+this.equality_constraint.length+this.inequality_constraint.length
				if(jacobian_rows != this.jacobian.length){throw("Error: not enough jacobian functions defined. Define one for the optimization function and one for each constraint. Need "+jacobian_rows)}
			}
			
			this.countEvals = 0
			this.stat = []
			let str = ""
			for(let j=0; j< xi.length; j++){
				str += "    x"+j+"\t\t"
			}
			this.stat.push(str)
			this.mode = mode
			this.minmax = 0
			if(mode == "min"){this.minmax = 1}
			else if(mode == "max"){this.minmax = 2}
			
			if(this.varLength <= this.maxLength ){this.varLength = xi.length}
			else{throw "Max number of vars exceeded"}
			
			this.load_fxns()
			
			for(let i = 0; i < this.varLength; i++){
				if(Number.isFinite(xs[i])){
					this.dataHeap[i] = xs[i];
				}
				else{
					this.dataHeap[i] = 1
				}
			}
			this.instance.set_xs(this.dataHeap.byteOffset, this.varLength);
			
			for(let i = 0; i < this.varLength; i++){
				this.dataHeap[i] = xi[i];
			}
			this.instance.setup_x(this.dataHeap.byteOffset, this.varLength);
			
			this.instance.solve(this.minmax, max_iterations, penalty, radius, diffstep, stop_threshold)
			
			let x = new Float64Array(this.dataHeap.buffer, this.dataHeap.byteOffset, this.varLength)
			let normalArray = [].slice.call(x);
			this.results = normalArray
			this.termType = this.instance.get_status()
			
			const t1 = performance.now();
			this.timing = "Call to optimizer took "+(t1 - t0)+" milliseconds.";
			
			return true
		}
		else{
			console.log("Warning the Alglib.js wasm has not been loaded yet.")
			return false
		}
	}
	get_results(){
		return this.results
	}
	get_status(){
		/*optimization report. You should check Rep.TerminationType
		in  order  to  distinguish  successful  termination  from
		unsuccessful one:
		* -8   internal integrity control  detected  infinite  or
			   NAN   values   in   function/gradient.    Abnormal
			   termination signalled.
		* -3   box constraints are inconsistent
		* -1   inconsistent parameters were passed:
			   * penalty parameter for minnssetalgoags() is zero,
				 but we have nonlinear constraints set by minnssetnlc()
		*  2   sampling radius decreased below epsx
		*  7    stopping conditions are too stringent,
				further improvement is impossible,
				X contains best point found so far.
		*  8    User requested termination via minnsrequesttermination()*/
		var termTypeText = ""
		if(this.termType == 2){termTypeText = "Found local max/min. Sampling radius decreased below the threshold value"}
		else if(this.termType == 7){termTypeText = "Stopping conditions are too stringent. Further improvement is impossible. X contains best point found so far."}
		else if(this.termType == 8){termTypeText = "User requested termination"}
		else if(this.termType == -8){termTypeText = "Internal integrity control detected infinite or NAN values in function/gradient. Abnormal termination signalled."}
		else if(this.termType == -3){termTypeText = "Box constraints are inconsistent"}
		else if(this.termType == -1){termTypeText = "Inconsistent parameters were passed"}
		else{termTypeText = "Unknown return type"}
		return termTypeText
	}
	get_report(){
		let x = this.get_results()
		var reportText = "The solver attempted find the "+this.mode+" of f(x) subject to "+this.equality_constraint.length+" equality constraints and "+this.inequality_constraint.length+" inequality constraints.\
		\nTermination condition: \""+this.get_status()+"\"\
		\nThe final value of the optimization function was f(x) = "+this.fxn[0](x)+"\
		\nThe final variable values were: ["+this.get_results()+"]\
		\nNumber of function evaluations: "+this.countEvals+"\
		\n"+this.timing+"\
		\n\nIteration Report:\
		\n"+this.stat.join("\n")+"\
		\n"
		return reportText
	}
	remove(){
		this.loaded == false;
		this.instance.delete();
	}
}