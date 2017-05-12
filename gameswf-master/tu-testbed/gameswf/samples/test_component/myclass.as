// sample of component class
// myvar is a component parameter
// it has been set up by Flash using onConstruct event
dynamic class myclass extends MovieClip
{
	var m_parameter : Number;
	
	function myclass()
	{
		trace("instance of myclass is created");
		m_parameter = this.myvar;
		trace(m_parameter);		
		this.txt.text = m_parameter;
	}
}

