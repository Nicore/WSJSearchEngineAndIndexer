/*
 * index.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flexarray.h"
#include "index.h"
#include "mylib.h"
#include "tree.h"

typedef enum bool_e {FALSE, TRUE} bool_t;

FILE *ind; /*index of words*/
FILE *pst; /*postings and weights of words*/
FILE *dci; /*DOCNO index*/

int doc_count = 0; /*number of document currently being parsed (as id'd by <DOCNO>)*/
int docno = 0;

char const *ind_file_nm = "../indexes/index-word";
char const *pst_file_nm = "../indexes/index-post";
char const *dci_file_nm = "../indexes/index-docno";

tree t;

void write_indexes(char *s, flexarray dg, flexarray fq);
char big_buff[8192];


/*initialise data structures and open files and what not*/
void begin_indexing(void) {
   
   
   dci = fopen(dci_file_nm, "wb");
   if (dci == 0) {
      perror(dci_file_nm);
      exit(EXIT_FAILURE);
      }
   if (setvbuf(dci, big_buff, _IOFBF, sizeof big_buff) != 0) {
      fprintf(stderr, "Error assigning buffer");
      }

   
   t = tree_new(RBT);
   
}

/*write RBTree indexes to files and free everything up*/
void end_indexing(void) {
   /*dci = fopen(dci_file_nm, "wb");
   if (dci == 0) {
      perror(dci_file_nm);
      exit(EXIT_FAILURE);
      }*/
   
   ind = fopen(ind_file_nm, "wb");
   if (ind == 0) {
      perror(ind_file_nm);
      exit(EXIT_FAILURE);
   }

   pst = fopen(pst_file_nm, "wb");
   if (pst == 0) {
      perror(pst_file_nm);
      exit(EXIT_FAILURE);
   }

   
   tree_inorder_w(t, write_indexes);
   
   /* if (setvbuf(f_ind, big_buff, _IOFBF, sizeof big_buff) != 0) {
      
      }

    */

   /*close the docno index file*/
   if (fclose(dci) < 0) {
      perror("Closing index-docno");
      exit(EXIT_FAILURE);
   }
   if (fclose(ind) < 0) {
      perror("Closing index-word");
      exit(EXIT_FAILURE);
   }
   if (fclose(pst) < 0) {
      perror("Closing index-post");
      exit(EXIT_FAILURE);
   }
   tree_delete(t);
}


void start_tag(char const *name) {
   if (strcmp(name,"<DOC>") == 0) {
      doc_count++;
      if ((doc_count % 5000) == 0) {
         fprintf(stderr, "%i\n", doc_count);
      }
   }
   if (strcmp(name, "<DOCNO>") == 0) {
      docno = 1;
   }
   /*printf("%s - ST\n", name);*/
}

void end_tag(char const *name) {
   if (strcmp(name, "</DOCNO>") == 0) {
      docno = 0;
   }


   /*printf("%s - ET\n", name);*/
}


void word(char const *spelling) {
   if (docno == 1) {
      
      
      /*insert word into docno index*/
      fprintf(dci, "%s\n", spelling);
      /*fwrite(spelling, 1, strlen(spelling), dci);*/
      
   } else {
      /*insert word into rbt*/
      t =  tree_insert(t, spelling, doc_count);
      t = set_to_black(t);
      
   }

   /*printf("%s\n", spelling);*/
}


void write_indexes(char *s, flexarray dg, flexarray fq) {
   int ln = flexarray_size(dg);
   int i;
   int v1, v2;
   long where;
   
   
   where = ftell(pst);
   for (i = 0; i < ln; i++) {
      v1 = flexarray_get(dg, i);
      v2 = flexarray_get(fq, i);
      fprintf(pst, "%i:%i ", v1, v2);
   }
   fprintf(pst, "|\n");
   if (where != 0) {
      fprintf(ind, "%s %ld\n", s, where);
   }
}
