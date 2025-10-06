#include "dentab.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(char *);

static FILE *instream;
static FILE *outstream;

int main(int argc, char *argv[]) {
  int ch, t, ed;
  char *last;
  t = 4;
  ed = -1;
  while ((ch = getopt(argc, argv, "edhf:o:t:")) != -1) {
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
    case 'e':
    case 'd':
      if (ed != -1) {
        usage("Can only use entab xor detab mode\n");
        return -1;
      }

      ed = (ch == 'e') ? 1 : 0;
      break;
    default:
      usage("");
      return 1;
    }
  }

  if (ed == -1) {
    usage("Must specify entab xor detab mode\n");
    return 1;
  }

  argv += optind;
  argc -= optind;

  // if we dont have an instream set we treat the next arguments as
  // our instream.
  if (!instream && argc > 1) {
    // expect only one argument after parsing options and no file stream
    // provided.
    usage("Expected only 1 text argument\n");
    return 1;

  } else if (!instream && argc == 1) {
      // open a stream backed by the argument provided.
      instream = fmemopen(argv[0], strlen(argv[0]), "r");
      if (!instream) {
          perror("Error while trying to open argument buffer stream");
          return 1;
      }
  } else if (!instream && argc == 0) {
        instream = fdopen(STDIN_FILENO, "r");
  }


  // we write to stdout.
  if (!outstream) {
    outstream = fdopen(STDOUT_FILENO, "w");
    if (!outstream) {
      perror("Error while trying to open STDOUT");
      return -1;
    }
  }

  return (ed == 1) ? entab(instream, outstream, t)
                   : detab(instream, outstream, t);
}

void usage(char *ctx) {
  printf("%s", ctx);
  printf("Usage: dentab {-e|-d} [-h] [-t tab] [-o file] [-f file | text]\n");
}
