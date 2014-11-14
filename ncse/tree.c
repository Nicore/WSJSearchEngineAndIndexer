/**
 * @file tree.c
 * @author Nick Comer
 * @date September 2010
 * It firstly defines a struct for the nodes of the tree and colour
 * enumerated types.
 * It then implements the methods:
 *  - tree_new:      creates a new tree.
 *  - tree_insert:   inserts a new element into the tree.
 *  - tree_search:   searches the tree for a given string.
 *  - tree_inorder:  prints out the contents of the tree in order.
 *  - tree_preorder: prints out the contents of the tree in a pre-order manner.
 *  - tree_delete:   deletes the tree and frees the memory.
 *  - right_rotate:  rotates the node and its two children to the right.
 *  - left_rotate:   rotates the node and its two children to the left.
 *  - tree_fix:      in the case the tree being an RBT this method fixes the
 *                   tree so its properties as an RBT remain intact.
 *  - tree_depth:    finds the greatest depth of the tree.
 *  - tree_output_dot: outputs the DOT representation of the tree to a file,
 *                     which can be turned into a pdf with an illustration of
 *                     the tree's structure.
 *  - tree_output_dot_aux: traverses the tree, writing a DOT description about
 *                         connections and possibly colours.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "flexarray.h"
#include "mylib.h"
#define IS_BLACK(x) (( NULL == (x)) || (BLACK == (x)->colour))
#define IS_RED(x) ((NULL != (x)) && (RED == (x)->colour))

typedef enum colour_e { RED, BLACK } colour_t;


struct treenode {
   char *key;
   colour_t colour;
   flexarray dgap;
   flexarray freqs;
   int diffsum;
   int flx_ln;
   tree left;
   tree right;
};

tree_t tree_type;

/**
 * Creates a new tree, initialising its data fields to NULL values and assigns
 * the type of tree to be used.
 *
 * @param var the type of tree to be used.
 *
 * @return tree the newly created tree.
 */
tree tree_new(tree_t var) {
   tree_type = var;
   /*dgap = flexarray_new();
     freqs = flexarray_new();*/
   return NULL;
}

/**
 * Sets a node in the tree to black. Used to set the root to black after
 * each insertion (called in main.c) to maintain that the root is always black.
 *
 * @param t the tree node to be coloured black
 *
 * @return tree the newly-black-coloured node.
 */
tree set_to_black(tree t) {
   t->colour = BLACK;
   return t;
}

/**
 * Inserts character strings into the tree as nodes.
 *
 * @param t the tree to add the string to.
 * @param s the string to add to the tree.
 * @param i the index line # of the docno.
 *
 * @return tree the tree plus the newly inserted node.
 */
tree tree_insert(tree t, const char *s, int i) {
   int dir = 0;
   int tmp = 0;
   /*int diffsum = 0;
   int flx_ln = 0;*/
   
   if (t == NULL) {
      t = emalloc(sizeof * t);
      t->right = t->left = NULL;
      t->key = emalloc(strlen(s)+1 * sizeof s[0]);
      strcpy(t->key, s);
      if (tree_type == RBT) {
         t->colour = RED;
      }
      /*add docno and init freq*/
      t->dgap = flexarray_new();
      t->freqs = flexarray_new();
      flexarray_append(t->dgap, i);
      flexarray_append(t->freqs, 1);
      t->diffsum = i;
      t->flx_ln = 1;

      
   } else {
      dir = strcmp(t->key, s);
      if (dir > 0) {
         t->left = tree_insert(t->left, s, i);
      } else if (dir < 0){
         t->right = tree_insert(t->right, s, i);
      } else if (dir == 0) {
         /*increment the freq at this doc*/
         /*diffsum = flexarray_sum(t->dgap);
         flx_ln = flexarray_size(t->freqs);
         if (diffsum == i) { *//*if still on the same document*//*
            flexarray_set(t->freqs, flx_ln-1, flexarray_get(t->freqs, flx_ln-1)+1);
            } else if (diffsum < i) { *//*on a new document now*//*
            flexarray_append(t->dgap, i - diffsum);
            flexarray_append(t->freqs, 1);
            }*/
         if (t->diffsum == i) { /*on same DOC*/
            tmp = flexarray_get(t->freqs, t->flx_ln-1);
            flexarray_set(t->freqs, t->flx_ln-1, tmp+1);
            /*flexarray_set(t->freqs, t->flx_ln-1, flexarray_get(t->freqs, t->flx_ln-1)+1);*/
         } else if (t->diffsum < i) { /*on new DOC*/
            flexarray_append(t->dgap, i - t->diffsum);
            flexarray_append(t->freqs, 1);
            t->diffsum = i;
            t->flx_ln++;
         }

      }

         
      }
   
   if (tree_type == RBT) {      
      t = tree_fix(t);
   }
   return t;
}

