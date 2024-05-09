%{
#include <stdio.h>
#include <unistd.h>
#include "tinysf.h"
#include "symtab.h"
#include "common_structs.h"
#include "common_functions.h"

#if DEBUG
  #define MSG(str, ...) do {                    \
    printf("  %s[MSG]%s ", MAGENTA, RST);       \
    printf(str, ##__VA_ARGS__);                  \
    printf("    %s\n", RST); \
  } while(0)
#else
  #define MSG(str, ...)
#endif

  int yylex();
  int yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
  }

%}

%code requires {
#include <stdlib.h>
#include "common_structs.h"
}

%union {
  struct number_struct fpval;
  int number;
  char *strval;
};

%expect 2
%token <number> NUMBER 
%token <fpval> FLOAT INF_T NAN_T
%token HELP_T PRINT_T DISPLAY_T EXIT_T SYNERR_T VARIABLE EOL 

%type <number> expression
%type <strval> VARIABLE

%right COMPADD COMPSUB COMPMUL
%right '='
%left '-' '+'
%left INCR DECR
%left '*' 
%left '(' ')'
%precedence UNARY

%%

start: %empty
  | EXIT_T {printf("%c%sSIC PARVIS MAGNA%s\n", IS_INTERACTIVE?'\r':'\n', CYAN, RST); return 42;}
  | expression EOL {print_prompt("$ ");} start
  | EOL {print_prompt("$ ");} start
  | error EOL {print_prompt("$ ");} start
  ;

