// contents of Oval.as
class oval extends MovieClip
{
	function oval()
	{
		trace("oval constructor is called");
		_x = 50;
		_y = 50;
	}
	
	function init() 
	{
		_width = 300;
		_height = 400;
	}
	
	public function onLoad () 
	{
		init();		
		trace ("onLoad called");
	}
}

