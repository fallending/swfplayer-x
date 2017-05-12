// Excerpted from Sean's Tool Box, and hacked up a little by tulrich.
// All modifications are Public Domain as well.  Thanks Sean!

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "sha1.h"

#ifdef _MSC_VER
typedef unsigned _int64 stb__64;
#else
#include <stdint.h>
typedef uint64_t stb__64;
#endif

typedef unsigned int stb_uint;
typedef unsigned char stb_uchar;

#define stb__fopen(x,y) fopen(x,y)


/* stb-2.17 - Sean's Tool Box -- public domain -- http://nothings.org/stb.h
          no warranty is offered or implied; use this code at your own risk

   This is a single header file with a bunch of useful utilities
   for getting stuff done in C/C++.

   Email bug reports, feature requests, etc. to 'sean' at the same site.


   Documentation: http://nothings.org/stb/stb_h.html
   Unit tests:    http://nothings.org/stb/stb.c
*/

#define stb_big32(c)    (((c)[0]<<24) + (c)[1]*65536 + (c)[2]*256 + (c)[3])

static void stb__sha1(const stb_uchar *chunk, stb_uint h[5])
{
   int i;
   stb_uint a,b,c,d,e;
   stb_uint w[80];

   for (i=0; i < 16; ++i)
      w[i] = stb_big32(&chunk[i*4]);
   for (i=16; i < 80; ++i) {
      stb_uint t;
      t = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
      w[i] = (t + t) | (t >> 31);
   }

   a = h[0];
   b = h[1];
   c = h[2];
   d = h[3];
   e = h[4];

   #define STB__SHA1(k,f)                                            \
   {                                                                 \
      stb_uint temp = (a << 5) + (a >> 27) + (f) + e + (k) + w[i];  \
      e = d;                                                       \
      d = c;                                                     \
      c = (b << 30) + (b >> 2);                               \
      b = a;                                              \
      a = temp;                                    \
   }

   i=0;
   for (; i < 20; ++i) STB__SHA1(0x5a827999, d ^ (b & (c ^ d))       );
   for (; i < 40; ++i) STB__SHA1(0x6ed9eba1, b ^ c ^ d               );
   for (; i < 60; ++i) STB__SHA1(0x8f1bbcdc, (b & c) + (d & (b ^ c)) );
   for (; i < 80; ++i) STB__SHA1(0xca62c1d6, b ^ c ^ d               );

   #undef STB__SHA1

   h[0] += a;
   h[1] += b;
   h[2] += c;
   h[3] += d;
   h[4] += e;
}

void stb_sha1(stb_uchar input_output[20], const stb_uchar *buffer, stb_uint len)
{
   unsigned char final_block[128];
   stb_uint end_start, final_len, j;
   int i;

   stb_uint h[5];
   const stb_uint* uint_io = (stb_uint*) input_output;
   h[0] = uint_io[0];
   h[1] = uint_io[1];
   h[2] = uint_io[2];
   h[3] = uint_io[3];
   h[4] = uint_io[4];

   // we need to write padding to the last one or two
   // blocks, so build those first into 'final_block'

   // we have to write one special byte, plus the 8-byte length

   // compute the block where the data runs out
   end_start = len & ~63;

   // compute the earliest we can encode the length
   if (((len+9) & ~63) == end_start) {
      // it all fits in one block, so fill a second-to-last block
      end_start -= 64;
   }

   final_len = end_start + 128;

   // now we need to copy the data in
   assert(end_start + 128 >= len+9);
   assert(end_start < len || len < 64-9);

   j = 0;
   if (end_start > len)
      j = (stb_uint) - (int) end_start;

   for (; end_start + j < len; ++j)
      final_block[j] = buffer[end_start + j];
   final_block[j++] = 0x80;
   while (j < 128-5) // 5 byte length, so write 4 extra padding bytes
      final_block[j++] = 0;
   // big-endian size
   final_block[j++] = len >> 29;
   final_block[j++] = len >> 21;
   final_block[j++] = len >> 13;
   final_block[j++] = len >>  5;
   final_block[j++] = len <<  3;
   assert(j == 128 && end_start + j == final_len);

   for (j=0; j < final_len; j += 64) { // 512-bit chunks
      if (j+64 >= end_start+64)
         stb__sha1(&final_block[j - end_start], h);
      else
         stb__sha1(&buffer[j], h);
   }

   for (i=0; i < 5; ++i) {
      input_output[i*4 + 0] = h[i] >> 24;
      input_output[i*4 + 1] = h[i] >> 16;
      input_output[i*4 + 2] = h[i] >>  8;
      input_output[i*4 + 3] = h[i] >>  0;
   }
}

