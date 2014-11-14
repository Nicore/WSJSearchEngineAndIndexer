/**
 * @file searcheng.c
 * @author Nick Comer
 * @date April 2012
 *
 * Reads a query from stdin and produces results on stdout
 * A query may be a set of _zero_ or more terms.
 * Document-level index is sufficient.
 * No wildcard or regex searching.
 * For each result (up to a limit), output a line with two columns:
 * <DOCNO> <rsv>
 * Where DOCNO is wsj document number and rsv is the relevance score.
 * Sorted from highest rsv to lowest rsv.
 * Tested on indexes but not for seen queries.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "flexarray.h"
#include "mylib.h"
#include <time.h>

#define CR 13
#define LF 10
#define EOF_MARKER 26
#define MAX_LINE_LEN 100 


typedef struct {
   char *word;
   long number;
} line;

typedef struct {
   char *journal;
} recd;

typedef struct {
   flexarray docs;
   flexarray freqs;
} postlist;

typedef struct {
   int docid;
   int freq;
} postpair;


char *wd_ind_def = "~/indexes/indexes-good/index-word";
char *dci_ind_def = "~/indexes/indexes-good/index-docno";
char *pst_ind_def = "~/indexes/indexes-good/index-post";

int binary_search(line *dictn, char *key, int min, int max);
int seekpost(FILE *input, long where);
int FileRead(FILE *inputF);
int DocNoRead(FILE *input);

int addtoLine(line item);
int addtoRecd(recd docn);
int checkdoubles(int c);
int addtoPair(postpair item);
int addtoResult(postpair item);
void quicksort(postpair *a, int n, int t);
/*int t as boolean int for whether to sort by docid or freq*/
void insertion_sort(postpair *a, int n, int t);
void merge_sort(postpair *a, postpair *w, int n, int t);
void merge(postpair *a, postpair *w, int n, int t);

line *dictionary = NULL;
int num_es_dict = 0;
int num_al_dict = 0;

recd *docdex = NULL;
int num_es_docd = 0;
int num_al_docd = 0;
void *_tmp;
void *_tmp2;

int num_es_post = 0;
int num_al_post = 0;
int num_es_res = 0;
int num_al_res = 0;
flexarray dict_indexes;


postpair *postarray;
postpair *workspace;
postpair *resultarray;

