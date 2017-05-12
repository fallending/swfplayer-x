// font_output.cpp	-- Thatcher Ulrich <http://tulrich.com> 2004

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Program to generate some sample output of a TrueType/OpenType font,
// using FreeType2.
//
// Helpful for automatically generating bitmaps showing what a
// particular font looks like, finding hinting errors, etc.


#include "base/container.h"
#include "base/image.h"
#include "base/png_helper.h"
#include <assert.h>
#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


static const int LITTLE_WIDTH = 500;
static const int LITTLE_HEIGHT = 60;

static const int BIG_WIDTH = 600;
static const int BIG_HEIGHT = 600;

static const int CHARMAP_CHARS_PER_ROW = 16;
static const int CHARMAP_PIXELS_PER_CHAR_X = 48;
static const int CHARMAP_PIXELS_PER_CHAR_Y = 64;
static const int CHARMAP_WIDTH = CHARMAP_CHARS_PER_ROW * CHARMAP_PIXELS_PER_CHAR_X;
static const int CHARMAP_HEIGHT = CHARMAP_PIXELS_PER_CHAR_Y * 16;

static image::alpha*	s_image;

static int	s_y = 0;


void	my_draw_x(int x, int y)
// For debugging.
{
	if (x >= 1 && x < s_image->m_width - 1
	    && y >= 1 && y < s_image->m_height - 1)
	{
		s_image->set_pixel(x    , s_image->m_height - 1 - y, 0);
		s_image->set_pixel(x - 1, s_image->m_height - 2 - y, 0);
		s_image->set_pixel(x + 1, s_image->m_height - 2 - y, 0);
		s_image->set_pixel(x - 1, s_image->m_height - 0 - y, 0);
		s_image->set_pixel(x + 1, s_image->m_height - 0 - y, 0);
	}
}


void	my_draw_bitmap(FT_Bitmap* bitmap, int left_x, int top_y )
{
	// Horizontal clipping.
	int	x0 = left_x;
	int	sx0 = 0;
	int	sw = bitmap->width;
	if (x0 >= s_image->m_width)
	{
		return;
	}
	else if (x0 < 0)
	{
		// Clip left.
		sx0 = -x0;
		x0 = 0;
		sw -= sx0;
	}
	if (x0 + sw <= 0)
	{
		return;
	}
	else if (x0 + sw > s_image->m_width)
	{
		// Clip right.
		sw -= (x0 + sw - s_image->m_width);
	}

	for (int i = 0; i < bitmap->rows; i++)
	{
		// Vertical clipping.
		int	y = s_image->m_height - 1 - (top_y) + i;
		if (y < 0)
		{
			continue;
		}
		else if (y >= s_image->m_height)
		{
			break;
		}

		uint8*	src = bitmap->buffer + (bitmap->pitch * (i)) + sx0;
		uint8*	dst = s_image->m_data + y * s_image->m_width + x0;

		int	x = sw;
		while (x-- > 0)
		{
			uint8	d = *dst;
			uint8	s = 255 - *src++;
			if (s < d)
			{
				*dst = s;
			}
			dst++;
		}
	}
}


