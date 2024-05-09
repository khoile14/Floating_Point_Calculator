#include <stdio.h>
#include <stdlib.h>
#include "common_structs.h"
#include "common_definitions.h"
#include "common_functions.h"
#include "tinysf.h"
// ----------Public API Functions (write these!)-------------------

void convertToWhole(Number_s *num, int *E) { // convert the fraction to whole
    num->whole <<= 1; // shift the whole part left
    num->whole |= (num->fraction >> 31); // move the most significant bit of the fraction part to the least significant bit of the whole part
    num->fraction <<= 1; // shift the fraction part left
    (*E)--; // decrement the E because shifting left
}

void leftShiftHelper(Number_s *number, int *E_of_number, int *E2) { // left shift the number
    int count = *E2 - *E_of_number; // get the count of the difference between E2 and E_of_number
    unsigned int msb = 0; // most significant bit
    while (count > 0) { // while there are more bits
        number->whole <<= 1; // shift the whole part left
        msb = number->fraction >> 31; // get the most significant bit of the fraction part
        number->whole |= msb; // move the most significant bit of the fraction part to the least significant bit of the whole part
        number->fraction <<= 1; // shift the fraction part left
        (*E2)--; // decrement E2
        count--; // decrement the count
    }
}

void rightShiftHelper(Number_s *number, int *E_of_number, int *E2){ // right shift the number
    int count = *E_of_number - *E2; // get the count of the difference between E_of_number and E2
    unsigned int lsb = 0; // least significant bit
    while(count > 0){ // while there are more bits
        lsb = number->whole & 1; // get the least significant bit of the whole part
        number->whole >>= 1; // shift the whole part right
        number->fraction >>= 1; // shift the fraction part right
        number->fraction |= lsb << 31; // move the least significant bit of the whole part to the most significant bit of the fraction part
        (*E2)++; // increment E2
        count--; // decrement the count
    }
}

int count_E(Number_s *number) { // count the E
    unsigned int lsb = 0; /* least significant bit */
    unsigned int msb = 0; /* most significant bit */
    int count = 0; /* count of bits */
    if (number->whole == 0) { /* if the whole part is 0 */
        while (number->whole < 1) { /* while the whole part is less than 1 */
            msb = number->fraction >> 31 & 1; /* get the most significant bit of fraction*/
            number->whole <<= 1; /* shift the whole part left */
            number->whole |= msb; /* move the most significant bit of the fraction part to the least significant bit of the whole part */
            number->fraction <<= 1; /* shift the fraction part left */
            count--; /* decrement the count */
        }
    } else { /* if the whole part is not 0 */
        while (number->whole > 1) { /* while there are more bits */
            lsb = number->whole & 1; /* get the least significant bit */
            number->whole >>= 1; /* shift the whole part right */
            number->fraction >>= 1; /* shift the fraction part right */
            number->fraction |= lsb << 31; /* move the least significant bit of the whole part to the most significant bit of the fraction part */
            count++; /* increment the count */
        }
    }
    return count; /* return the count of E*/
}


unsigned int round_frac(Number_s *number, int *E) {
    unsigned int lsb = (number->fraction >> 25) & 1; // getting the least significant bit of the frac part
    unsigned int next_to_lsb = (number->fraction >> 24) & 1; // getting the bit that is next to lsb
    unsigned int lower_bits = !!(number->fraction & 0x00FFFFFF); // get the result of lower bits (either 1 or 0)
    unsigned int upper7_bits = (number->fraction & 0xFE000000) >> 25; // getting the upper 7 bits in fraction
    // if the bit next to lsb and the lower bits is 1 or the lsb is 1 or the bit next to it is 1 and the lower bit is 0
    if ((next_to_lsb == 1 && lower_bits == 1) || (lsb == 1 && next_to_lsb == 1 && lower_bits == 0)) {
        if (upper7_bits != 0x7F) { // check if the upper 7 bits is all 1s
            upper7_bits += 1; // if not then add 1 to the upper 7 bits
            number->fraction = upper7_bits; // set the fraction to the new upper 7 bits
            return number->fraction;
        } else {
            number->fraction = 0; // if it is all 1 then set the fraction to 0s
            number->whole += 1; // add 1 to the whole
            (*E)++; // increment the E
            return number->fraction; // return the fraction
        }
    }
    return upper7_bits; // return the upper 7 bits
}