int main(int argc, char **argv) {

   /*time_t t1, t2;*/
   FILE *dict;
   FILE *post;
   FILE *docno;
   char *wd_ind;
   char *dci_ind;
   char *pst_ind;
   int match_ct;  
   int docpos, docct;
   int result;
   int i = 0;
   int j = 0;
   char w[256][256];
   postpair ph;
   clock_t start, end; /*timing vars */

   start = clock(); /* now featuring timing! */
   
   if (argc == 4) { /*if the index locations are specified*/
      dci_ind = argv[1];
      wd_ind = argv[2];
      pst_ind = argv[3];
   } else { /*use default indexes*/
      dci_ind = dci_ind_def;
      wd_ind = wd_ind_def;
      pst_ind = pst_ind_def;
      fprintf(stderr, "Using default indexes.\n");
   }
   
   fprintf(stderr, "Enter a line of query terms:\n");
   
   /*w = emalloc(wd_ct_cap * sizeof w[0][0]);*/
   
   while (scanf("%s", w[i]) != EOF ) {
      i++;
   }

   /*for (j = 0; j < i; j++) {
      printf("%s ", w[j]);
      }*/

   if (i == 0) {
      fprintf(stdout, "No search terms inputted");
   } else { /*actually search*/
      /*show search terms*/
      fprintf(stderr, "Results for query:");
      for (j = 0; j < i; j++) {
         fprintf(stderr, " %s", w[j]);
      }
      fprintf(stderr, "\n");

      dict = fopen(wd_ind, "rb");
      if (dict == 0) {
         perror(wd_ind);
         exit(EXIT_FAILURE);
      }
      /*read in the dictionary and then close the file*/
      result = FileRead(dict);
      if (fclose(dict) < 0) {
         perror("Closing dictionary");
         exit(EXIT_FAILURE);
      }

      /*read in the docno list*/
      docno = fopen(dci_ind, "rb");
      if (docno == 0) {
         perror(dci_ind);
         exit(EXIT_FAILURE);
      }
      
      result = DocNoRead(docno);
      if (fclose(docno) < 0) {
         perror("Closing docno index");
         exit(EXIT_FAILURE);
      }
      
      
      /*look up the search terms in the dictionary array*/
      dict_indexes = flexarray_new();
      for (j = 0; j < i; j++) {
      	result = binary_search(dictionary, w[j], 0, num_es_dict-1);
      	if (result >= 0) {
         flexarray_append(dict_indexes, result);
         }
      }
      
      
	match_ct = flexarray_size(dict_indexes);
	fprintf(stderr, "Collection has %i of %i search terms.\n", match_ct, i);
      
      /*now seek the postings list from the new file.*/
      
      if (match_ct > 0) { 
		  post = fopen(pst_ind, "rb");
		  if (post == 0) {
		     perror(pst_ind);
		     exit(EXIT_FAILURE);
		  }
		  for (j = 0; j < match_ct; j++) {
		  
		  	result = seekpost(post, dictionary[flexarray_get(dict_indexes, j)].number);
		  }
		  
		  if (fclose(post) < 0) {
		     perror("Closing postings list");
		     exit(EXIT_FAILURE);
		  }


		  
		  /*for (j = 0; j < num_es_post && j <15; j++) {
		  	fprintf(stdout, "%s %i %i\n", docdex[postarray[j].docid].journal, postarray[j].docid, postarray[j].freq);
	   	  }		*/
	   	  
	   	  /*now do relevance ranking*/
	   	  	  
	   	  _tmp = erealloc(workspace, (num_al_post * sizeof(postpair)));
	   	  workspace = (postpair*)_tmp;
	   	  
	   	  merge_sort(postarray, workspace, num_es_post, 1);
	   	  /*quicksort(postarray, num_es_post, 1);*/
	   	  
	   	  
	   	  /*fprintf(stderr, "----\n");
	   	  for (j = 0; j < num_es_post && j < 15; j++) {
		  	fprintf(stdout, "%s %i %i\n", docdex[postarray[j].docid].journal, postarray[j].docid, postarray[j].freq);
	   	  }		*/
	   	  
	   	  /*for (j = num_es_post-1; j >= num_es_post-11 && j >= 0; j--) {
		  	fprintf(stdout, "%s %i %i\n", docdex[postarray[j].docid].journal, postarray[j].docid, postarray[j].freq);
	   	  }	*/
	   	  
	   	  /*remove duplicates*/
	   	  
	   	  
	   	  docpos = 0;
	   	  docct = 0;
	   	  i = 0;
	   	  /*resultarray[0].docid = postarray[0].docid;
	   	  resultarray[0].freq = postarray[0].freq;
	   	  for (j = 0; j < num_es_post; j++) { 
	   	  	 if (resultarray[i].docid == postarray[j].docid) {
	   	  	 	if (j!= 0) {
	   	  	 		resultarray[i].freq += postarray[j].freq;		
	   	  	 	} else {
	   	  	 		resultarray[i].freq = postarray[j].freq;
	   	  	 	}
	   	  	 } else if (resultarray[i].docid != postarray[j].docid) { 
	   	  	 	
	   	  	 	resultarray[i].docid = postarray[j].docid;
	   	  	 	resultarray[i].freq = postarray[j].freq;
	   	  	 	i++;
	   	  	 	docct++;
	   	  	 }

	   	  }*/
	   	  
	   	  ph.docid = postarray[0].docid;
	   	  ph.freq = postarray[0].freq;
	   	  addtoResult(ph);
	   	  docpos = resultarray[0].docid;
	   	  docct = 1;
	   	  i = 0;
	   	  for (j = 1; j < num_es_post; j++) {
	   	  	if (resultarray[i].docid == postarray[j].docid) {
	   	  		resultarray[i].freq += postarray[j].freq;
	   	  	} else if (resultarray[i].docid != postarray[j].docid) {
	   	  		ph.docid = postarray[j].docid;
	   	  		ph.freq = postarray[j].freq;
	   	  		addtoResult(ph);
	   	  		i++;
	   	  		docct++;
	   	  	}
	   	  	
	   	  		
	   	  	
	   	  }
	   	  
	   	  
	   	  /*fprintf(stderr, "docct %i\n", docct);
	   	  for (j = 0; j < docct ; j++) {
		  	fprintf(stdout, "%s %i %i\n", docdex[workspace[j].docid].journal, workspace[j].docid, workspace[j].freq);
	   	  }	*/
	   	  
	   	  /*for (j = docct-1; j >= docct-16 && j >= 0; j--) {
		  	fprintf(stdout, "%s %i %i\n", docdex[resultarray[j].docid].journal,  resultarray[j].docid, resultarray[j].freq);
	   	  }	*/
	   	  
	   	  /*quicksort(resultarray, docct, 0);*/
	   	  merge_sort(resultarray, workspace, docct, 0);
	   	  
	   	  
	   	  end = clock();
	   	  fprintf(stderr, "Time: %f\n",(end - start) / (double)CLOCKS_PER_SEC);
	   	  
	   	  /* Print out results */
	   	  for (j = docct-1;  j >= 0; j--) {
		  	fprintf(stdout, "%s %i\n", docdex[resultarray[j].docid].journal,  resultarray[j].freq);
	   	  }	
	   	  	
	   	  
	   	  free(_tmp);
	   	  free(_tmp2);

      } else {
      	fprintf(stderr, "No results found\n");
      }

	for (i = num_es_docd-1; i >= 0; i--) {
      		free(docdex[i].journal);
   			}
   		free(docdex);
   for (i = num_es_dict-1; i >= 0; i--) {
      free(dictionary[i].word);
   }
   free(dictionary);
   
   /*free(_tmp);*/
   


   
   flexarray_delete(dict_indexes);

	}
   
   return EXIT_SUCCESS;

}

