/* 
 * logoinsert - insert an image into a dreamcast ip.bin
 *
 * use pngtomr to produce images to insert
 * images must be less than 8192 bytes to use in a "normal" ip.bin
 *
 * adk / napalm 2001 
 *
 * andrewk@napalm-x.com
 *
 * http://www.napalm-x.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#if __STDC_VERSION__ < 199901L
  #error "Minimum ISO C99 Required"
#elif __STDC_VERSION__ == 199901L
  #define noreturn
#elif __STDC_VERSION__ <= 201710L
  #include <stdnoreturn.h>
#elif __STDC_VERSION__ > 201710L
  #define noreturn [[noreturn]]
#endif

#include <stdio.h>
#include <stdlib.h>

static FILE* mr = (void*) 0;
static FILE* ip = (void*) 0;
static char* data = (void*) 0;

static inline void cleanup(void) {
  if (mr && (fclose(mr) == EOF)) fprintf(stderr, "Failed to close mr file.\n");
  if (ip && (fclose(ip) == EOF)) fprintf(stderr, "Failed to close ip file.\n");
  free(data);
}

noreturn static inline void error(const char* string) {
  perror(string);
  exit(EXIT_FAILURE);
}

int main(int argc, char* argv[argc + 1]) {
  if (atexit(cleanup)) error("atexit() registration failed");
  switch (argc) {
    case 3: {
      puts("logoinsert 0.1 by adk/napalm");

      if (!(mr = fopen(argv[1], "rb"))) error(argv[1]);
      if (!(ip = fopen(argv[2], "rb+"))) error(argv[2]);

      size_t size = 0;
      while (fgetc(mr) != EOF) size++;
      if (ferror(mr)) error(argv[1]);
      else if (!feof(mr)) error(argv[1]);

      if (size > 8192) puts("Warning: this image is larger than 8192 bytes and will corrupt a normal ip.bin, inserting anyway!");

      if (!fseek(ip, 0x3820, SEEK_SET)) error(argv[2]);
      if (!fseek(mr, 0, SEEK_SET)) error(argv[1]);

      if (!(data = calloc(size, sizeof(*data)))) error("calloc() failed");
      if (fread(data, sizeof(*data), size, mr) < size) error("fread() failed");
      if (fwrite(data, sizeof(*data), size, ip) < size) error("fwrite() failed");
      return EXIT_SUCCESS;
    } break;
    default: return fprintf(stderr, "%s <image.mr> <ip.bin>\n", argv[0]), EXIT_FAILURE;
  }
}
