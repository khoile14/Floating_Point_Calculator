

#ifndef TINYSF_H
#define TINYSF_H

#include "tinysf_precision.h"
#include "common_structs.h"

/* Public Types for Use */
typedef unsigned int tinysf_s;

/* We have no doubles or floats in this system */
#define double long // Use a signed 64-bit integer type.
#define float int // Use a signed 32-bit integer type.

/* Public TINYSF Library Functions */
tinysf_s toTinySF(Number_s *num);
int toNumber(Number_s *num, tinysf_s value);
tinysf_s negTinySF(tinysf_s value);
tinysf_s addTinySF(tinysf_s val1, tinysf_s val2);
tinysf_s subTinySF(tinysf_s val1, tinysf_s val2);
tinysf_s mulTinySF(tinysf_s val1, tinysf_s val2);

#endif