void quicksort(postpair *a, int n, int t) {
   int i, j;
   int pivot;
   int ph1, ph2;
   
   if (t == 0) {	
   if (n > 1) {
      pivot = a[0].freq;
      i = -1;
      j = n;
      for (;;) {      
         do {i++;} while (a[i].freq < pivot);
         do {j--;} while (a[j].freq > pivot);
         if (i < j) {
            ph1 = a[i].freq;
            ph2 = a[i].docid;
            a[i].freq = a[j].freq;
            a[i].docid = a[j].docid;
            a[j].freq = ph1;
            a[j].docid = ph2;
         } else {
            break;
         }
         
      }
   
   quicksort(a, j+1,t);
   quicksort(a+(j+1),n-j-1,t); 
   }
   } else if (t == 1) {
   if (n > 1) {
      pivot = a[0].docid;
      i = -1;
      j = n;
      for (;;) {      
         do {i++;} while (a[i].docid < pivot);
         do {j--;} while (a[j].docid > pivot);
         if (i < j) {
            ph1 = a[i].freq;
            ph2 = a[i].docid;
            a[i].freq = a[j].freq;
            a[i].docid = a[j].docid;
            a[j].freq = ph1;
            a[j].docid = ph2;
         } else {
            break;
         }
         
      }
   
   quicksort(a, j+1,t);
   quicksort(a+(j+1),n-j-1,t); 
   }
   
   }
   
}

/**
 * The merge/insertion sorts are throwing some ridiculous number of valgrind 
 * errors due to how they're returning a struct.
 * insertion sort takes in an array 'a' and int 'n' 
 *  then sorts the 'n' integers in the array.
 */
