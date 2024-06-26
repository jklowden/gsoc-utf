#define _GNU_SOURCE  1

#include <err.h>
#include <errno.h>
#include <iconv.h>
#include <langinfo.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ANSI_X3.4-1968 */
/* CP1252 */
/* ISO-8859-1 */
/* UTF-8 */

/*
 * This program demonstrates how the locale is collected from the
 * enviroment and how iconv(3) reports errors. It convert a hard-coded
 * string, a UTF-8 Euro sign, to an output encoding taken from the
 * command line (default: ISO 8859-15).
 * 
 * The input encoding is taken from the environment and can be
 * overridden on the command line.
 *
 * The user can experiment by changing the encodings and/or the input.
 */
int main(int argc, char *argv[])
{
  const char   *tocode = "ISO-8859-15";
  char *fromcode;

  // Set the locale in the C standard library.
  char * locale = setlocale(LC_CTYPE, "");
  if( ! locale  ) {
    const char *name = getenv("LC_CTYPE");
    err(EXIT_FAILURE, "could not set locale with LC_CTYPE %s ", name? name : "(none)");
  }
  // Set the input encoding from the locale. 
  fromcode = nl_langinfo(CODESET);

  // Potentially override the input & output encodings
  // syntax: iconv_test [to-code [from-code]]
  if( 1 < argc ) {
    tocode = argv[1];
  }
  if( 2 < argc ) {
    fromcode = argv[2];
  }

  // Establish the iconv handle. 
  iconv_t cd = iconv_open(tocode, fromcode);

  if( cd == (iconv_t)-1 ) {
    err(EXIT_FAILURE, "error: iconv_open: cannot convert from %s to %s",
        fromcode, tocode);
  }
  printf("converting from %s to %s\n", fromcode, tocode);

  // input
  static char euro_utf8[64] = { 0xE2, 0x82, 0xAC }, *input = euro_utf8;;

  // output buffer
  char outbuf[64] = {}, *output = outbuf;
  size_t ilen = strlen(euro_utf8), olen = sizeof(output);

  // convert
  size_t erc = iconv(cd, &input, &ilen, &output, &olen);

  if( erc == -1 ) {
    warn("error: iconv: %s: cannot convert input to output @ pos %zu",
         strerrorname_np(errno), input - euro_utf8);
  } else {
    if( erc > 0 ) {
      printf( "iconv: %zu nonreversible conversions", erc );
    }
  }

  // Write the output as a hexadecimal sequence.
  
  // "output" was updated by iconv to point to the end of the data
  // written to the "outbuf" buffer.  We stop either when the buffer
  // is exhausted, or when the current character is the last one.
  size_t i;
  for( i=0; i < sizeof(outbuf); i++ ) {
    if( outbuf + i == output ) break;
    printf( "0x%02X ", outbuf[i] );
  }
  if( i > 0 ) puts("");

  return EXIT_SUCCESS;
}
