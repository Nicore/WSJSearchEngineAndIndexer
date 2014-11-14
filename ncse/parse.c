/*
 * parse.c
 */
#include <stdio.h>
#include <stdlib.h>
#include "index.h"
#include <ctype.h>
#include "mylib.h"
#include "flexarray.h"



char *word_buff;
int word_cap = 20;
int word_len = 0;

static void fail(void) {
   fprintf(stderr, "Invalid xml syntax alert!\n");
}

static void flex_to_strn(flexarray f) {
   int i;
   word_len = flexarray_size(f);
   if (word_len >= word_cap) {
      word_cap += word_cap;
      word_buff = erealloc(word_buff, word_cap+1 * sizeof word_buff[0]);
   }
   
   for (i = 0; i < word_len; i++) {
      word_buff[i] = flexarray_get(f, i);
   }
   word_buff[word_len] = '\0';
}

/*needs to define:*/
void parse(FILE *stream) {
   
   int c = 0;
   int d = 0;
   flexarray str = flexarray_new();

   word_buff = emalloc(word_cap * sizeof word_buff[0]);

   /*get initial character*/
   c = getc(stream);

   while (c != EOF) {
      if (c == '<') { /*if it's the start of an xml tag*/
         flexarray_append(str, c);
         c = getc(stream);
         if (c == '/') { /*if the start of an end tag*/
            flexarray_append(str, c);
            c = getc(stream);
            if (!isalpha(c)) fail();
            while (isalnum(c)) {
               flexarray_append(str, c);
               c = getc(stream);
            }
            if (c != '>') fail();
            flexarray_append(str, c);
            /*completed an end tag*/
            flex_to_strn(str);
            end_tag(word_buff);
            
            /*c = getc(stream);*/
         } else { /*it's a start tag then*/
            if (!isalpha(c)) fail();
            flexarray_append(str, c);
            c = getc(stream);
            while (isalnum(c)) {
               flexarray_append(str, c);
               c = getc(stream);
            }
            if (c != '>') fail();
            flexarray_append(str, c);
            /*completed a start tag*/
            flex_to_strn(str);
            start_tag(word_buff);
            
            /*c = getc(stream);*/
         } /*end start tag*/
        
      } else if (c == '&') { /*if it's a symbol*/
         c = getc(stream);
         if (isalpha(c)) {
            while (isalpha(c)) {
               c = getc(stream);
            }
            if (c != ';') fail();
         }
         /*c = getc(stream);*/
         /*end of symbol*/
      } else if (isalnum(c)) {
         flexarray_append(str, c);
         c = getc(stream);
         while (isalnum(c) || c == '-' || c == '\'') {
            if (c == '-' || c == '\'') {
               d = c;
               
               c = getc(stream);
               if (isalnum(c)) {
                  flexarray_append(str, d);
                  flexarray_append(str, c);
                  c = getc(stream);
               } else {
                  /*reset word*/
                     flex_to_strn(str);
                     if (word_buff[0] != '\'' && word_buff[0] != '-' && word_buff[0] != ' ') {
                        word(word_buff);
                     }
                  flexarray_reset(str);
                  c = getc(stream);
               }
            } else if (isalnum(c)) {
               flexarray_append(str, c);
               c = getc(stream);
            }
         }
         /*completed a word*/
            flex_to_strn(str);
            if (word_buff[0] != '\'' && word_buff[0] != '-' && word_buff[0] != ' ') {
               word(word_buff);
            }
               
      }/*end of word*/

      /*reset word flexarray*/
      flexarray_reset(str);
      
      if (c != '<') {
         c = getc(stream);
      }
   }
   

   flexarray_delete(str);
   /*fprintf(stderr, "end parse\n");*/

}


