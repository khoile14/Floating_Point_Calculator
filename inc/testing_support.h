

#ifndef TESTING_SUPPORT_H
#define TESTING_SUPPORT_H

#include "common_definitions.h" // Adds Color Information

// Prints an Informational Message with printf-style formatting arguments
#define PRINT_INFO(str, ...) do {               \
  printf("  %s[Info ]%s ", GREEN, YELLOW);     \
  printf(str, ##__VA_ARGS__);                   \
  printf("%s\n", RST);                          \
} while(0)

// Prints a Status Message with printf-style formatting arguments
#define PRINT_STATUS(str, ...) do {             \
  printf("  %s[Status]%s ", YELLOW, YELLOW);    \
  printf(str, ##__VA_ARGS__);                   \
  printf("%s\n", RST);                          \
} while(0)

// Prints a Warning Message with printf-style formatting arguments
#define PRINT_WARNING(str, ...) do {            \
  printf("  %s[Warn  ]%s ", MAGENTA, YELLOW);   \
  printf(str, ##__VA_ARGS__);                   \
  printf("%s\n", RST);                          \
} while(0)

// Prints a Message with Context, with printf-style formatting arguments
#define MARK(str, ...) do {                    \
  printf("  %s[MARK]%s ", MAGENTA, RST);       \
  printf(str, ##__VA_ARGS__);                  \
  printf("    %s{%s:%d in %s}%s\n", MAGENTA, __FILE__, __LINE__, __func__, RST); \
} while(0)

#endif