/**
 * Searches the tree for a specific character string.
 *
 * @param t the tree to search.
 * @param key the string to search for in the tree.
 *
 * @return int either 0 if the string isn't found or 1 if it is.
 */
int tree_search(tree t, char *key) {

   if ((t == NULL)) {
      return 0;
   } else if ((strcmp(t->key, key)) == 0) {
      return 1;
   } else if ((strcmp(t->key, key)) > 0) {
      return tree_search(t->left, key);
   } else {
      return tree_search(t->right, key);
   }

}

/**
 * 
 *
 */
void tree_inorder_w(tree t, void f(char *s, flexarray dg, flexarray fq)) {
   if (t == NULL) {
      return;
   } else {
      tree_inorder_w(t->left, f);

      f(t->key, t->dgap, t->freqs);

      tree_inorder_w(t->right, f);
   }

}



/**
 * Traverses the tree and prints out the nodes in order.
 *
 * @param t the tree to traverse.
 * @param f the print function to be used when printing out the nodes.
 */
void tree_inorder(tree t, void f(char *s, char *c)) {
   if (t == NULL) {
      return;
   } else {
      tree_inorder(t->left, f);
      /* Note: may not be required to print colour */
      if (tree_type == RBT) {
         f(t->key, IS_RED(t)?"red:":"black:");
      } else {
         f(t->key, NULL);
      }
      tree_inorder(t->right, f);
   }
}

/**
 * Traverses the tree in a pre-order manner, printing out the nodes as it goes.
 *
 * @param t the tree to traverse.
 * @param f the print function to be used when printing out the nodes.
 */
void tree_preorder(tree t, void f(char *s, char *c)) {
   if (t == NULL) {
      return;
   } else {
      if (tree_type == RBT) {         
         f(t->key, IS_RED(t)?"red:":"black:");
      } else {
         f(t->key, NULL);
      }
      tree_preorder(t->left, f);
      tree_preorder(t->right, f);
   }
}

/**
 * Traverses the tree in a post-order manner, erasing and freeing the tree and
 * its memory as it goes.
 *
 * @param t the tree to delete
 *
 * @return tree the NULL value that used to have a tree in its place.
 */
tree tree_delete(tree t) {
   if (t == NULL) {
      return NULL;
   } else {
      tree_delete(t->left);
      tree_delete(t->right);
      flexarray_delete(t->freqs);
      flexarray_delete(t->dgap);
      free(t->key);
      free(t);
      t = NULL;
      return t;
   }
}

/**
 * Rotates the node in the parameters and its two children to the right.
 *
 * @param t the node about which to rotate.
 *
 * @return tree the tree that results from the rotation.
 */
static tree right_rotate(tree t) {
   tree temp = t;
   t = t->left;
   temp->left = t->right;
   t->right = temp;
   return t;   
}

/**
 * Rotates the node in the parameters and its two children to the left.
 *
 * @param t the node about which to rotate.
 *
 * @return tree the tree that results from the rotation.
 */
static tree left_rotate(tree t) {
   tree temp = t;
   t = t->right;
   temp->right = t->left;
   t->left = temp;
   return t;
}

/**
 * Handles all the possible cases of the RBT that require fixing to maintain
 * the properties of an RBT, in the case that the tree type used is an RBT.
 *
 * @param t the tree to fix up.
 *
 * @return tree the fixed up tree.
 */
