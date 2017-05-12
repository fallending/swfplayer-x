﻿/*The RewardPoints class is linked to a RewardPoints movie clip in the library that will get attached to stage when triggeredby classes that want to show a point value floating by a hit, etc...*/class RewardPoints extends MovieClip{	//We want to create a couple of varibles to keep track of important RewardPoints information. 	//We must define them here		//This will just keep track of whether this clip is fading in or out	var fadeDirection;	//this variable will define how fast it fades	var fadeSpeed;	//This onLoad function is a built-in function of every movie clip. 	//When a RewardPoints is first loaded onto stage (it will get attached to the stage from the library) we want to:	function onLoad()	{		//Lets start it at 25 percent transparent. We could start it at zero, but it takes too long to fade in		_alpha = 25;		//Set the direction of fade to a positive number so we can increase the alpha (making it more opaque) at frame rate to start with		fadeDirection = 1;		//Let's increment it's alpha value by five at frame rate		fadeSpeed = 5;	}		//This onEnterFrame function is a built-in function of every movie clip. 	//All the code that we need to continuously execute at 30 frames a second goes inside this function	function onEnterFrame()	{		//let's increase the alpha by 5, to make it fade in		_alpha += fadeSpeed * fadeDirection;		//if th alph reaches 100 (fully opaque) and it's still fading in		if(_alpha > 100 && fadeDirection == 1)		{			//reverse it's direction, now it will fade by -5 instead of 5, thus fding out			fadeDirection = -1;		}				//once it hs faded out completely		if(_alpha < 0)		{			//just remove it from stage			this.removeMovieClip();		}			}	} 