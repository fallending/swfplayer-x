#ifndef MYGAMESWF_H
#define MYGAMESWF_H

#include <functional>

/**
 *  初始化
 */
bool init_gameswf(int argc, char *argv[]);
/**
 *
 */
void set_gameswf_window(int backingWidth, int backingHeight);
/**
 *
 */
void advance_gameswf(int w, int h, std::function<void()> handler_pre_play, std::function<void()> hanler_post_play);


void onMouseDown(float x, float y);
void onMouseUp(float x, float y);
void onMouseMove(float x, float y);

struct mouse_event
{
	int m_x;
	int m_y;
	int m_state;
};

#endif
