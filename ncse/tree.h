/* File: tree.h */

#ifndef TREE_H_
#define TREE_H_

#include "flexarray.h"
#include <stdio.h>

typedef struct treenode *tree;
typedef enum tree_e { BST, RBT } tree_t;

extern tree tree_new(tree_t type);
extern tree tree_insert(tree t, const char *s, int i);
extern int tree_search(tree t, char *key);
extern void tree_inorder_w(tree t, void f(char *s, flexarray dg, flexarray fq));
extern void tree_inorder(tree t, void f(char *s, char *c));
extern void tree_preorder(tree t, void f(char *s, char *c));
extern tree tree_delete(tree t);
extern tree tree_fix(tree t);
extern int tree_depth(tree t);
extern void tree_output_dot(tree t, FILE *out);
extern tree set_to_black(tree t);


#endif