tinysf_s toTinySF(Number_s *number) { // convert the number to tinySF

    if(number == NULL){ // check if the number is NULL
        return 0x788; // return NaN
    }

    if(number->is_nan && number->is_infinity){ // check if it is NaN and infinity
        return 0x788 | (number->is_negative << 11); // return NaN with its sign
    }

    if(number->is_infinity && number->fraction == 0){ // check for infinity
        return 0x780 | (number->is_negative << 11); // return infinity with its sign
    }

    if(number->is_nan ){ // check for NaN
        return 0x788 | (number->is_negative << 11); // return NaN with its sign
    }

    if(number->whole == 0 && number->fraction == 0) { //check if 0
        number->is_infinity = 0; // set infinity to 0
        number->is_nan = 0; // set NaN to 0
        return 0x0 | (number->is_negative << 11) ; // return 0 with its sign
    }

    unsigned int E = count_E(number); // get the E
    int bias = 7; // set the bias
    unsigned int exp = E + bias; // get the exp

    unsigned int S = number->is_negative; // get the sign
    unsigned int frac = round_frac(number, &E); // get the fraction after rounding
    exp = E + bias; // get the exp again

    if(exp <= 0){ // check for underflow
        return 0 | (S << 11); // return 0 with its sign
    }else if(exp >= 15){ //check for overflow
        number->is_infinity = 1; // set infinity to 1
        number->is_nan = 0; // set NaN to 0
        return 0x780 | (S << 11); // return infinity with its sign
    }

    return (S << 11) | (exp << 7) | frac; // return the tinySF value 12 bits encoding
}

/* toNumber - Converts a TinySF Value into a Number Struct (whole and fraction parts)
*  - number is managed by MUAN, DO NOT FREE or re-Allocate number.
*    - It is already allocated.  Do not call malloc/calloc for number.
*  - Follow the project documentation for this function.
*  If the conversion is successful, return 0.
*  - If number is NULL or there are any inconsistencies, return -1.
*/
int helperForToNumberThatReturnE(Number_s *number, tinysf_s value){ // helper function for toNumber that returns E
    unsigned int S = (value & 0x800) >> 11; // extract S
    unsigned int exp = (value & 0x780) >> 7; // extract exp
    unsigned int frac = (value & 0x07F) << 25; //extract frac
    int E = (int) exp - 7; // get E

    if(exp == 0xF && (frac >> 25) == 0x0){ // check if it is infinity
        number->is_infinity = 1; // set infinity to 1
    }else{
        number->is_infinity = 0; // set infinity to 0
    }
    if(exp == 0xF && (frac >> 25) != 0x0 ){ // check if it is NaN
        number->is_nan = 1; // set NaN to 1
    }else{
        number->is_nan = 0; // set NaN to 0
    }

    number->whole = 1; // set whole to 1

    if(exp <= 0){ // check if it is underflow
        number->is_negative = S;
        number->whole = 0;
        number->fraction = 0;
    }


    if (E > 0) { // check if E is greater than 0
        while (E > 0) { //loop until E is 0
            number->whole <<= 1; // shift the whole part left
            number->whole |= (frac >> 31); // move the most significant bit of the fraction part to the least significant bit of the whole part
            frac <<= 1; // shift the fraction part left
            E--; // decrement E
        }
    } else {
        while (E < 0) { // loop until E is 0
            frac >>= 1; // shift the fraction part right
            frac |= (number->whole & 1) << 31; // move the least significant bit of the whole part to the most significant bit of the fraction part
            number->whole >>= 1; // shift the whole part right
            E++; // increment E
        }
    }

    number->is_negative = S; // set the sign for the number
    number->fraction = frac; // set the fraction for the number

    return E; // return E
}

