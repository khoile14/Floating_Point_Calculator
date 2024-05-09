#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "tinysf.h"
#include "muan_settings.h"
#include "common_functions.h"
#include "common_definitions.h"

/* Local Helper Functions */
static void print_binary_digits(unsigned int val);
static void print_binary_digits_precision(unsigned int val, int precision);

/* Converts the Fractional Part of a Number to Binary (eg. 4.245, 4 is the whole, 245 is fractional) */
unsigned int fraction_to_binary(long fraction_part, int precision) {
  unsigned int result = 0;
  long modval = 1;
  long update = 0;
  int i = 0;

  /* Get the Mod Value based on the Input Number Precision
   * - If 1.5 was input, then the fraction_part would be 5 with precision 1
   * - If 1.05 was input, then the fraction_part would be 5 with precision 2
   * - If 1.50 was input, then the fraction_part would be 50 with precision 2
   */
  for(i = 0; i < precision; i++) {
    long modcheck = modval * 10;
    if(modval != (modcheck / 10)) {
      printf("Error: Precision of %d is not possible!\n", precision);
      exit(1);
    }
    modval *= 10;
  }

  /* Compute the binary value of the fraction_part
   * - Multiply by 2, then look at the precision overflows.
   * - The precision overflow is the next bit in the sequence.
   * - Set that bit, then remove the overflow from the fraction_part and repeat.
   * - Repeats until the resulting binary value is 32-bit
   */
  for(i = 0; i < 32; i++) {
    result <<= 1;
    fraction_part <<= 1;
    update = fraction_part%modval;
    result |= !!(fraction_part-update);
    fraction_part = update;
  }

  return result;
} 

/* Converts the Fractional Part of a Number to Binary (eg. 4.245, 4 is the whole, 245 is fractional) */
long binary_to_fraction(unsigned int binary, int *precision) {
  long i = 0;

  /* Compute the binary value of the fraction_part
   * - Multiply by 2, then look at the precision overflows.
   * - The precision overflow is the next bit in the sequence.
   * - Set that bit, then remove the overflow from the fraction_part and repeat.
   * - Repeats until the resulting binary value is 32-bit
   */
  int temp = binary;
  *precision = 0;
  long multiplier = 0;
  while(temp) {
    temp <<= 1;
    if(multiplier == 0) {
      multiplier = 1;
    }
    else {
      multiplier *= 10;
    }
    (*precision)++;
  }

  long result = 0;
  for(i = 5; binary; i*= 5, multiplier /= 10) {
    if(binary & (1<<31)) {
      result += (i * multiplier);
    }
    binary <<= 1;
  }

  return result;
} 

/* Print the Value for MUAN */
void print_value(const char *msg, tinysf_s value) {
  printf("\r%s%s: %s", GREEN, msg, CYAN);
  print_binary_digits_precision(value, TINYSF_TOTAL_BITS);
  printf(" (0x%03x)%s%c", value & ((1 << TINYSF_TOTAL_BITS)-1), RST, IS_INTERACTIVE?'\n':'\0');
}

/* Prints the value in Binary for MUAN */
void print_binary_value(const char *msg, unsigned int whole, unsigned int fraction) {
  printf("\r%s: ", msg);
  print_binary_whole(whole);
  printf(".");
  print_binary_fraction(fraction);
  printf("\n");
}

/* Prints out the Whole Number (or 0 if none) */
void print_binary_whole(unsigned int val) {
  if(val) {
    print_binary_digits(val);
  }
  else {
    printf("0");
  }
}

/* Prints out the Fraction in Binary */
void print_binary_fraction(unsigned int val) {
  if(val) {
    while(val) {
      printf("%d", (val >> 31)&1);
      val <<= 1;
    }
  }
  else {
    printf("0");
  }
}

/* Prints out the digits of any value in Binary (up to a given precision) */
static void print_binary_digits_precision(unsigned int val, int precision) {
  if(precision == 0) {
    return;
  }
  else {
    print_binary_digits_precision((val>>1)&(~(1<<31)), precision - 1);
    printf("%d", val&0x1);
    if(precision == TINYSF_SIGN_BITS) printf(" ");
    else if(precision == TINYSF_SIGN_BITS + TINYSF_EXP_BITS) printf(" ");
  }
} 

/* Prints out the digits of any value in Binary */
static void print_binary_digits(unsigned int val) {
  if(val == 0) {
    return;
  }
  else {
    print_binary_digits((val>>1)&(~(1<<31)));
    printf("%d", val&0x1);
  }
} 

/* Prints out the MUAN Prompt. */
void print_prompt(char *msg) {
  printf("%s%s %s%s%s", MAGENTA, MUAN_PROMPT, YELLOW, msg, RST);
}

