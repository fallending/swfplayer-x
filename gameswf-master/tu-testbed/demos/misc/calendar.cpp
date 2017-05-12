// calendar.cpp	-- by Thatcher Ulrich http://tulrich.com

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Generate Day-Timer sized planner sheets that are so fricking hard
// to find at Staples.


#include <stdio.h>
#include "base/postscript.h"
#include "base/container.h"
#include "base/tu_file.h"


const int ROWS = 6;
const int COLS = 7;

struct month_grid
{
	const char*	m_month_name;
	int	m_year;
	int	m_day_number[ROWS][COLS];
};


// Use cal.pl to generate month initializers


month_grid	year_2003[12] = {
	{ "January", 2003, {
		{ 0, 0, 0, 1, 2, 3, 4,},
		{ 5, 6, 7, 8, 9,10,11,},
		{12,13,14,15,16,17,18,},
		{19,20,21,22,23,24,25,},
		{26,27,28,29,30,31, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "February", 2003, {
		{ 0, 0, 0, 0, 0, 0, 1,},
		{ 2, 3, 4, 5, 6, 7, 8,},
		{ 9,10,11,12,13,14,15,},
		{16,17,18,19,20,21,22,},
		{23,24,25,26,27,28, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "March", 2003, {
		{ 0, 0, 0, 0, 0, 0, 1,},
		{ 2, 3, 4, 5, 6, 7, 8,},
		{ 9,10,11,12,13,14,15,},
		{16,17,18,19,20,21,22,},
		{23,24,25,26,27,28,29,},
		{30,31, 0, 0, 0, 0, 0,},
	}},
	{ "April", 2003, {
		{ 0, 0, 1, 2, 3, 4, 5,},
		{ 6, 7, 8, 9,10,11,12,},
		{13,14,15,16,17,18,19,},
		{20,21,22,23,24,25,26,},
		{27,28,29,30, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "May", 2003, {
		{ 0, 0, 0, 0, 1, 2, 3,},
		{ 4, 5, 6, 7, 8, 9,10,},
		{11,12,13,14,15,16,17,},
		{18,19,20,21,22,23,24,},
		{25,26,27,28,29,30,31,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "June", 2003, {
		{ 1, 2, 3, 4, 5, 6, 7,},
		{ 8, 9,10,11,12,13,14,},
		{15,16,17,18,19,20,21,},
		{22,23,24,25,26,27,28,},
		{29,30, 0, 0, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "July", 2003, {
		{ 0, 0, 1, 2, 3, 4, 5,},
		{ 6, 7, 8, 9,10,11,12,},
		{13,14,15,16,17,18,19,},
		{20,21,22,23,24,25,26,},
		{27,28,29,30,31, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "August", 2003, {
		{ 0, 0, 0, 0, 0, 1, 2,},
		{ 3, 4, 5, 6, 7, 8, 9,},
		{10,11,12,13,14,15,16,},
		{17,18,19,20,21,22,23,},
		{24,25,26,27,28,29,30,},
		{31, 0, 0, 0, 0, 0, 0,},
	}},
	{ "September", 2003, {
		{ 0, 1, 2, 3, 4, 5, 6,},
		{ 7, 8, 9,10,11,12,13,},
		{14,15,16,17,18,19,20,},
		{21,22,23,24,25,26,27,},
		{28,29,30, 0, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "October", 2003, {
		{ 0, 0, 0, 1, 2, 3, 4,},
		{ 5, 6, 7, 8, 9,10,11,},
		{12,13,14,15,16,17,18,},
		{19,20,21,22,23,24,25,},
		{26,27,28,29,30,31, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "November", 2003, {
		{ 0, 0, 0, 0, 0, 0, 1,},
		{ 2, 3, 4, 5, 6, 7, 8,},
		{ 9,10,11,12,13,14,15,},
		{16,17,18,19,20,21,22,},
		{23,24,25,26,27,28,29,},
		{30, 0, 0, 0, 0, 0, 0,},
	}},
	{ "December", 2003, {
		{ 0, 1, 2, 3, 4, 5, 6,},
		{ 7, 8, 9,10,11,12,13,},
		{14,15,16,17,18,19,20,},
		{21,22,23,24,25,26,27,},
		{28,29,30,31, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
};


month_grid	year_2004[12] = {
	{ "January", 2004, {
		{ 0, 0, 0, 0, 1, 2, 3,},
		{ 4, 5, 6, 7, 8, 9,10,},
		{11,12,13,14,15,16,17,},
		{18,19,20,21,22,23,24,},
		{25,26,27,28,29,30,31,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "February", 2004, {
		{ 1, 2, 3, 4, 5, 6, 7,},
		{ 8, 9,10,11,12,13,14,},
		{15,16,17,18,19,20,21,},
		{22,23,24,25,26,27,28,},
		{29, 0, 0, 0, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "March", 2004, {
		{ 0, 1, 2, 3, 4, 5, 6,},
		{ 7, 8, 9,10,11,12,13,},
		{14,15,16,17,18,19,20,},
		{21,22,23,24,25,26,27,},
		{28,29,30,31, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "April", 2004, {
		{ 0, 0, 0, 0, 1, 2, 3,},
		{ 4, 5, 6, 7, 8, 9,10,},
		{11,12,13,14,15,16,17,},
		{18,19,20,21,22,23,24,},
		{25,26,27,28,29,30, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "May", 2004, {
		{ 0, 0, 0, 0, 0, 0, 1,},
		{ 2, 3, 4, 5, 6, 7, 8,},
		{ 9,10,11,12,13,14,15,},
		{16,17,18,19,20,21,22,},
		{23,24,25,26,27,28,29,},
		{30,31, 0, 0, 0, 0, 0,},
	}},
	{ "June", 2004, {
		{ 0, 0, 1, 2, 3, 4, 5,},
		{ 6, 7, 8, 9,10,11,12,},
		{13,14,15,16,17,18,19,},
		{20,21,22,23,24,25,26,},
		{27,28,29,30, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "July", 2004, {
		{ 0, 0, 0, 0, 1, 2, 3,},
		{ 4, 5, 6, 7, 8, 9,10,},
		{11,12,13,14,15,16,17,},
		{18,19,20,21,22,23,24,},
		{25,26,27,28,29,30,31,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "August", 2004, {
		{ 1, 2, 3, 4, 5, 6, 7,},
		{ 8, 9,10,11,12,13,14,},
		{15,16,17,18,19,20,21,},
		{22,23,24,25,26,27,28,},
		{29,30,31, 0, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "September", 2004, {
		{ 0, 0, 0, 1, 2, 3, 4,},
		{ 5, 6, 7, 8, 9,10,11,},
		{12,13,14,15,16,17,18,},
		{19,20,21,22,23,24,25,},
		{26,27,28,29,30, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "October", 2004, {
		{ 0, 0, 0, 0, 0, 1, 2,},
		{ 3, 4, 5, 6, 7, 8, 9,},
		{10,11,12,13,14,15,16,},
		{17,18,19,20,21,22,23,},
		{24,25,26,27,28,29,30,},
		{31, 0, 0, 0, 0, 0, 0,},
	}},
	{ "November", 2004, {
		{ 0, 1, 2, 3, 4, 5, 6,},
		{ 7, 8, 9,10,11,12,13,},
		{14,15,16,17,18,19,20,},
		{21,22,23,24,25,26,27,},
		{28,29,30, 0, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "December", 2004, {
		{ 0, 0, 0, 1, 2, 3, 4,},
		{ 5, 6, 7, 8, 9,10,11,},
		{12,13,14,15,16,17,18,},
		{19,20,21,22,23,24,25,},
		{26,27,28,29,30,31, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
};


month_grid	year_2005[12] = {
	{ "January", 2005, {
		{ 0, 0, 0, 0, 0, 0, 1,},
		{ 2, 3, 4, 5, 6, 7, 8,},
		{ 9,10,11,12,13,14,15,},
		{16,17,18,19,20,21,22,},
		{23,24,25,26,27,28,29,},
		{30,31, 0, 0, 0, 0, 0,},
	}},
	{ "February", 2005, {
		{ 0, 0, 1, 2, 3, 4, 5,},
		{ 6, 7, 8, 9,10,11,12,},
		{13,14,15,16,17,18,19,},
		{20,21,22,23,24,25,26,},
		{27,28, 0, 0, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "March", 2005, {
		{ 0, 0, 1, 2, 3, 4, 5,},
		{ 6, 7, 8, 9,10,11,12,},
		{13,14,15,16,17,18,19,},
		{20,21,22,23,24,25,26,},
		{27,28,29,30,31, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "April", 2005, {
		{ 0, 0, 0, 0, 0, 1, 2,},
		{ 3, 4, 5, 6, 7, 8, 9,},
		{10,11,12,13,14,15,16,},
		{17,18,19,20,21,22,23,},
		{24,25,26,27,28,29,30,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "May", 2005, {
		{ 1, 2, 3, 4, 5, 6, 7,},
		{ 8, 9,10,11,12,13,14,},
		{15,16,17,18,19,20,21,},
		{22,23,24,25,26,27,28,},
		{29,30,31, 0, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "June", 2005, {
		{ 0, 0, 0, 1, 2, 3, 4,},
		{ 5, 6, 7, 8, 9,10,11,},
		{12,13,14,15,16,17,18,},
		{19,20,21,22,23,24,25,},
		{26,27,28,29,30, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "July", 2005, {
		{ 0, 0, 0, 0, 0, 1, 2,},
		{ 3, 4, 5, 6, 7, 8, 9,},
		{10,11,12,13,14,15,16,},
		{17,18,19,20,21,22,23,},
		{24,25,26,27,28,29,30,},
		{31, 0, 0, 0, 0, 0, 0,},
	}},
	{ "August", 2005, {
		{ 0, 1, 2, 3, 4, 5, 6,},
		{ 7, 8, 9,10,11,12,13,},
		{14,15,16,17,18,19,20,},
		{21,22,23,24,25,26,27,},
		{28,29,30,31, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "September", 2005, {
		{ 0, 0, 0, 0, 1, 2, 3,},
		{ 4, 5, 6, 7, 8, 9,10,},
		{11,12,13,14,15,16,17,},
		{18,19,20,21,22,23,24,},
		{25,26,27,28,29,30, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "October", 2005, {
		{ 0, 0, 0, 0, 0, 0, 1,},
		{ 2, 3, 4, 5, 6, 7, 8,},
		{ 9,10,11,12,13,14,15,},
		{16,17,18,19,20,21,22,},
		{23,24,25,26,27,28,29,},
		{30,31, 0, 0, 0, 0, 0,},
	}},
	{ "November", 2005, {
		{ 0, 0, 1, 2, 3, 4, 5,},
		{ 6, 7, 8, 9,10,11,12,},
		{13,14,15,16,17,18,19,},
		{20,21,22,23,24,25,26,},
		{27,28,29,30, 0, 0, 0,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
	{ "December", 2005, {
		{ 0, 0, 0, 0, 1, 2, 3,},
		{ 4, 5, 6, 7, 8, 9,10,},
		{11,12,13,14,15,16,17,},
		{18,19,20,21,22,23,24,},
		{25,26,27,28,29,30,31,},
		{ 0, 0, 0, 0, 0, 0, 0,},
	}},
};


const float	WIDTH = 6.875f * 72;
const float HEIGHT = 6.5f * 72;
const float	X0 = (612 - WIDTH) / 2;
const float	Y0 = 72;
const float	TOP = Y0 + HEIGHT;
const float	RIGHT = X0 + WIDTH;


// Some helpers.

static postscript*	s_ps = NULL;
static float s_x0(X0), s_x1(X0 + WIDTH), s_y0(Y0), s_y1(Y0 + HEIGHT);

void	set_box(float x0, float x1, float y0, float y1)
{
	s_x0 = x0;
	s_x1 = x1;
	s_y0 = y0;
	s_y1 = y1;
}

void	show_box()
{
	s_ps->rectangle(s_x0, s_x1, s_y0, s_y1);
}


void	fill_box()
{
	s_ps->box(s_x0, s_x1, s_y0, s_y1);
}


void	vrule(float x)
{
	s_ps->line(x, s_y0, x, s_y1);
}


void	hrule(float y)
{
	s_ps->line(s_x0, y, s_x1, y);
}


void	thinline()
{
	s_ps->linewidth(0.1f);
}


void	thickline()
{
	s_ps->linewidth(0.5f);
}


void	do_page(const month_grid& m, bool left_half, bool right_half)
{
	s_ps->black();
	s_ps->font("Arial", 10);
//	s_ps->rectangle(X0, X0 + WIDTH, Y0, Y0 + HEIGHT);
	set_box(X0, RIGHT, Y0, TOP);
	thinline();
	show_box();

	// Line down the middle.
	vrule(X0 + WIDTH / 2);

	float	column_width = WIDTH / 8;

	// Top row: day names.
	float	top_row_height = 20;

	// Title.
	if (right_half)
	{
		// Right column; title & notes.
		float	x = X0 + WIDTH - column_width;
		vrule(x);
		s_ps->printf(x + 10, s_y1 - 20, m.m_month_name);
		s_ps->printf(x + 10, s_y1 - 40, "%d", m.m_year);
	}
	if (left_half)
	{
		// Mini-title, for double-checking book layout.
		s_ps->printf(X0 + WIDTH / 2 - 90, s_y0 + 3, "%s %d", m.m_month_name, m.m_year);
	}

	for (int i = 0; i < 7; i++)
	{
		if (left_half == false && i < 4) continue;
		if (right_half == false && i >= 4) continue;

		static const char*	day_name[7] = { "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY" };
		float	x = X0 + column_width * i;
		set_box(x, x + column_width, TOP - top_row_height, TOP);
		s_ps->gray(0.75f);
		fill_box();
		s_ps->black();
		show_box();

		s_ps->font("Arial", 8);
		s_ps->printf(x + 5, TOP - top_row_height + 3, day_name[i]);
	}

	float	row_height = (HEIGHT - top_row_height) / ROWS;

	// Days.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			if (left_half == false && col < 4) continue;
			if (right_half == false && col >= 4) continue;

			int	day_number = m.m_day_number[row][col];
			if (day_number == 0)
			{
				// Not a day.
				continue;
			}

			// Show this day.
			float	x = X0 + column_width * col;
			float	y = Y0 + row_height * (ROWS - 1 - row);
			set_box(x, x + column_width, y, y + row_height);
			thickline();
			show_box();

			// Show lines.
			static const int	LINES = 8;
			float	line_height = row_height / LINES;
			for (int line = 0; line < LINES; line++)
			{
				thinline();
				hrule(y + line_height * line);
			}

			// Show the day number.
			s_ps->printf(s_x1 - 11, s_y0 + 3, "%2d", day_number);
		}
	}
}


void	do_book(const month_grid year[12])
{
	// First half.

	s_ps->clear();
	do_page(year[0], true, false);
	// (blank half-page at end.)

	for (int i = 1; i < 7; i++)
	{
		s_ps->clear();
		do_page(year[i], true, false);
		do_page(year[12 - i], false, true);
	}

	// Second half (for back side of the paper).

	{for (int i = 4; i >= 0; i--)
	{
		s_ps->clear();
		do_page(year[11 - i], true, false);
		do_page(year[i + 1], false, true);
	}}

	s_ps->clear();
	// (blank half-page at end).
	do_page(year[0], false, true);

	s_ps->clear();	// back of front; i.e. outside cover
	s_ps->black();
	s_ps->font("Arial", 12);
	s_ps->printf(X0 + 3.0f * WIDTH / 4, Y0 + HEIGHT / 2, "%d", year[0].m_year);
}


int	main()
{
	tu_file	out(stdout, false);

	postscript	ps(&out, "My calendar sheets", false);
	s_ps = &ps;

	// do_book(year_2003);
	// do_book(year_2004);
	do_book(year_2005);

	return 0;
}
