/*flexarray.c*/
#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "flexarray.h"
#include <ctype.h>

struct flexarrayrec {
   int capacity;
   int itemcount;
   int *items;
};

flexarray flexarray_new() {
   flexarray result = emalloc(sizeof *result);
   result->capacity = 2;
   result->itemcount = 0;
   result->items = emalloc(result->capacity * sizeof result->items[0]);
   return result;
}

void flexarray_append(flexarray f, int num) {
   if (f->itemcount == f->capacity) {
      f->capacity += (f->capacity/2);
      f->items = erealloc(f->items, f->capacity * sizeof f->items[0]);
   }
   f->items[f->itemcount++] = num;
}

void flexarray_print(flexarray f) {
   int i;
   for (i = 0; i < f->itemcount; i++) {
      fprintf(stdout, "%d\n", f->items[i]); 
   }
}

int flexarray_check(flexarray f, int c) {
	int i;
	for (i = 0; i < f->itemcount; i++) {
      if (c == f->items[i]) {
      	return i;
      } 
   }
   return -1;
}

int flexarray_get(flexarray f, int n) {
   int c;
   if (n < f->itemcount) {
      c = f->items[n];
   } else {
      c = -1;
   }
   return c;
}

void flexarray_set(flexarray f, int i, int n) {
   if (i < f->itemcount) {
      f->items[i] = n;
   }
}

void flexarray_reset(flexarray f) {
   f->itemcount = 0;
}

int flexarray_size(flexarray f) {
   return f->itemcount;
}

int flexarray_sum(flexarray f) {
   int i = 0;
   int sum = 0;
   for (i = 0; i < f->itemcount; i++) {
      sum += f->items[i];
   }
   return sum;
}

char * flexarray_to_word(flexarray f) {
   int i;
   /*char word[f->itemcount] = {0};*/
   char tmp;
   char *word = emalloc((f->itemcount+1) * sizeof tmp);
   tmp = 'a';
   
   for (i = 0; i < f->itemcount; i++) {
      word[i] = f->items[i];
   }
   word[f->itemcount] = '\0';
   return word;
}

static void quick_sort(int *a, int n) {
   int i, j;
   int pivot;
   int ph;

   if (n > 1) {
      pivot = a[0];
      i = -1;
      j = n;
      fprintf(stderr, "i");
      for (;;) {
         do {i++;} while (a[i] < pivot);
         do {j--;} while (a[j] > pivot);
         if (i < j) {
            ph = a[i];
            a[i] = a[j];
            a[j] = ph;
         } else {
            break;
         }
      }
   
   fprintf(stderr, "x");
   quick_sort(a, j+1);
   quick_sort(a+(j+1),n-j-1); 
   }
}



void flexarray_sort(flexarray f) { /*hah insertion sort cos its easy*/
   /*int p, j, key;
   int n = (*f).itemcount;

   if (n > 1) {
      for (p = 1; p < n; p++) {
         key = (*f).items[p];
         j = p-1;
         while ((key < (*f).items[j]) && (j >= 0)) {
            (*f).items[j+1] = (*f).items[j];
            j--;
         }
         (*f).items[j+1] = key;
      }
      }*/
  
   quick_sort(f->items, f->itemcount);
   
   
}

void flexarray_delete(flexarray f) {
   free(f->items);
   free(f);
   f = NULL;
}

         