void compute_string_bbox( FT_BBox *abbox, const array<FT_Glyph>& glyphs, const array<FT_Vector>& pos)
{
	FT_BBox bbox;

	/* initialize string bbox to "empty" values */
	bbox.xMin = bbox.yMin = 32000;
	bbox.xMax = bbox.yMax = -32000;

	/* for each glyph image, compute its bounding box, */
	/* translate it, and grow the string bbox */
	for (int n = 0; n < glyphs.size(); n++)
	{
		FT_BBox glyph_bbox;
		FT_Glyph_Get_CBox( glyphs[n], ft_glyph_bbox_pixels, &glyph_bbox );
		glyph_bbox.xMin += pos[n].x;
		glyph_bbox.xMax += pos[n].x;
		glyph_bbox.yMin += pos[n].y;
		glyph_bbox.yMax += pos[n].y;
		if ( glyph_bbox.xMin < bbox.xMin )
			bbox.xMin = glyph_bbox.xMin;
		if ( glyph_bbox.yMin < bbox.yMin )
			bbox.yMin = glyph_bbox.yMin;
		if ( glyph_bbox.xMax > bbox.xMax )
			bbox.xMax = glyph_bbox.xMax;
		if ( glyph_bbox.yMax > bbox.yMax )
			bbox.yMax = glyph_bbox.yMax;
	}

	/* check that we really grew the string bbox */
	if ( bbox.xMin > bbox.xMax )
	{
		bbox.xMin = 0;
		bbox.yMin = 0;
		bbox.xMax = 0;
		bbox.yMax = 0;
	}

	/* return string bbox */
	*abbox = bbox;
}

void	draw_centered_text(FT_Face& face, const char* text)
{
	// Hacked up from the FreeType tutorial.

	FT_UInt glyph_index;
	FT_Bool use_kerning;
	FT_UInt previous;

	int pen_x, pen_y, n;
	array<FT_Glyph> glyphs; 	/* glyph image */
	glyphs.reserve(strlen(text));
	array<FT_Vector> pos;	/* glyph position */

	pen_x = 0;	/* start at (0,0) */
	pen_y = 0;
	use_kerning = static_cast<FT_Bool>(FT_HAS_KERNING( face ));
	previous = 0;
	int	c;
	while ((c = *text++))
	{
		/* convert character code to glyph index */
		glyph_index = FT_Get_Char_Index( face, c );

		/* retrieve kerning distance and move pen position */
		if ( use_kerning && previous && glyph_index )
		{
			FT_Vector delta;
			FT_Get_Kerning( face, previous, glyph_index, FT_KERNING_DEFAULT, &delta );
			pen_x += delta.x;
			pen_y += delta.y;
		}

		/* store current pen position in rounded pixel coords */
		pos.resize(pos.size() + 1);
		pos.back().x = (pen_x + 32) >> 6;
		pos.back().y = (pen_y + 32) >> 6;

		/* load glyph image into the slot without rendering */
		int	error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );

		if ( error ) continue;	/* ignore errors, jump to next glyph */

		/* extract glyph image and store it in our table */
		glyphs.resize(glyphs.size() + 1);
		error = FT_Get_Glyph( face->glyph, &glyphs.back() );

		if ( error ) continue;	/* ignore errors, jump to next glyph */

		/* increment pen position */
		pen_x += face->glyph->advance.x;
		pen_y += face->glyph->advance.y;

		/* record current glyph index */
		previous = glyph_index;
	}

	FT_BBox	string_bbox;
	compute_string_bbox(&string_bbox, glyphs, pos);

	/* compute string dimensions in integer pixels */
	int	string_width = string_bbox.xMax - string_bbox.xMin;
	int	string_height = string_bbox.yMax - string_bbox.yMin;

	s_y += string_height + 5;

	/* compute start pen position in 26.6 cartesian pixels */
	int	start_x = ( ( s_image->m_width - string_width ) / 2 ) * 64;	// centered
	int	start_y = (s_image->m_height - 1 - s_y) * 64;
	for ( n = 0; n < glyphs.size(); n++ )
	{
		FT_Glyph image;
		FT_Vector pen;
		image = glyphs[n];
		pen.x = start_x + pos[n].x * 64;
		pen.y = start_y + pos[n].y * 64;

		int	error = FT_Glyph_To_Bitmap( &image, FT_RENDER_MODE_NORMAL, &pen, 0 );
		if ( !error )
		{
			FT_BitmapGlyph bit = (FT_BitmapGlyph)image;
			my_draw_bitmap(&bit->bitmap, bit->left, bit->top);
		}
	}

	for (n = 0; n < glyphs.size(); n++)
	{
		FT_Done_Glyph( glyphs[n] );
	}
}


