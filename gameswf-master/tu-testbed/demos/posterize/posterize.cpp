// posterize.cpp -- Thatcher Ulrich <http://tulrich.com> 2007

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Make a hi-res posterized version of an input image.

#include <assert.h>
#include "base/image.h"
#include "base/png_helper.h"
#include "base/utility.h"


const int OVERSAMPLE = 4;

struct image_level {
	float threshold;  // apply this color to lightness above this level
	int r, g, b;
} LEVELS[] = {
	{ 0.15f, 124, 115, 66 },
	{ 0.35f, 191, 181, 123 },
	{ 0.60f, 229, 218, 159 },
	{ 0.83f, 255, 255, 255 },
};


// Return the level in [0,1] for a supersampled pixel.
float sample_level(const image::rgb* in, int x, int y, int oversample, int s, int t) {
	assert(x >= 0 && x < in->m_width);
	assert(y >= 0 && y < in->m_height);

	// TODO: larger kernel for smoother edges
	const uint8* p00 = in->m_data + (y * in->m_width + x) * 3;
	const uint8* p10 = p00 + (in->m_width * 3);
	float level00 = float(p00[0] + p00[1] + p00[2]);
	float level01 = float(p00[3] + p00[4] + p00[5]);
	float level10 = float(p10[0] + p10[1] + p10[2]);
	float level11 = float(p10[3] + p10[4] + p10[5]);

	int s0 = (oversample - 1 - s);
	int t0 = (oversample - 1 - t);

	float level = (level00 * s0 + level01 * s) * t0
		+ (level10 * s0 + level11 * s) * t;
	level /= ((oversample - 1) * (oversample - 1) * 255.0f * 3);

	return fclamp(level, 0, 1);
}

// Take the input file, apply the posterize operation according to
// parameters above, and write an output image to the output_file.
void posterize(const char* input_file, const char* output_file) {
	image::rgb* in = image::read_jpeg(input_file);
	if (!in) {
		fprintf(stderr, "couldn't read %s\n", input_file);
		exit(1);
	}

	int w_out = (in->m_width - 1) * OVERSAMPLE;
	int h_out = (in->m_height - 1) * OVERSAMPLE;
	image::rgba* out = new image::rgba(w_out, h_out);

	for (int y = 0; y < h_out; y++) {
		int t = y % OVERSAMPLE;
		for (int x = 0; x < w_out; x++) {
			int s = x % OVERSAMPLE;
			float level = sample_level(in, x / OVERSAMPLE, y / OVERSAMPLE, OVERSAMPLE, s, t);
			int r = 0;
			int g = 0;
			int b = 0;
			int a = 0;
			for (int i = TU_ARRAYSIZE(LEVELS) - 1; i >= 0; i--) {
				if (level > LEVELS[i].threshold) {
					r = LEVELS[i].r;
					g = LEVELS[i].g;
					b = LEVELS[i].b;
					a = 255;
					break;
				}
			}
			uint8* p = out->m_data + (y * w_out + x) * 4;
 			p[0] = r;
 			p[1] = g;
 			p[2] = b;
 			p[3] = a;
		}
	}

	// Write the output.
	FILE* fp = fopen(output_file, "wb");
	if (!fp) {
		fprintf(stderr, "can't open %s for output\n", output_file);
		delete in;
		delete out;
		exit(1);
	}
	png_helper::write_rgba(fp, out->m_data, w_out, h_out);

	delete in;
	delete out;
}

int main(int argc, const char* argv[]) {
	const char* input_file = NULL;
	const char* output_file = NULL;
	for (int a = 1; a < argc; a++) {
		if (argv[a][0] == '-') {
			switch (argv[a][1]) {
			case 'h':
			default:
				fprintf(stderr, "unknown switch %s\n", argv[a]);
				exit(1);
				break;
			}
		} else if (input_file == NULL) {
			input_file = argv[a];
		} else if (output_file == NULL) {
			output_file = argv[a];
		}
	}

	posterize(input_file, output_file);

	return 0;
}

