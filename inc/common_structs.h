

#ifndef COMMON_STRUCTS_H
#define COMMON_STRUCTS_H

// Ignore factoring_struct, this is for the MUAN internal use only
typedef struct factoring_struct {
  char original[255];   // Reference String for Original User Input
  long fracorig;
  int precision;
} Factoring_s;

// This is the main struct you'll be working with
typedef struct number_struct {
  int is_negative;        // 1 if Negative, 0 if Positive        (eg. 3.25, whole = 3, is_negative = 0)
                          //                                     (eg.-3.25, whole = 3, is_negative = 1)
  int is_infinity;        // 1 if Infinity, 0 Otherwise  
  int is_nan;             // 1 if NaN, 0 Otherwise
  unsigned int whole;     // Whole Portion of Number             (eg. 3.25, whole = 3) 
  unsigned int fraction;  // 32-bit Fraction Portion in Binary   (eg. 3.25, frac  = 0x40000000)
  Factoring_s conversion; // Ignore.  Only used for internal conversions by MUAN.
} Number_s;

#endif