void insertion_sort(postpair *a, int n, int t) { 
   int p, j; 
   postpair ph; 
   
  
   if (t == 1) {
	   if (n > 1) {
		  for (p = 1; p < n; p++) { 
		     ph.docid = a[p].docid;
		     ph.freq = a[p].freq;
		     j = p-1;
		     while ((ph.docid < a[j].docid) && (j >= 0)) {
		        a[j+1].docid = a[j].docid;
		        a[j+1].freq = a[j].freq;
		        j--;
		     }
		     a[j+1].docid = ph.docid;
		     a[j+1].freq = ph.freq;
		  }
	   }
   } else if (t == 0) {
   	   if (n > 1) {
		  for (p = 1; p < n; p++) { 
		     ph.docid = a[p].docid;
		     ph.freq = a[p].freq;
		     j = p-1;
		     while ((ph.freq < a[j].freq) && (j >= 0)) {
		        a[j+1].docid = a[j].docid;
		        a[j+1].freq = a[j].freq;
		        j--;
		     }
		     a[j+1].docid = ph.docid;
		     a[j+1].freq = ph.freq;
		  }
	   }
   }
}


void merge_sort(postpair *a, postpair *w, int n, int t) {
   int i;
   if (n < 2) {
      return;
   } else if (n < 45) {
      insertion_sort(a, n, t);
   } else {

      merge_sort(a, w, (n/2), t);
      merge_sort((a+(n/2)), w, (n - (n/2)), t);

      merge(a, w, n, t);

      for (i = 0; i < n; i++) {
         a[i].docid = w[i].docid;
         a[i].freq = w[i].freq;
      }
   }
}


void merge(postpair *a, postpair *w, int n, int t) {
   int i, j; 
   int k = 0; 

   i = 0;
   j = n/2;
   

   if (t == 1)	{
	   while ((i < (n/2)) && (j < n)) {
		  if (a[i].docid < a[j].docid) {
		     w[k].docid = a[i].docid;
		     w[k].freq = a[i].freq;
		     i++;
		  } else if (a[i].docid >= a[j].docid) {
		     w[k].docid = a[j].docid;
		     w[k].freq = a[j].freq;
		     j++;
		  }
		  k++;
	   }
	   while (i < n/2) {
		  w[k].docid = a[i].docid;
		  w[k].freq = a[i].freq;
		  i++;
		  k++;
	   }
	   while (j < n) {
		  w[k].docid = a[j].docid;
		  w[k].freq = a[j].freq;
		  j++;
		  k++;
	   }
   } else if (t == 0) {
   	   while ((i < (n/2)) && (j < n)) {
		  if (a[i].freq < a[j].freq) {
		     w[k].docid = a[i].docid;
		     w[k].freq = a[i].freq;
		     i++;
		  } else if (a[i].freq >= a[j].freq) {
		     w[k].docid = a[j].docid;
		     w[k].freq = a[j].freq;
		     j++;
		  }
		  k++;
	   }
	   while (i < n/2) {
		  w[k].docid = a[i].docid;
		  w[k].freq = a[i].freq;
		  i++;
		  k++;
	   }
	   while (j < n) {
		  w[k].docid = a[j].docid;
		  w[k].freq = a[j].freq;
		  j++;
		  k++;
	   }
	}
   
}


int binary_search(line *dictn, char *key, int min, int max) {
   int mid;
   int dir;
   
   if (max < min) {
      return -1;
   } else {
      mid = (min + max) /2;
      /*fprintf(stderr, "mid:%i\n", mid);*/
      dir = strcmp(dictn[mid].word, key);
		/*fprintf(stderr, "test\n");*/
      if (dir > 0) {
         return binary_search(dictn, key, min, mid-1);
      } else if (dir < 0) {
         return binary_search(dictn, key, mid+1, max);
      } else {
         return mid;
      }
   }


}

int checkdoubles(int c) {
   int i;
   
   for (i = 0; i < num_es_post; i++) {
      if (c == postarray[i].docid) {
         return i;
      }
   }
   return -1;

}

