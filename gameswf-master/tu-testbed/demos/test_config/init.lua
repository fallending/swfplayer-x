-- init.lua

-- example preferences file


actorprefs = property_set{
	runspeed = {
		value = 10,
		min = 1,
		max = 100,
		comment = "running speed, in meters/sec"
	},
	jumpheight = {
		value = 20,
		min = 0,
		max = 100,
		comment = "height of an ordinary jump, in meters"
	},
	color = {
		value = "red",
		options = { "red", "yellow", "blue" },
		comment = "color to use for the player"
	}
}


playerprefs = property_set{
	defaults = actorprefs,
	color = {
		value = "purple",
	}
}
