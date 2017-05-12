// clip.hpp	-- Thatcher Ulrich <tu@tulrich.com>

// This code has been donated to the Public Domain.

// Declare a quickie frustum-culling interface for use by quadsquare::Render().


#ifndef CLIP_HPP
#define CLIP_HPP


namespace Clip {
	enum Visibility { NO_CLIP, SOME_CLIP, NOT_VISIBLE };
	Visibility	ComputeBoxVisibility(const float min[3], const float max[3]);
};


#endif // CLIP_HPP