tree tree_fix(tree  t) {
   if (( t == NULL)) {
      return t;
   } else if (IS_RED(t->left)) { /*the left child is red*/
      if (IS_RED(t->left->left)/*case1*/) {
         if (IS_RED(t->right)) { /*uncle is red*/
            t->colour = RED;
            t->left->colour = BLACK;
            t->right->colour = BLACK;
         } else if (IS_BLACK(t->right)) { /*uncle is black*/
            t = right_rotate(t);
            t->colour = BLACK;
            t->right->colour = RED;
         }
      } else if (IS_RED(t->left->right)) { /*case2*/
         if (IS_RED(t->right)) { /*uncle is red*/
            t->colour = RED;
            t->left->colour = BLACK;
            t->right->colour = BLACK;
         } else if (IS_BLACK(t->right)) { /*uncle is black */
            t->left = left_rotate(t->left);
            t = right_rotate(t);
            t->colour = BLACK;
            t->right->colour = RED;
         }
      }
   } else if (IS_RED(t->right)) { /*the right child is red*/
      if (IS_RED(t->right->left)) { /*case3*/
         if (IS_RED(t->left)) { /*uncle is red*/
            t->colour = RED;
            t->left->colour = BLACK;
            t->right->colour = BLACK;
         } else if (IS_BLACK(t->left)){ /*uncle is black*/
            t->right = right_rotate(t->right);
            t = left_rotate(t);
            t->colour = BLACK;
            t->left->colour = RED;
         }
      } else if(IS_RED(t->right->right)) { /*case4*/
         if (IS_RED(t->left)) { /*uncle is red*/
            t->colour = RED;
            t->left->colour = BLACK;
            t->right->colour = BLACK;
         } else if (IS_BLACK(t->left)) { /*uncle is black*/
            t = left_rotate(t);
            t->colour = BLACK;
            t->left->colour = RED;
         }
      }
   }   
   return t;
}

/**
 * Finds out the maximum depth of the tree.
 *
 * @param t the tree to determine the depth of.
 *
 * @return int the depth of the tree.
 */
int tree_depth(tree t) {
   int ldepth = 0, rdepth = 0;
   if (t == NULL) {
      return -1;
   } else {
      ldepth += tree_depth(t->left);
      rdepth += tree_depth(t->right);

      if (ldepth > rdepth) {
         return (ldepth+1);
      } else {
         return (rdepth+1);
      }
   }      
}

/**
 * Traverses the tree writing a DOT description about connections, and
 * possibly colours, to the given output stream.
 *
 * @param t the tree to output a DOT description of.
 * @param out the stream to write the DOT output to.
 */
static void tree_output_dot_aux(tree t, FILE *out) {
   if (t->key != NULL) {
      fprintf(out, "\"%s\"[label=\"{<f0>%s|{<f1>|<f2>}}\"color=%s];\n", t->key,
              t->key, (RBT == tree_type && RED == t->colour) ? "red":"black");
   }
   if (t->left != NULL) {
      tree_output_dot_aux(t->left, out);
      fprintf(out, "\"%s\":f1 -> \"%s\":f0;\n", t->key, t->left->key);
   }
   if (t->right != NULL) {
      tree_output_dot_aux(t->right, out);
      fprintf(out, "\"%s\":f2 -> \"%s\":f0;\n", t->key, t->right->key);
   }
}


/**
 * Output a DOT description of this tree to the given output stream.
 * DOT is a plain text graph description language (have a look at
 * http://www.graphviz.org if you want more info).  You can create a
 * viewable image with a command like
 *
 *    dot -Tpdf < dotfile > dotfile.pdf
 *
 * where 'dotfile' is a file that has been written by tree_output_dot()
 *
 * You can also use png, ps, jpg, svg... instead of pdf
 *
 * @param t the tree to output the DOT description of.
 * @param out the stream to write the DOT description to.
 */
void tree_output_dot(tree t, FILE *out) {
   fprintf(out, "digraph tree {\nnode [shape = Mrecord, penwidth = 2];\n");
   tree_output_dot_aux(t, out);
   fprintf(out, "}\n");
}


