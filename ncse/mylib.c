/**
 * @file mylib.c
 * @author Nick Comer
 * @date September 2010
 *
 * This file contains the functions getword, emalloc and print_key
 * that are used in tree.c, also contains the function erealloc.
 */

#include <stdio.h> /*for fprintf*/
#include <stdlib.h> /*for size_t, malloc, realloc, exit*/
#include "mylib.h"
#include <assert.h>
#include <ctype.h>

/**
 *
 * An error-checking malloc.
 *
 * @param s the size of the memory to be allocated.
 *
 * @return result the parameter with memory allocated or an error message if
 *                unsuccessful.
 */
void *emalloc(size_t s) {
   void *result = malloc(s);
   if (NULL == result) {
      fprintf(stderr, "mem alloc fail\n");
      exit(EXIT_FAILURE);
   }
   return result;

}

/**
 * An error-checking realloc.
 *
 * @param p a pointer to the memory location to be reallocated.
 * @param s the size of the memory to be allocated.
 *
 * @return my_array the reallocated parameter or an error message if
 *                  unsuccessful.
 */
void *erealloc(void *p, size_t s) {
   void *result = realloc(p, s);
   if (NULL == result) {
      fprintf(stderr, "mem realloc fail\n");
      exit(EXIT_FAILURE);
   }
   return result;
}

/**
 * Gets a word from a file, converts it to lower case and then converts it
 * to an integer.
 *
 * @param s the word that is being retrieved.
 * @param limit the size of the word.
 * @param file the file the word is being read in from.
 *
 * @return w-s the integer representation of the word or EOF if the end of
 *             the file is reached.
 */
int getword(char *s, int limit, FILE *stream) {
   int c;
   char *w = s;
   /*assert(limit > 0 && s != NULL && stream != NULL);*/

   /*skip to start of word */
   while (!isalnum(c = getc(stream)) && EOF != c)
      ;
   if (EOF == c) {
      return EOF;
   } else if (--limit > 0) { /*reduce limit by 1 to allow for \0*/
      *w++ = c;
   }
   while (--limit > 0) {
      if (isalnum(c = getc(stream)) || c == '\'' || c == '-') {
         *w++ = c;
      } else if ('\'' == c) {
         limit++;
      } else {
         break;
      }
   }
   *w = '\0';
   return w - s;
}

/**
 * A print method that takes in two character strings and prints out the first
 * string and the second depending on if it's NULL or not.
 *
 * @param s the first character string to print out. In the tree ADT's case;
 *          the string of each node.
 * @param c the second character string to print out. In the tree ADT's case;
 *          the colour of the node.
 */
void print_key(char *s, char *c) {
   if (c == NULL) {
      printf("%s\n", s);
   } else {
      printf("%-6s %s\n", c, s);
   }
}
