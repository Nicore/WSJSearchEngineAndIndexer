/*
 * parse.c
 */
#include <stdio.h>
#include "index.h"
#include <sys/types.h>
#include <regex.h>
/*#include <ctype.h>*/


regex_t rx_start;
regex_t rx_end;
regex_t rx_word;
regex_t rx_sym;

int regstart;
int regend;
int regword;
int regsym;

char msgbuf[100];
const char *xmlTstt = "<DOCNO>";
const char *xmlTend = "</DOCNO>";
const char *wordT0 = "t";
const char *wordT1 = "asdf";
const char *wordT2 = "thi213";
const char *wordT3 = "O'ke-thle";
const char *symT = "&lt;";
const char *symT2 = "& and ther was text";
const char *word_regex = "[a-zA-Z0-9][a-zA-Z0-9]*";

/*needs to define:*/
void parse(FILE *stream) {

   stream = NULL;
   regstart = regcomp(&rx_start, "<[a-zA-Z][a-zA-Z0-9]*>", 0);
   regend = regcomp(&rx_end, "</[a-zA-Z][a-zA-Z0-9]*>", 0);
   regword = regcomp(&rx_word, word_regex, 0);
   regsym = regcomp(&rx_sym, "&[a-zA-Z0-9]*;", 0);

   if (regstart) {
      fprintf(stderr, "Couldn't compile regstart\n");
   }
   if (regend) {
      fprintf(stderr, "Couldn't compile regend\n");
   }
   if (regsym) {
      fprintf(stderr, "Couldn't compile regsym");
   }
   if (regword) {
      fprintf(stderr, "Couldn't compile regword\n");
      
   }
   

   if (!regexec(&rx_start, xmlTstt, 0, NULL, 0)) {
      printf("xmlTstt match success\n");
   }
   if (!regexec(&rx_end, xmlTend, 0, NULL, 0)) {
      printf("xmlTend match success\n");
   }
   regword = regexec(&rx_word, wordT0, 0, NULL, 0);
   if (!regword) {
      printf("wordT0 match success\n");
   } else if (regword == REG_NOMATCH) {
      printf("wordT0 doesn't match\n");
   } else {
      regerror(regword, &rx_word, msgbuf, sizeof(msgbuf));
      fprintf(stdout, "Regex match failed: %s\n", msgbuf);
   }

   
   if (!regexec(&rx_word, wordT1, 0, NULL, 0)) {
      printf("wordT1 match success\n");
   }
   if (!regexec(&rx_word, wordT2, 0, NULL, 0)) {
      printf("wordT2 match success\n");
   }
   if (!regexec(&rx_word, xmlTstt, 0, NULL, 0)) {
      printf("wordT3 match success\n");
   }
   if (!regexec(&rx_sym, symT, 0, NULL, 0)) {
      printf("symT match success\n");
   }
   if (!regexec(&rx_sym, symT2, 0, NULL, 0)) {
      printf("symT2 match success, so that's bad");
   }
   

   if (!regexec(&rx_start, xmlTend, 0, NULL, 0)) {
      printf("somethings wrong with the regexes");
   }
   if (!regexec(&rx_end, xmlTstt, 0, NULL, 0)) {
      printf("somethings wrong with the regexes");
   }
   if (!regexec(&rx_sym, wordT0, 0, NULL, 0)) {
      printf("somethings wrong with the regexes");
   }
   
   
   
   /*needs to call*/
    
   /*start_tag(char const *name);
   end_tag(char const *name);
   word(char const *word);*/

   regfree(&rx_start);
   regfree(&rx_end);
   regfree(&rx_word);
   regfree(&rx_sym);
   printf("end parse\n");

}


