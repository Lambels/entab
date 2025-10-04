#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(char *);

static FILE *instream;
static FILE *outstream;

int main(int argc, char *argv[]) {
  int ch, t;
  char *last;
  t = 4;
  while ((ch = getopt(argc, argv, "hf:o:t:")) != -1) {
    switch (ch) {
    case 'f':
      instream = fopen(optarg, "r");
      if (!instream) {
        perror("Error while trying to open the input file\n");
        return 1;
      }
      break;
    case 'o':
      outstream = fopen(optarg, "w");
      if (!outstream) {
        perror("Error while trying to open the output file\n");
        return 1;
      }
      break;
    case 'h':
      usage("");
      return 0;
    case 't':
      t = (int)strtoul(optarg, &last, 10);
      if (*last != '\0') {
        usage("Expected a unsigned number as a tab size\n");
        return 1;
      }
      if (errno != 0) {
        perror("");
        return 1;
      }
      break;
    default:
      usage("");
      return 1;
    }
  }

  argv += optind;
  argc -= optind;

  // if we dont have an instream set we treat the next arguments as
  // our instream.
  if (!instream) {
    // expect only one argument after parsing options and no file stream
    // provided.
    if (argc != 1) {
      usage("Expected 1 text argument\n");
      return 1;
    }

    // open a stream backed by the argument provided.
    instream = fmemopen(argv[0], strlen(argv[0]), "r");
    if (!instream) {
      perror("Error while trying to open argument buffer stream");
      return 1;
    }
  }

  // we write to stdout.
  if (!outstream) {
    outstream = fdopen(STDOUT_FILENO, "w");
    if (!outstream) {
      perror("Error while trying to open STDOUT");
      return -1;
    }
  }

  // split the stream in columns/tab stops. A tab is whitespace up to a tab
  // stop.
  int i = 0;
  int inspace = 0;
  int startspace = 0;
  while ((ch = fgetc(instream)) != EOF) {
    if (i % t == 0)
      i = 0; // we are at a tabstop.

    if (i == 0 && inspace)
      fputc('\t', outstream);

    switch (ch) {
    case '\n':
    case '\t':
      i = inspace = 0;
      fputc(ch, outstream);
      break;
    case ' ':
      if (!inspace || i == 0) {
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
}

void usage(char *ctx) {
  printf("%s", ctx);
  printf("Usage: entab [-h] [-t tab] [-o file] {-f file | text}\n");
}
