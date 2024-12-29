#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "greatest/greatest.h"

#define RED_BLACK_TREE_NAME red_black_tree_uint32
#define RED_BLACK_TREE_KEY_TYPE uint32_t
#define RED_BLACK_TREE_VALUE_TYPE char *
#include "red_black_tree.h"
#undef RED_BLACK_TREE_NAME
#undef RED_BLACK_TREE_KEY_TYPE
#undef RED_BLACK_TREE_VALUE_TYPE

TEST test_red_black_tree(void) {
    red_black_tree_uint32 *tree = red_black_tree_uint32_new();

    red_black_tree_uint32_insert(tree, 7, "a");
    red_black_tree_uint32_insert(tree, 18, "b");
    red_black_tree_uint32_insert(tree, 3, "c");
    red_black_tree_uint32_insert(tree, 26, "d");
    red_black_tree_uint32_insert(tree, 22, "e");
    red_black_tree_uint32_insert(tree, 11, "f");
    red_black_tree_uint32_insert(tree, 8, "g");
    red_black_tree_uint32_insert(tree, 10, "h");
    red_black_tree_uint32_insert(tree, 15, "i");

    char *a = red_black_tree_uint32_get(tree->root, 7);
    ASSERT_STR_EQ(a, "a");

    char *b = red_black_tree_uint32_get(tree->root, 18);
    ASSERT_STR_EQ(b, "b");

    char *c = red_black_tree_uint32_get(tree->root, 3);
    ASSERT_STR_EQ(c, "c");

    char *d = red_black_tree_uint32_get(tree->root, 26);
    ASSERT_STR_EQ(d, "d");

    char *e = red_black_tree_uint32_get(tree->root, 22);
    ASSERT_STR_EQ(e, "e");

    char *f = red_black_tree_uint32_get(tree->root, 11);
    ASSERT_STR_EQ(f, "f");

    char *g = red_black_tree_uint32_get(tree->root, 8);
    ASSERT_STR_EQ(g, "g");

    char *h = red_black_tree_uint32_get(tree->root, 10);
    ASSERT_STR_EQ(h, "h");

    char *i = red_black_tree_uint32_get(tree->root, 15);
    ASSERT_STR_EQ(i, "i");

    h = red_black_tree_uint32_delete(tree, 10);
    ASSERT_STR_EQ(h, "h");
    
    d = red_black_tree_uint32_delete(tree, 26);
    ASSERT_STR_EQ(d, "d");

    b = red_black_tree_uint32_delete(tree, 18);
    ASSERT_STR_EQ(b, "b");

    a = red_black_tree_uint32_delete(tree, 7);
    ASSERT_STR_EQ(a, "a");

    c = red_black_tree_uint32_delete(tree, 3);
    ASSERT_STR_EQ(c, "c");

    c = red_black_tree_uint32_get(tree->root, 3);
    ASSERT(c == NULL);

    red_black_tree_uint32_insert(tree, 3, "c");
    c = red_black_tree_uint32_get(tree->root, 3);
    ASSERT_STR_EQ(c, "c");

    c = red_black_tree_uint32_delete(tree, 3);
    ASSERT_STR_EQ(c, "c");

    c = red_black_tree_uint32_get(tree->root, 3);
    ASSERT(c == NULL);

    e = red_black_tree_uint32_delete(tree, 22);
    ASSERT_STR_EQ(e, "e");

    f = red_black_tree_uint32_delete(tree, 11);
    ASSERT_STR_EQ(f, "f");

    g = red_black_tree_uint32_delete(tree, 8);
    ASSERT_STR_EQ(g, "g");

    i = red_black_tree_uint32_delete(tree, 15);
    ASSERT_STR_EQ(i, "i");

    i = red_black_tree_uint32_get(tree->root, 15);
    ASSERT(i == NULL);

    ASSERT(tree->root->left == NULL);
    ASSERT(tree->root->right == NULL);

    red_black_tree_uint32_destroy(tree);
    PASS();
}



/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_red_black_tree);

    GREATEST_MAIN_END();        /* display results */
}