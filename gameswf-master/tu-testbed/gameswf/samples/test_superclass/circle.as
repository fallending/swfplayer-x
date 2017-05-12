// Circle class defined in external file named Circle.as
class circle extends shape
{
	var i : Number;
	var j : Number;
	
	function circle(param)
	{
		super(param);
		i = param;
		j = 10*param;
		trace("circle constructor: " + i + ", j=" + j);
	}

	function draw()
	{
		trace("CIRCLE: draw is called: i=" + i + ", j=" + j);
		super.draw();
	}
	
}