void draw_character_map(FT_Face& face)
// Draw all the characters in a big grid.
{
	int pen_x, pen_y;
	FT_Glyph glyph; 	/* glyph image */

	pen_x = 0;	/* start at (0,0) */
	pen_y = CHARMAP_HEIGHT - CHARMAP_PIXELS_PER_CHAR_Y;

	// Iterate through all glyphs in the font.
	FT_ULong  charcode;
	FT_UInt glyph_index;
	charcode = FT_Get_First_Char(face, &glyph_index);
	if (glyph_index == 0) {
		// No glyphs!
		return;
	}
	for (;;) {
		// Draw this character.
		pen_x = (charcode % CHARMAP_CHARS_PER_ROW) * CHARMAP_PIXELS_PER_CHAR_X;
		int error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (!error) {
			// extract glyph image
			error = FT_Get_Glyph(face->glyph, &glyph);
			if (!error) {
				FT_Vector pen;
				pen.x = pen_x * 64;
				pen.y = pen_y * 64;

				// Center the glyph horizontally.
				FT_BBox glyph_bbox;
				FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &glyph_bbox);
				int width = glyph_bbox.xMax - glyph_bbox.xMin;
				pen.x -= glyph_bbox.xMin * 64;
				pen.x += (CHARMAP_PIXELS_PER_CHAR_X - width) / 2 * 64;

				error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, &pen, 0);
				if (!error) {
					// Draw it.
					FT_BitmapGlyph bit = (FT_BitmapGlyph) glyph;
					my_draw_bitmap(&bit->bitmap, bit->left, bit->top);
				}
				FT_Done_Glyph(glyph);
			}
		}

		FT_ULong next_charcode = FT_Get_Next_Char(face, charcode, &glyph_index);
		if (glyph_index == 0) {
			// Done.
			break;
		}

		if ((next_charcode / CHARMAP_CHARS_PER_ROW) != (charcode / CHARMAP_CHARS_PER_ROW)) {
			// Drop down to the next row.
			pen_y -= CHARMAP_PIXELS_PER_CHAR_Y;
		}
		charcode = next_charcode;
	}
}


void	output_image(const char* filename)
// Output a grayscale png file.
{
	FILE*	fp = fopen(filename, "wb");
	if (fp)
	{
		// PPM -- very simple format
// 		fprintf(fp, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
// 		for (int j = 0; j < HEIGHT; j++)
// 		{
// 			for (int i = 0; i < WIDTH; i++)
// 			{
// 				fputc(s_bitmap[j][i], fp);
// 				fputc(s_bitmap[j][i], fp);
// 				fputc(s_bitmap[j][i], fp);
// 			}
// 		}

		// PNG
		png_helper::write_grayscale(fp, s_image->m_data, s_image->m_width, s_image->m_height);

 		fclose(fp);
	}
}


void	print_usage()
{
	printf(
		"font_output -- A program to render text in a given font.\n"
		"Intended for making images of sample text.\n"
		"This program uses FreeType, see http://freetype.org\n"
		"\n"
		"usage:\n"
		"\tfont_output [font_filename] [output_basename]\n"
		);
}


