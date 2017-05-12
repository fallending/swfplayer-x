// Shape class defined in external file named Shape.as
class shape extends box
{
	var i : Number;
	
	function shape(param)
	{
		super(param);
		trace("shape constructor:" + i + ", j=" + j);
		i = param;
	}
	
	function draw()
	{
		trace("SHAPE: draw is called: i=" + i + ", j=" + j);
		super.draw();
	}

}
