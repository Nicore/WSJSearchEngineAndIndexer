/**
 * @file main.c
 * @author Nick Comer
 * @date April 2012
 *
 * This is the main driver code for the parser and indexer.
 * It finds any files needed and passes open files to parse.c via parse(FILE *stream)
 * Main.c opens and closes the stream.
 *
 * Main.c also calls begin_ and end_indexing( ) as defined in index.c so the indexer
 * knows when to initialise data structures and when to erase and flush any buffered data.
 */
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "index.h"
#include "mylib.h"


int main(int argc, char **argv) {
   /*char const *wsj_colla = "/home/wsj477.xml";
     char const *wsj_collb = "~/wsj.xml";*/

   if (argc == 2) {
      
      char const *wsj_arg = argv[1];
   
      FILE *fwsj;
      fwsj = fopen(wsj_arg, "rb");
      if (fwsj == 0) {
         perror(wsj_arg);
         exit(EXIT_FAILURE);
      }
   

   
      /*so more like:*/
      begin_indexing();

      /*parse(FILE *stream);*/
      parse(fwsj);

   
   

      end_indexing();
   } else {
      fprintf(stderr, "Please place the collection file as an argument ie: ./parser <collection file location>");
   }
   return EXIT_SUCCESS;
}