int toNumber(Number_s *number, tinysf_s value) { // convert the tinySF to number
    if (number == NULL) { // check if the number is NULL
        return -1;
    }

    helperForToNumberThatReturnE(number, value); // call the helper function
    return 0; // return 0 if successful
}

/* mulTinySF - Performs an operation on two tinySF values
*  - Follow the project documentation for this function.
* Return the resulting tinysf_s value
*/
tinysf_s mulTinySF(tinysf_s val1, tinysf_s val2) { // multiply two tinySF values
    unsigned int exp1 = (val1 & 0x780) >> 7; // extract exp1 from val1
    unsigned int exp2 = (val2 & 0x780) >> 7; // extract exp2 from val2

    int E1 = (int) exp1 - 7; // get E1 from val1
    int E2 = (int) exp2 - 7; // get E2 from val2

    Number_s *num1 = malloc(sizeof(Number_s)); // allocate memory for num1
    Number_s *num2 = malloc(sizeof(Number_s)); // allocate memory for num2
    Number_s *result = malloc(sizeof(Number_s)); // allocate memory for result

    E1 = helperForToNumberThatReturnE(num1, val1); // convert the 12 bits encoding into the binary of the original number with its E

    E2 = helperForToNumberThatReturnE(num2, val2); // convert the 12 bits encoding into the binary of the original number with its E

    if (E1 > E2) { // check if E1 is greater than E2
        rightShiftHelper(num1, &E1, &E2); // right shift the number
    } else if (E2 > E1) { // check if E2 is greater than E1
        leftShiftHelper(num2, &E1, &E2); // left shift the number
    }

    while (num1->fraction != 0 || num2->fraction != 0) { //loop until both numbers have whole part and no fraction part
        convertToWhole(num1, &E1); // convert the fraction to whole
        convertToWhole(num2, &E2); // convert the fraction to whole
    }

    unsigned int E = E1 + E2; // get the E from both number by adding them

    unsigned int S = num1->is_negative ^ num2->is_negative; // get the sign by XORing the signs of the numbers

    //check for mutiplication between 0 and infinity
    if(((num1->whole == 0 && num1->fraction == 0) && num2->is_infinity) || ((num2->whole == 0 && num2->fraction == 0) && num1->is_infinity)){
        result->is_nan = 1; // set NaN to 1
        return toTinySF(result); // return the result (should be NaN)
    }else if(num1->is_infinity && num2->is_infinity || (num1->is_infinity || num2->is_infinity)){ // check for multiplication between infinity or at least one of them is infinity
        result->is_infinity = 1; // set infinity to 1
        result->whole = 0x780; //set result whole to infinity
    }else if(num1->is_nan || num2->is_nan){ // check for multiplication between NaN
        result->is_nan = 1; // set NaN to 1
        result->whole = 0x788; //set it to NaN
    }else{ // if none of the above
        result->is_infinity = 0; // set infinity to 0
        result->is_nan = 0; // set NaN to 0
        result->whole = num1->whole * num2->whole; // multiply the whole parts like normal
    }


    result->fraction = 0; // set the fraction to 0
    unsigned int lsb = 0; // least significant bit
    while(E != 0){ // loop until E is 0
        lsb = result->whole & 1; // get the least significant bit of the whole part
        result->whole >>= 1; // shift the whole part right
        result->fraction >>= 1; // shift the fraction part right
        result->fraction |= lsb << 31; // move the least significant bit of the whole part to the most significant bit of the fraction part
        E++; // increment E
    }

    result->is_negative = S; // set the sign for the result
    return toTinySF(result); // return the result
}