void stb_sha1_init(stb_uchar output[20]) {
   stb_uint* uint_io = (stb_uint*) output;
   uint_io[0] = 0x67452301;
   uint_io[1] = 0xefcdab89;
   uint_io[2] = 0x98badcfe;
   uint_io[3] = 0x10325476;
   uint_io[4] = 0xc3d2e1f0;
}

int stb_sha1_file(stb_uchar input_output[20], const char *file)
{
   int i;
   stb__64 length=0;
   unsigned char buffer[128];

   FILE *f = stb__fopen(file, "rb");
   stb_uint h[5];

   if (f == NULL) return 0; // file not found

   const stb_uint* uint_io = (const stb_uint*) input_output;
   h[0] = uint_io[0];
   h[1] = uint_io[1];
   h[2] = uint_io[2];
   h[3] = uint_io[3];
   h[4] = uint_io[4];

   for(;;) {
      int n = fread(buffer, 1, 64, f);
      if (n == 64) {
         stb__sha1(buffer, h);
         length += n;
      } else {
         int block = 64;

         length += n;

         buffer[n++] = 0x80;

         // if there isn't enough room for the length, double the block
         if (n + 8 > 64)
            block = 128;

         // pad to end
         memset(buffer+n, 0, block-8-n);

         i = block - 8;
         buffer[i++] = (stb_uchar) (length >> 53);
         buffer[i++] = (stb_uchar) (length >> 45);
         buffer[i++] = (stb_uchar) (length >> 37);
         buffer[i++] = (stb_uchar) (length >> 29);
         buffer[i++] = (stb_uchar) (length >> 21);
         buffer[i++] = (stb_uchar) (length >> 13);
         buffer[i++] = (stb_uchar) (length >>  5);
         buffer[i++] = (stb_uchar) (length <<  3);
         assert(i == block);
         stb__sha1(buffer, h);
         if (block == 128)
            stb__sha1(buffer+64, h);
         else
            assert(block == 64);
         break;
      }
   }
   fclose(f);

   for (i=0; i < 5; ++i) {
      input_output[i*4 + 0] = h[i] >> 24;
      input_output[i*4 + 1] = h[i] >> 16;
      input_output[i*4 + 2] = h[i] >>  8;
      input_output[i*4 + 3] = h[i] >>  0;
   }

   return 1;
}

char stb_sha1_readable_encoding[65] =
	"0123456789abcdefghijklmnopqrstuv"
	"wxyzABCDEFGHIJKLMNOPQRSTUVWXYZ%$";

// client can truncate this wherever they like
void stb_sha1_readable(char display[27], const unsigned char sha[20])
{
   int num_bits = 0, acc=0;
   int i=0,o=0;
   while (o < 26) {
      int v;
      // expand the accumulator
      if (num_bits < 6) {
         assert(i != 20);
         acc += sha[i++] << num_bits;
         num_bits += 8;
      }
      v = acc & ((1 << 6) - 1);
      display[o++] = stb_sha1_readable_encoding[v];
      acc >>= 6;
      num_bits -= 6;
   }
   assert(num_bits == 20*8 - 26*6);
   display[o++] = stb_sha1_readable_encoding[acc];
}

// Returns true if display[] was a valid readable sha1 value.  If
// display[] is not valid, returns false and fills sha[] with 0.
bool stb_sha1_from_readable(const char display[27], unsigned char sha[20])
{
   static unsigned char decode_table[256] = { 0 };
   if (decode_table[0] == 0) {
      memset(decode_table, 0xFF, sizeof(decode_table));
      int i = 0;
      while (char c = stb_sha1_readable_encoding[i]) {
         decode_table[(unsigned char) c] = i;
	 i++;
      }
   }

   int num_bits = 0, acc = 0;
   int i = 0, o = 0;
   while (o < 20) {
      // expand the accumulator
      while (num_bits < 8) {
         assert(i <= 26);
	 unsigned char val = decode_table[(unsigned char) display[i++]];
	 if (val & 0xC0) {
	    memset(sha, 0, sizeof(sha));
	    return false;
	 }
         acc += val << num_bits;
         num_bits += 6;
      }
      sha[o++] = acc & 0xFF;
      acc >>= 8;
      num_bits -= 8;
   }
   if (acc) {
      // Bits left over! Display[] is invalid.
      memset(sha, 0, sizeof(sha));
      return false;
   }

   return true;
}
