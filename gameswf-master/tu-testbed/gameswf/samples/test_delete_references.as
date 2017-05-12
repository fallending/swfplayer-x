// Compile like this:
//
// ~/src/mtasc-1.13/mtasc.exe test_delete_references.as -main -header 640:480:30:ffffff -swf test_delete_references.swf

// Test delete operator.

class Test {
	// entry point
	static function main() {
 		trace("Hello!");

                var x : Object = new Object;
		var y : Object = new Object;
		x.x = "x";
		y.y = "y";
		x.z = y;

		delete x.z;
		trace(x.z);  // should be undefined
		trace(y.y);  // should be "y"
	}
}
