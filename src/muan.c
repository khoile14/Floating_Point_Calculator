#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "symtab.h"
#include "common_functions.h"

// Externally defined (Shared Global) Variables
extern int yyparse();

// Private Prototypes
static void register_handlers();

// Welcome and begin parsing the script/running user input
int main(int argc, char *argv[]) {
  if(argc == 2) {
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1) {
      printf("[Error] File %s not found. Continuing in interactive mode.\n", argv[1]);
    }
    else {
      dup2(fd, STDIN_FILENO);
    }
  }

  register_handlers();
  initialize_symtab();

  printf("%sWelcome to the Micro-Ubiquitous Accounting Notary (MUAN) programmable calculator.%s\n", CYAN, RST);
  print_prompt("$ ");

  // Run the MUAN Language Parser
  int ret = yyparse();
  teardown_symtab();
  return ret;
}

// Runs whenever user quits MUAN (or Ctrl-C exits)
void clean_exit(void) {
  printf("\n");
  printf("Have a nice day!\n");
}

// Handles user pressing Ctrl-C to trigger the clean_exit code
void handle_interrupts(int sig) {
  exit(0);
}

// Registers the exit() and CTRL-C Handlers
static void register_handlers() {
  atexit(clean_exit);
  struct sigaction sa = {0};
  sa.sa_handler = handle_interrupts;
  sigaction(SIGINT, &sa, NULL);
}
