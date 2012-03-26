#include <stdio.h> // fprintf
#include <stdlib.h> // exit

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: can2c .h lib/script.can [build/script.h]\n");
    exit(1);
  }

  // Open both files
  FILE* in = fopen(argv[1], "r");
  if (in == NULL) {
    fprintf(stderr, "Can\'t open input file: %s\n", argv[1]);
    exit(1);
  }

  FILE* out;
  if (argc >= 3) {
    out = fopen(argv[2], "w");
    if (out == NULL) {
      fclose(in);
      fprintf(stderr, "Can\'t open output file: %s\n", argv[2]);
      exit(1);
    }
  } else {
    out = stdout;
  }

  // Replace non-chars with _ in input filename
  // and '.' with '\0'
  for (int i = 0; argv[1][i] != 0; i++) {
    unsigned char c = argv[1][i];
    if (c == '.') {
      argv[1][i] = 0;
      break;
    } else if (c < 'a' || c > 'z') {
      argv[1][i] = '_';
    }
  }

  // Put generated content into the output file
  fprintf(out, "extern const char __binding_%s[] = {\n", argv[1]);

  int offset = 0;
  while (!feof(in)) {
    // Read file
    unsigned char buffer[1024];
    size_t read = fread(buffer, 1, sizeof(buffer), in);
    if (read == 0) {
      fclose(in);
      fclose(out);
      fprintf(stderr, "Failed to read input file\n");
      exit(1);
    }

    // Translate it to byte sequence
    for (size_t i = 0; i < read; i++) {
      fprintf(out, "0x%.2x, ", buffer[i]);
      if (++offset % 30 == 0) fprintf(out, "\n");
    }
  }

  fprintf(out, "0x0\n};");

  fclose(in);
  fclose(out);
}