int	main(int argc, const char** argv)
{
	FT_Library	lib;

	int	error = FT_Init_FreeType(&lib);
	if (error)
	{
		fprintf(stderr, "can't init FreeType!  error = %d\n", error);
		exit(1);
	}

	const char*	font_filename = NULL;
	const char*	output_basename = NULL;

	for (int arg = 1; arg < argc; arg++)
	{
		if (argv[arg][0] == '-')
		{
			// Option.
			if (argv[arg][1] == 'h' || argv[arg][1] == '?')
			{
				print_usage();
				exit(0);
			}
			else
			{
				fprintf(stderr, "unknown option '%s', use -h for usage\n", argv[arg]);
				exit(1);
			}
		}
		else if (font_filename == NULL)
		{
			font_filename = argv[arg];
		}
		else if (output_basename == NULL)
		{
			output_basename = argv[arg];
		}
	}

	if (font_filename == NULL)
	{
		// Default
		font_filename = "Tuffy_Bold.otf";
	}

	if (output_basename == NULL)
	{
		// Default
		output_basename = "font_sample";
	}

	FT_Face	face;
	error = FT_New_Face(lib, font_filename, 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		fprintf(stderr, "file '%s' has bad format\n", font_filename);
		exit(1);
	}
	else if (error)
	{
		fprintf(stderr, "some error opening font '%s'\n", font_filename);
		exit(1);
	}

	error = FT_Set_Char_Size(
		face,
		0 /*width*/,
		12 * 64 /*height*64 in points!*/,
		300 /*horiz device dpi*/,
		300 /*vert device dpi*/);
	assert(error == 0);

	const char*	fontname = FT_Get_Postscript_Name(face);
	
	// Draw stuff.

	// The small image, containing just the font name.
	{
		s_image = image::create_alpha(LITTLE_WIDTH, LITTLE_HEIGHT);

		// Clear.
		memset(s_image->m_data, 255, s_image->m_width * s_image->m_height);

		s_y = -10;
		if (fontname)
		{
			draw_centered_text(face, fontname);
		}
		else
		{
			draw_centered_text(face, font_filename);
		}

		tu_string	ofile0(output_basename);
		ofile0 += "_thumb.png";
		output_image(ofile0.c_str());

		delete s_image;
	}


	// The big image, with sample text in two sizes.
	{
		s_image = image::create_alpha(BIG_WIDTH, BIG_HEIGHT);

		// Clear.
		memset(s_image->m_data, 255, s_image->m_width * s_image->m_height);

		// Large text.
		s_y = 0;
		if (fontname)
		{
			draw_centered_text(face, fontname);
		}
		else
		{
			draw_centered_text(face, font_filename);
		}
		s_y += 10;
		draw_centered_text(face, "ABCDEFGHIJKLM");
		draw_centered_text(face, "NOPQRSTUVWXYZ");
		draw_centered_text(face, "abcdefghijklmnopqr");
		draw_centered_text(face, "stuvwxyz 0123456789");
		draw_centered_text(face, "!@#$%^&*()_+-=`~{}|");
		draw_centered_text(face, "[]\\- ,./<>?;':\"");
		draw_centered_text(face, "The quick brown fox");
		draw_centered_text(face, "jumps over the lazy dog.");

		// Small text.
		error = FT_Set_Char_Size(
			face,
			0 /*width*/,
			12*64 /*height*64 in points!*/,
			96 /*horiz device dpi*/,
			96 /*vert device dpi*/);
		assert(error == 0);
		draw_centered_text(face, "12pt @ 96 dpi:");
		draw_centered_text(face, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
		draw_centered_text(face, "!@#$%^&*()_+-=`~{}|[]\\- ,./<>?;':\"");
		draw_centered_text(face, "Satan oscillate my metallic sonatas!  The quick brown fox jumps over the lazy dog.");
		draw_centered_text(face, "Go hang a salami, I'm a lasagna hog.");

		tu_string	ofile1(output_basename);
		ofile1 += ".png";
		output_image(ofile1.c_str());

		delete s_image;
	}

	//
	// A character map, showing everything in the font.
	//
	{
		s_image = image::create_alpha(CHARMAP_WIDTH, CHARMAP_HEIGHT);
		memset(s_image->m_data, 255, s_image->m_width * s_image->m_height);

		error = FT_Set_Char_Size(
			face,
			0 /*width*/,
			12*64 /*height*64 in points!*/,
			300 /*horiz device dpi*/,
			300 /*vert device dpi*/);
		assert(error == 0);
		draw_character_map(face);

		tu_string ofile2(output_basename);
		ofile2 += "_charmap.png";
		output_image(ofile2.c_str());

		delete s_image;
	}

	FT_Done_FreeType(lib);
}

