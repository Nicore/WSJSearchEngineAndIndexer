#ifndef FLEXARRAY_H_
#define FLEXARRAY_H_

typedef struct flexarrayrec *flexarray;

extern flexarray flexarray_new();
extern void flexarray_append(flexarray, int);
extern void flexarray_print(flexarray);
extern int flexarray_check(flexarray, int);
extern int flexarray_get(flexarray, int);
extern void flexarray_set(flexarray, int, int);
extern void flexarray_reset(flexarray);
extern int flexarray_size(flexarray);
extern int flexarray_sum(flexarray);
extern char *flexarray_to_word(flexarray);
extern void flexarray_sort(flexarray);
extern void flexarray_delete(flexarray);


#endif
