// TestFunction2.as -- Thatcher Ulrich 2005

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// For compiling with MTASC.  Compiles into a test movie for checking
// support of the function2 opcode, as well as a few other
// miscellaneous ActionScript features.


class TestFunction2
{
	function TestFunction2()
	{
		// Create a set of text lines, for logging.
		for (var i = 0; i < 30; i++)
		{
			_root.createTextField("line" + i, i /*depth*/, 0, i * 20, 800, 20);
		}
	}

	// Logs the argument via trace, and also visibly as text
	// fields in the movie.
	static function log(message)
	{
		trace(message);

		// Put on the screen.

		// Shift current lines up.
		for (var i = 0; i < 29; i++)
		{
			var line_next = eval("_root.line" + (i + 1));
			var line_curr = eval("_root.line" + i);

			line_curr.text = line_next.text;
		}

		// Insert new text.
		_root.line29.text = message;
	}

	function my_test_func(a : String, b : String)
	{
		log(a);
		log(b);
		log(this);
		log(super);
		log(arguments);

		// Modify arg b.
		arguments[1] = "modified b!";
		log(b);
		log(arguments[1]);
	}

	// entry point
	static function main() {
		var t = new TestFunction2();
		log("Tulsa nightlife -");
		log("filth, gin, a slut.");
		log("");
		t.my_test_func("Arg a", "Arg b");
		t.my_test_func("another test", 27);
	}
}