int seekpost(FILE *posts, long where) {
   int swap = 2;

   char strint[20];
   int c;
   int tempint;
   int i = 0;
   int docnumber = 0;
   postpair temppair;
   
   /*fprintf(stderr, "Seeking: %ld\n", where);*/
	tempint = 0;   
   
   fseek(posts, where, SEEK_SET);
   
   /*scan through line until '|', first int is doc, second is freq*/
   c = getc(posts);
   
   while (c != '|') { /*while we haven't hit the end of the line*/
      /*i = 0;*/
      
      if (isdigit(c)) {
         strint[i] = c;
         i++;
      } else if (c == ':' || c == ' ') {
         
         if ((swap % 2) == 0) {
            strint[i] = '\0';
            
            tempint = (int) strtol(strint, NULL, 0);
            docnumber += tempint;
            
            temppair.docid = docnumber-1;
               
            
            
            i = 0;
         } else {
            strint[i] = '\0';
            
            tempint = (int)strtol(strint, NULL, 0);
            temppair.freq = tempint;
            if (addtoPair(temppair) == -1) {
                fprintf(stderr, "Failed addtoPair\n");
            }
            
            
            i = 0;
            
         }   
         swap++;
         
         
         }
      c = getc(posts);
  			
  	}
   
	rewind(posts);
   
   return 1;
}

int FileRead(FILE *dict) {
   int newLine;
   long FileLen;
   long ind;
   long lnCt;
   long lnLen;
   int spc = 0;
   char *wordrec;
   char whitespace;
   char *seekrec;
   long seekval;
   long stPos;
   long tChars;
   char cLine[MAX_LINE_LEN]; /*the current line*/
   char *cFile;
   char *cPtr;

   line temp;
   
   fseek(dict, 0L, SEEK_END);
   FileLen = ftell(dict);
   rewind(dict);

   cFile = calloc(FileLen +1, sizeof(char));

   if (cFile == NULL) {
      fprintf(stderr, "Insufficient memory to read file\n");
      return 0;
   }

   fread(cFile, FileLen, 1, dict); /*read entire file into cFile*/

   lnCt = 0L;
   tChars = 0L;

   cPtr = cFile; /*point to beginning of array*/

   while (*cPtr) { /*read until null char*/
      ind = 0L; /*reset values*/
      newLine = 0;
      stPos = tChars;

      
      
      while (*cPtr) { /*read until null char*/
                             
         if (!newLine) { 
            if (*cPtr == CR || *cPtr == LF) { /*if we hit newline char*/
               newLine = 1;
            } else {
               cLine[ind++] = *cPtr;
               
               /*cLine[ind++] = *cPtr;*/
            }
         } else if (*cPtr != CR && *cPtr != LF) { /*already found newline*/
            break;
         }
         /*cLine[ind++] = *cPtr++;*/ /*add char to output and increment*/
         whitespace = *cPtr;
         if (whitespace == ' ') {
            spc = ind;
         }

         cPtr++;
         tChars++;
         
      } /*end while *cPtr*/
      cLine[ind] = '\0'; /*end string*/
      lnCt++;
      lnLen = strlen(cLine);

      wordrec = emalloc((spc) *sizeof wordrec[0]);
      seekrec = emalloc((lnLen-spc+1) *sizeof seekrec[0]);
      strncpy(wordrec, cLine, spc-1);
      wordrec[spc-1] = '\0';
      strncpy(seekrec, (cLine+(spc)), ((ind+1)-spc));
      
      seekval = strtol(seekrec, NULL, 0);
      /*PrintLine(cLine, lnCt, lnLen, stPos, NULL, 0);*/
      temp.word = emalloc((strlen(wordrec)+1) * sizeof wordrec[0]);
      strncpy(temp.word, wordrec, strlen(wordrec)+1);
      temp.number = seekval;
      if (addtoLine(temp) == -1) {
         fprintf(stderr, "Failed to add line\n");
      }
      
      /*printf("%s: %s, %ld, %i\n", cLine, wordrec, seekval, spc);*/
      free(wordrec);
      free(seekrec);
      
   } /*end of while we haven't hit the end of file */
   /*free(temp.word);*/
   
   free(cFile);
   cFile = NULL;
   return 1;
}

