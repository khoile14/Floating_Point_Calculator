
#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include "tinysf.h"
#include "common_definitions.h"

#define IS_INTERACTIVE isatty(0)

#define ECHOTTY(str, ...) do {     \
  !IS_INTERACTIVE?printf("%s", YELLOW):1;       \
  !IS_INTERACTIVE?printf(str, ##__VA_ARGS__):1;   \
  !IS_INTERACTIVE?printf("%s", RST):1;          \
} while(0)

#define ECHONOTTY(str, ...) do {     \
  IS_INTERACTIVE?printf(str, ##__VA_ARGS__):1;   \
} while(0)


void print_prompt(char *msg);
void print_num(char *var, struct number_struct *num, int precision, long result);
void print_sym_not_found(char *str);
void disable_buffering(FILE *stream);
void enable_buffering(FILE *stream);
unsigned int fraction_to_binary(long fraction_part, int precision);
long binary_to_fraction(unsigned int binary, int *precision);
void print_binary_value(const char *msg, unsigned int whole, unsigned int fraction);
void print_value(const char *msg, tinysf_s value);
void print_help();
void print_ni(char *msg);
void print_binary_whole(unsigned int val);
void print_binary_fraction(unsigned int val);

#endif
