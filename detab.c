#include <stdio.h>

int detab(FILE *instream, FILE *outstream, int t) {
  int ch, i;
  ch = i = 0;
  while ((ch = fgetc(instream)) != EOF) {
    i %= t;

    if (ch != '\t') {
      fputc(ch, outstream);
      i++;
    } else {
      // we have a tab, insert spaces to move to cursor to next tabstop.
      for (int j = 0; j < t - i; j++)
        fputc(' ', outstream);
      i = 0; // whatever follows starts at a new tabstop.
    }
  }

  if (ferror(instream)) {
    fprintf(stderr, "Error while reading from instream");
    return 1;
  }

  return 0;
}
