// sha1.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// (This is ripped off from Sean Barrett's stb.h; see sha1.cpp.)

#ifndef SHA1_H_
#define SHA1_H_

// Fill output[] with a fixed initial pattern.
void stb_sha1_init(unsigned char output[20]);  // added by tulrich

// tulrich: I changed the semantics of these functions; they now read
// input_output[] before modifying it.  This allows accumulation over
// multiple calls involving multiple data sources.
int stb_sha1_file(unsigned char input_output[20], const char* filename);
void stb_sha1(unsigned char input_output[20],
	      const unsigned char* buffer, unsigned int len);

void stb_sha1_readable(char display[27], const unsigned char sha[20]);

// added by tulrich
//
// Returns true if display[] was a valid readable sha1 value.  If
// display[] is not valid, returns false and fills sha[] with 0.
bool stb_sha1_from_readable(const char display[27], unsigned char sha[20]);

#endif  // SHA1_H_
