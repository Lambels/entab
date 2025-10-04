#include <stdio.h>

int entab(FILE *instream, FILE *outstream, int t) {
  // split the stream in columns/tab stops. A tab is whitespace up to a tab
  // stop.
  int i, ch, inspace, startspace;
  i = ch = inspace = startspace = 0;
  while ((ch = fgetc(instream)) != EOF) {
    if (i % t == 0)
      i = 0; // we are at a tabstop.

    if (i == 0 && inspace) {
      inspace = 0;            // prevent inspace state from previous tab stop.
      fputc('\t', outstream); // if we are at a tab stop and we were previously
                              // inspace we insert a tab char.
    }

    switch (ch) {
    case '\n':
    case '\t':
      i = inspace = 0; // if we encounter a newline or tab character we will
                       // automatically move to a new tabstop.
      fputc(ch, outstream);
      break;
    case ' ':
      if (!inspace) {
        inspace = 1;
        startspace = i;
      }
      break;
    default:
      if (inspace) {
        inspace = 0;
        for (int j = 0; j < (i - startspace); j++)
          fputc(' ', outstream);
      }
      fputc(ch, outstream);
      break;
    }

    i++;
  }

  // flush any trailing spaces.
  if (inspace)
    for (int j = 0; j < (i - startspace); j++)
      fputc(' ', outstream);

  if (ferror(instream)) {
    fprintf(stderr, "An error occurend on the input stream\n");
    return 1;
  }

  return 0;
}
