class myButton extends MovieClip
{
	function myButton()
	{
		trace("myButton instance is created: width=" + width + ", height=" + height + ", frames=" + _totalframes);
		stop();
	}

	function onPress() 
	{
		trace("onPress");
		gotoAndStop(2);
	}
	
	function onRelease()
	{
		trace("onRelease");
		gotoAndStop(1);
	}
	
	[Inspectable(defaultValue=77)]
	public function get width():Number
	{
		return _width;
	}
	
	public function set width(value:Number)
	{
		_width = value;
	}
	
	[Inspectable(defaultValue=77)]
	public function get height():Number
	{
		return _height;
	}
	
	public function set height(value:Number)
	{
		_height = value;
	}	
}