/* addTinySF - Performs an operation on two tinySF values
*  - Follow the project documentation for this function.
* Return the resulting tinysf_s value
*/
tinysf_s addTinySF(tinysf_s val1, tinysf_s val2) { // add two tinySF values

    unsigned int exp1 = (val1 & 0x780) >> 7; // extract exp1 from val1
    unsigned int exp2 = (val2 & 0x780) >> 7; // extract exp2 from val2

    int E1 = (int) exp1 - 7; // get E1 from val1
    int E2 = (int) exp2 - 7; // get E2 from val2

    Number_s *num1 = malloc(sizeof(Number_s)); // allocate memory for num1
    Number_s *num2 = malloc(sizeof(Number_s)); // allocate memory for num2
    Number_s *result = malloc(sizeof(Number_s)); // allocate memory for result

    E1 = helperForToNumberThatReturnE(num1, val1); // convert the 12 bits encoding into the binary of the original number
    E2 = helperForToNumberThatReturnE(num2, val2); // convert the 12 bits encoding into the binary of the original number

    if (E1 > E2) { // check if E1 is greater than E2
        rightShiftHelper(num1, &E1, &E2); // right shift the number
    } else if (E2 > E1) { // check if E2 is greater than E1
        leftShiftHelper(num2, &E1, &E2); // left shift the number
    }

    while (num1->fraction != 0 || num2->fraction != 0) { //loop until both numbers have whole part and no fraction part
        convertToWhole(num1, &E1); // convert the fraction to whole
        convertToWhole(num2, &E2); // convert the fraction to whole
    }


    int a = num1->whole; // get the whole part of num1
    int b = num2->whole; // get the whole part of num2

    //check for addition between NaN and non-NaN or NaN and NaN
    if((num1->is_nan && !num2->is_nan) || (num2->is_nan && !num1->is_nan) || (num1->is_nan && num2->is_nan)){
        return 0x788; // return NaN
    }
    if(num1->is_infinity && !num2->is_infinity){ // check if the first number is infinity and the second is not
        return 0x780 | (num1->is_negative << 11); // return infinity with its sign
    }else if(num2->is_infinity && !num1->is_infinity){ // check if the second number is infinity and the first is not
        return 0x780 | (num2->is_negative << 11); // return infinity with its sign
    }
    if(num1->is_infinity && num2->is_infinity){ // check if both numbers are infinity
        if(num1->is_negative != num2->is_negative){ // check if the signs are different
            return 0x788; // return NaN
        }
    }

    if(num1->is_negative != num2->is_negative){ // check if the signs are different
        if(a > b){ // check if a is greater than b
            result->whole = a - b; // set the whole to a - b
            result->is_negative = num1->is_negative; // set the sign to the sign of num1
        }else if(a < b){ // check if a is less than b
            result->whole = b - a; // set the whole to b - a
            result->is_negative = num2->is_negative; // set the sign to the sign of num2
        }else{ // if a is equal to b
            result->whole = 0; // set the whole to 0
            result->is_negative = 0; // set the sign to 0
        }
    }else{ // if the signs are the same
        result->whole = a + b; // set the whole to a + b
        result->is_negative = num2->is_negative; // set the sign to the sign of num2
    }

    int E = E1; // set E to E1
    result->fraction = 0; // set the fraction to 0
    unsigned int lsb = 0; // least significant bit
    while(E != 0){ // loop until E is 0
        lsb = result->whole & 1; // get the least significant bit of the whole part
        result->whole >>= 1; // shift the whole part right
        result->fraction >>= 1; // shift the fraction part right
        result->fraction |= lsb << 31; // move the least significant bit of the whole part to the most significant bit of the fraction part
        E++; // increment E
    }
    return toTinySF(result); // return the result
}

/* negTinySF - Negates a TinySF Value.
*  - Follow the project documentation for this function.
* Return the resulting TinySF Value
*/
tinysf_s negTinySF(tinysf_s value) {
    return value^0x800; // return the negated value
}

/* opTinySF - Performs an operation on two tinySF values
*  - Follow the project documentation for this function.
* Return the resulting tinysf_s value
*/
tinysf_s subTinySF(tinysf_s val1, tinysf_s val2) {
    val2 = negTinySF(val2); // negate the second value
    tinysf_s result = addTinySF(val1, val2); // add the first value with the negated second value
    return result; // return the result
}