/* Prints out the Help Menu. */
void print_help() {
  print_ni(""); 
  printf("%s", YELLOW);
  printf(".----------------------------------.\n");
  printf("| Assignment (Works with values, variables, or expressions)\n");
  printf("| %svar = 2.0%s       | Assigns a value to a variable\n", CYAN, YELLOW);
  printf("| %svar = x%s         | Assigns a variable's value to another variable\n", CYAN, YELLOW);
  printf("| %svar = (x + 1)%s   | Assigns an expression's value to a variable\n", CYAN, YELLOW);
  printf("| %svar += x%s        | Assigns var + expression to var\n", CYAN, YELLOW);
  printf("| %svar -= x%s        | Assigns var - expression to var\n", CYAN, YELLOW);
  printf("| %svar *= x%s        | Assigns var * expression to var\n", CYAN, YELLOW);
  printf("+-------*--------------------------.\n");
  printf("| Functions (Works with values, variables, or expressions)\n");
  printf("| %sprint(-4.13)%s    | Prints a value\n", CYAN, YELLOW);
  printf("| %sprint(val)%s      | Prints variable val\n", CYAN, YELLOW);
  printf("| %sprint(x+y+2.3)%s  | Prints expressions\n", CYAN, YELLOW);
  printf("| %sdisplay(x)%s      | Shows the binary representation of a variable\n", CYAN, YELLOW);
  printf("| %sdisplay(1.5)%s    | Shows the binary representation of a value\n", CYAN, YELLOW);
  printf("| %sdisplay(3.1 + x)%s| Shows the binary representation of an expression\n", CYAN, YELLOW);
  printf("+----------------------------------.\n");
  printf("| Operations (Works with values, variables, or expressions)\n");
  printf("| %sx + y%s           | Addition:       x + y\n", CYAN, YELLOW);
  printf("| %sx - y%s           | Subtraction:    x - y\n", CYAN, YELLOW);
  printf("| %sx * y%s           | Multiplication: x * y\n", CYAN, YELLOW);
  printf("| %s-x%s              | Negation:       -x\n", CYAN, YELLOW);
  printf("| %sx++%s             | Post-Increment: x++\n", CYAN, YELLOW);
  printf("| %sx--%s             | Post-Decrement: x--\n", CYAN, YELLOW);
  printf("| %s++x%s             | Pre-Increment:  ++x\n", CYAN, YELLOW);
  printf("| %s--x%s             | Pre-Decrement:  --x\n", CYAN, YELLOW);
  printf("+----------------------------------.\n");
  printf("| Commands \n");
  printf("| %shelp%s            | Prints out this Help\n", CYAN, YELLOW);
  printf("| %sexit%s            | Quits MUAN\n", CYAN, YELLOW);
  printf("| %squit%s            | Quits MUAN\n", CYAN, YELLOW);
  printf("+----------------------------------.\n");
  printf("| You can combine operations, variables, values, and expressions\n");
  printf("| ex. %sval = 1.2 - (a * foo) * (1.4 - x)%s\n", CYAN, YELLOW);
  printf("| ex. %sprint(bar + (x - y + 1.25) * foo)%s\n", CYAN, YELLOW);
  printf(".----------------------------------\n");
  printf("%s", RST);
}

/* Prints out a string only if not Interactive Mode */
void print_ni(char *msg) {
  if(!IS_INTERACTIVE) {
    printf("%s\n", msg);
  }
}

/* Prints out a Number from the user */
void print_num(char *var, struct number_struct *num, int precision, long result) {
  if(num->is_infinity) {
    printf("\r%s%s%s = %s%sInfinity%s%c", GREEN, var, RST, GREEN, num->is_negative?"-":"", RST,IS_INTERACTIVE?'\n':'\0');
  }
  else if(num->is_nan) {
    printf("\r%s%s%s = %sNaN%s%c", GREEN, var, RST, GREEN, RST, IS_INTERACTIVE?'\n':'\0');
  }
  else {
    printf("\r%s%s%s = %s%s%d.%0*ld%s%c", GREEN, var, RST, GREEN, num->is_negative?"-":"", 
                                          num->whole, precision, result, RST, IS_INTERACTIVE?'\n':'\0');
  }
}

/* Prints out an error message - variable not found */
void print_sym_not_found(char *str) {
  printf("\rVariable %s Not Found. (Using Value 0.0)\n", str);
}

/* Disables Buffering on a Valid Stream */
void disable_buffering(FILE *stream) {
  if(stream) {
    setvbuf(stream, NULL, _IONBF, 0);
  }
}

/* Enables Buffering on a Valid Stream */
void enable_buffering(FILE *stream) {
  if(stream) {
    setvbuf(stream, NULL, _IOLBF, 0);
  }
}