int DocNoRead(FILE *docd) {
   int newLine;
   long FileLen;
   long ind;
   long lnCt;
   long lnLen;
   
   recd temprec;
   long stPos;
   long tChars;
   char cLine[MAX_LINE_LEN]; /*the current line*/
   char *cFile;
   char *cPtr;

   
   
   fseek(docd, 0L, SEEK_END);
   FileLen = ftell(docd);
   rewind(docd);

   cFile = calloc(FileLen +1, sizeof(char));

   if (cFile == NULL) {
      fprintf(stderr, "Insufficient memory to read file\n");
      return 0;
   }

   fread(cFile, FileLen, 1, docd); /*read entire file into cFile*/

   lnCt = 0L;
   tChars = 0L;

   cPtr = cFile; /*point to beginning of array*/

   while (*cPtr) { /*read until null char*/
      ind = 0L; /*reset values*/
      newLine = 0;
      stPos = tChars;

      
      
      while (*cPtr) { /*read until null char*/
                             
         if (!newLine) { 
            if (*cPtr == CR || *cPtr == LF) { /*if we hit newline char*/
               newLine = 1;
            } else {
               cLine[ind++] = *cPtr;
               
               /*cLine[ind++] = *cPtr;*/
            }
         } else if (*cPtr != CR && *cPtr != LF) { /*already found newline*/
            break;
         }
         
         cPtr++;
         tChars++;
         
      } /*end while *cPtr*/
      cLine[ind] = '\0'; /*end string*/
      lnCt++;
      lnLen = strlen(cLine);

      /*PrintLine(cLine, lnCt, lnLen, stPos, NULL, 0);*/
      temprec.journal = emalloc((strlen(cLine)+1) * sizeof cLine[0]);
      strncpy(temprec.journal, cLine, strlen(cLine)+1);
      
      if (addtoRecd(temprec) == -1) {
         fprintf(stderr, "Failed to add line\n");
      }
      
      /*printf("%s: %s, %ld, %i\n", cLine, wordrec, seekval, spc);*/
      
   } /*end of while we haven't hit the end of file */
   
   free(cFile);
   return 1;
}

   
int addtoLine (line item) {
   if (num_es_dict == num_al_dict) {
      if (num_al_dict == 0) {
         num_al_dict = 3;
      } else {
         num_al_dict *= 2;
      }

      _tmp = erealloc(dictionary, (num_al_dict *sizeof(line)));

      if (!_tmp) {
         fprintf(stderr, "Couldn't erealloc mem\n");

         return (-1);
      }

      dictionary = (line*)_tmp;
   }

   dictionary[num_es_dict] = item;
   num_es_dict++;
	
   return num_es_dict;
}

int addtoPair(postpair item) {
	
   if (num_es_post == num_al_post) {
      if (num_al_post == 0) {
         num_al_post = 3;
      } else {
         num_al_post *= 2;
      }
      _tmp = erealloc(postarray, (num_al_post * sizeof(postpair)));

      if (!_tmp) {
         fprintf(stderr, "Couldn't erealloc mem to postarray\n");
         return -1;
      }
      postarray = (postpair*)_tmp;
   }
   postarray[num_es_post] = item;
   num_es_post++;
	
   return num_es_post;
   
}

int addtoResult(postpair item) {
	
   if (num_es_res == num_al_res) {
      if (num_al_res == 0) {
         num_al_res = 3;
      } else {
         num_al_res *= 2;
      }
      _tmp = erealloc(resultarray, (num_al_res * sizeof(postpair)));

      if (!_tmp) {
         fprintf(stderr, "Couldn't erealloc mem to postarray\n");
         return -1;
      }
      resultarray = (postpair*)_tmp;
   }
   resultarray[num_es_res] = item;
   num_es_res++;
	
   return num_es_res;
   
}

int addtoRecd (recd item) {
   if (num_es_docd == num_al_docd) {
      if (num_al_docd == 0) {
         num_al_docd = 3;
      } else {
         num_al_docd *= 2;
      }

      _tmp = erealloc(docdex, (num_al_docd *sizeof(recd)));

      if (!_tmp) {
         fprintf(stderr, "Couldn't erealloc mem\n");

         return (-1);
      }

      docdex = (recd*)_tmp;
   }

   docdex[num_es_docd] = item;
   num_es_docd++;
	
   return num_es_docd;   
}
