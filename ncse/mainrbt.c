/**
 * @file main.c
 * @author Nick Comer
 * @date September 2010
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "mylib.h"
#include "tree.h"

/* A boolean type which can be TRUE or FALSE */
typedef enum bool_e {FALSE, TRUE} bool_t;

/*Function declarations*/
static void usage(char *prog);
static void setup(int argc, char **argv, bool_t *rbt, bool_t *preorder,
                  bool_t *depth, bool_t *dot, char **file);

/**
 * Creates a tree and inserts words into it read from stdin.
 * Arguments on the command line alter the behaviour of the program
 * as follows:
 *  -   -r       Create a Red/Black tree instead of a BST (the default)
 *  -   -d       Print out the depth of the tree
 *  -   -p       Print the tree out in pre-order format
 *  -   -o FILE  Send the dot representation of the tree to a file
 *  -   -h       Display this message
 *
 * By default each word in the tree is printed out in in-order format to stdout.
 *
 * @param argc the number of command-line arguments.
 * @param argv an array of strings containing the command-line arguments.
 *
 * @return EXIT_SUCCESS if the program us successful.
 */
int main(int argc, char **argv) {
   bool_t rbt = FALSE, preorder = FALSE, depth = FALSE, dot = FALSE;
   FILE *out;
   char key[256];   
   tree t;
   char *file;

   setup(argc, argv, &rbt, &preorder, &depth, &dot, &file);

   t = tree_new((rbt) ? RBT : BST);

   while (getword(key, sizeof key, stdin) != EOF) {      
      t = tree_insert(t, key);
      t = set_to_black(t);
   }
   if (preorder) {
      tree_preorder(t, print_key);
   } else {   
      tree_inorder(t, print_key);
   }
   if (depth) {
      fprintf(stderr, "Depth: %d\n", tree_depth(t));
   }
   if (dot) {
      if (NULL == (out = fopen(file, "w"))) {
         fprintf(stderr, "%s: can't find file %s\n", argv[0], file);
         return EXIT_FAILURE;
      }
      
      tree_output_dot(t, out);
      fclose(out);
   }
   tree_delete(t);

   return EXIT_SUCCESS;
}

/**
 * Prints out a usage message to stderr outlining all of the options.
 * @param prog the name of the program to include in usage message.
 */
static void usage(char *prog) {
   fprintf(stderr, "Usage: %s [OPTION]... <STDIN>\n\n%s%s", prog,
          "Perform various operations using a tree.  By default read\n"
          "words from stdin and add them to a BST.\n\n"
          " -r       Use a Red/Black tree type instead of a BST\n"
          " -d       Print the depth of the tree\n",
          " -p       Print the tree with a preorder traversal\n"
          " -o FILE  Send the result of the dot method to a file\n\n"
          " -h       Display this message\n\n");
}

/**
 * Handle options given on the command-line by setting a number of
 * variables appropriately.  May call usage() if incorrect arguments
 * or -h given.
 *
 * @param argc the number of command-line arguments.
 * @param argv an array of strings contain the command-line arguments.
 * @param rbt set to TRUE if -r given
 * @param depth set to TRUE if -d given
 * @param preorder set to TRUE if -p given
 * @param dot set to TRUE if -o given
 * @param file set to FILE if -o FILE given
 */
static void setup(int argc, char **argv, bool_t *rbt, bool_t *preorder,
                  bool_t *depth, bool_t *dot, char **file) {
   const char *optstring = "rdpo:h";
   char option;

   while ((option = getopt(argc, argv, optstring)) != EOF) {
      switch (option) {
      case 'r':
         *rbt = TRUE;
         break;
      case 'd':
         *depth = TRUE;
         break;
      case 'p':
         *preorder = TRUE;
         break;
      case 'o':
         *dot = TRUE;
         *file = optarg;
         break;
      case 'h':
      default:
         usage(argv[0]);
      exit(EXIT_SUCCESS);
      }
   }
}
   