expression:  NUMBER           { $$ = $1; }
  | FLOAT                     { MSG("float");
                                $1.fraction = fraction_to_binary($1.conversion.fracorig, $1.conversion.precision);
                                $1.conversion.precision = 0;
                                $1.is_nan = 0;
                                $1.is_infinity = 0;
                                $$ = toTinySF(&$1); ECHONOTTY("\r"); }
  | VARIABLE                  { MSG("VARIABLE");
                                $$ = sym_exists($1)?get_value($1):0; 
                                sym_exists($1)?1:print_sym_not_found($1); ECHONOTTY("\r");}
  | INF_T                     { MSG("INF_T");
                                $1.fraction = 0;
                                $1.conversion.precision = 0;
                                $1.is_nan = 0;
                                $1.is_infinity = 1;
                                $1.is_negative = 0;
                                $$ = toTinySF(&$1); ECHONOTTY("\r");}
  | NAN_T                     { MSG("NAN_T");
                                $1.fraction = 0;
                                $1.conversion.precision = 0;
                                $1.is_nan = 1;
                                $1.is_infinity = 0;
                                $1.is_negative = 0;
                                $$ = toTinySF(&$1); ECHONOTTY("\r");}
  | '-' expression %prec UNARY{ MSG("-X");
                                $$ = negTinySF($2); }
  | '+' expression %prec UNARY{ MSG("+X");
                                $$ = $2; }
  | INCR VARIABLE %prec UNARY{  MSG("++X");
                                int orig_val = sym_exists($2)?get_value($2):0; 
                                int new = orig_val;
                                if(sym_exists($2)) {
                                  Number_s num = {0,0,0,1,0};
                                  new = addTinySF(orig_val, toTinySF(&num));
                                  insert_symbol($2, new);
                                } else { 
                                  print_sym_not_found($2);
                                }
                                ECHONOTTY("\r");
                                $$ = new;}
  | DECR VARIABLE %prec UNARY{  MSG("--X");
                                int orig_val = sym_exists($2)?get_value($2):0; 
                                int new = orig_val;
                                if(sym_exists($2)) {
                                  Number_s num = {0,0,0,1,0};
                                  new = subTinySF(orig_val, toTinySF(&num));
                                  insert_symbol($2, new);
                                } else { 
                                  print_sym_not_found($2);
                                }
                                ECHONOTTY("\r");
                                $$ = new;}
  | VARIABLE INCR %prec UNARY{  MSG("X++");
                                int orig_val = sym_exists($1)?get_value($1):0; 
                                if(sym_exists($1)) {
                                  Number_s num = {0,0,0,1,0};
                                  int new = addTinySF(orig_val, toTinySF(&num));
                                  insert_symbol($1, new);
                                } else { 
                                  print_sym_not_found($1);
                                }
                                ECHONOTTY("\r");
                                $$ = orig_val;}
  | VARIABLE DECR %prec UNARY{  MSG("X--");
                                int orig_val = sym_exists($1)?get_value($1):0; 
                                if(sym_exists($1)) {
                                  Number_s num = {0,0,0,1,0};
                                  int new = subTinySF(orig_val, toTinySF(&num));
                                  insert_symbol($1, new);
                                } else { 
                                  print_sym_not_found($1);
                                }
                                ECHONOTTY("\r");
                                $$ = orig_val;}
  | expression '+' expression { MSG("X+X");
                                $$ = addTinySF($1, $3); }
  | expression '-' expression { MSG("X-X");
                                $$ = subTinySF($1, $3); }
  | expression '*' expression { MSG("X*X");
                                $$ = mulTinySF($1, $3); }
  | '(' expression ')'        { MSG("(X)");
                                $$ = $2; }
  | VARIABLE '=' expression   { MSG("X = Y");
                                $$ = $3; insert_symbol($1, $3); }
  | VARIABLE COMPADD expression  {  MSG("X += Y");
                                int orig_val = sym_exists($1)?get_value($1):0; 
                                int new = orig_val;
                                if(sym_exists($1)) {
                                  new = addTinySF(orig_val, $3);
                                  insert_symbol($1, new);
                                } else { 
                                  print_sym_not_found($1);
                                }
                                ECHONOTTY("\r");
                                $$ = new;}
  | VARIABLE COMPSUB expression  {  MSG("X -= Y");
                                int orig_val = sym_exists($1)?get_value($1):0; 
                                int new = orig_val;
                                if(sym_exists($1)) {
                                  new = subTinySF(orig_val, $3);
                                  insert_symbol($1, new);
                                } else { 
                                  print_sym_not_found($1);
                                }
                                ECHONOTTY("\r");
                                $$ = new;}
  | VARIABLE COMPMUL expression  {  MSG("X *= Y");
                                int orig_val = sym_exists($1)?get_value($1):0; 
                                int new = orig_val;
                                if(sym_exists($1)) {
                                  new = mulTinySF(orig_val, $3);
                                  insert_symbol($1, new);
                                } else { 
                                  print_sym_not_found($1);
                                }
                                ECHONOTTY("\r");
                                $$ = new;}
  | HELP_T                    { MSG("HELP_T");
                                $$ = 0; print_help(); }
  | PRINT_T '(' VARIABLE ')'  { MSG("print(X)");
                                struct number_struct *num = calloc(1, sizeof(Number_s));
                                sym_exists($3)?toNumber(num, get_value($3)):0;
                                int precision = 0;
                                long result = binary_to_fraction(num->fraction, &precision);
                                ECHOTTY("\n");
                                sym_exists($3)?print_num($3, num, precision, result):print_sym_not_found($3);
                                free(num);
                                $$ = get_value($3);
                              }
  | PRINT_T '(' expression ')'  { MSG("print(exp)");
                                struct number_struct *num = calloc(1, sizeof(Number_s));
                                toNumber(num, $3);
                                int precision = 0;
                                long result = binary_to_fraction(num->fraction, &precision);
                                ECHOTTY("\n");
                                print_num("Value", num, precision, result);
                                free(num);
                                $$ = $3;
                              }
  | DISPLAY_T '(' VARIABLE ')'  { MSG("display(X)");
                                $$ = get_value($3);
                                ECHOTTY("\n");
                                sym_exists($3)?print_value("TinySF Value in Binary", $$):print_sym_not_found($3);
                              }
  | DISPLAY_T '(' expression ')'  { MSG("display(exp)");
                                $$ = $3;
                                ECHOTTY("\n");
                                print_value("TinySF Value in Binary", $$);
                              }
  | SYNERR_T                  { MSG("ERROR");
                                printf("\rCOMMAND Error\n"); }
  ;

%%

extern int yylex();
extern int yyparse();
extern int yyerror();
extern FILE *yyin;

