#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#endif // RED_BLACK_TREE_H

#ifndef RED_BLACK_TREE_NAME
#error "Must define RED_BLACK_TREE_NAME"
#endif

#ifndef RED_BLACK_TREE_KEY_TYPE
#error "Must define RED_BLACK_TREE_TYPE"
#endif

#ifndef RED_BLACK_TREE_VALUE_TYPE
#error "Must define RED_BLACK_TREE_VALUE_TYPE"
#endif

#ifndef RED_BLACK_TREE_MAX_HEIGHT
#define RED_BLACK_TREE_MAX_HEIGHT 128
#endif


#define RED_BLACK_TREE_CONCAT_(a, b) a ## b
#define RED_BLACK_TREE_CONCAT(a, b) RED_BLACK_TREE_CONCAT_(a, b)
#define RED_BLACK_TREE_TYPED(name) RED_BLACK_TREE_CONCAT(RED_BLACK_TREE_NAME, _##name)
#define RED_BLACK_TREE_FUNC(func) RED_BLACK_TREE_CONCAT(RED_BLACK_TREE_NAME, _##func)

#define RED 0
#define BLACK 1

#define BST_NAME RED_BLACK_TREE_NAME
#define BST_KEY_TYPE RED_BLACK_TREE_KEY_TYPE
#define BST_VALUE_TYPE RED_BLACK_TREE_VALUE_TYPE
#define BST_NODE_EXTRA \
    uint8_t color:1;

#ifdef RED_BLACK_TREE_KEY_EQUALS
#define BST_KEY_EQUALS RED_BLACK_TREE_KEY_EQUALS
#endif

#ifdef RED_BLACK_TREE_KEY_LESS_THAN
#define BST_KEY_LESS_THAN RED_BLACK_TREE_KEY_LESS_THAN
#endif

#include "binary_tree/binary_tree.h"

#undef BST_NAME
#undef BST_KEY_TYPE
#undef BST_VALUE_TYPE
#undef BST_NODE_EXTRA
#ifndef RED_BLACK_TREE_KEY_EQUALS
#define RED_BLACK_TREE_KEY_EQUALS RED_BLACK_TREE_TYPED(key_equals)
#endif
#ifndef RED_BLACK_TREE_KEY_LESS_THAN
#define RED_BLACK_TREE_KEY_LESS_THAN RED_BLACK_TREE_TYPED(key_less_than)
#endif

#define RED_BLACK_TREE_NODE RED_BLACK_TREE_TYPED(node_t)

#define RED_BLACK_TREE_NODE_MEMORY_POOL_NAME RED_BLACK_TREE_TYPED(node_memory_pool)

#define MEMORY_POOL_NAME RED_BLACK_TREE_NODE_MEMORY_POOL_NAME
#define MEMORY_POOL_TYPE RED_BLACK_TREE_NODE
#include "memory_pool/memory_pool.h"
#undef MEMORY_POOL_NAME
#undef MEMORY_POOL_TYPE

#define RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(name) RED_BLACK_TREE_CONCAT(RED_BLACK_TREE_NODE_MEMORY_POOL_NAME, _##name)


typedef struct RED_BLACK_TREE_NAME {
    RED_BLACK_TREE_TYPED(node_t) *root;
    RED_BLACK_TREE_TYPED(node_memory_pool) *pool;
} RED_BLACK_TREE_NAME;

RED_BLACK_TREE_NAME *RED_BLACK_TREE_FUNC(new)(void) {
    RED_BLACK_TREE_NAME *tree = malloc(sizeof(RED_BLACK_TREE_NAME));
    if (tree == NULL) return NULL;

    tree->pool = RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(new)();
    if (tree->pool == NULL) {
        free(tree);
        return NULL;
    }
    tree->root = RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(get)(tree->pool);
    if (tree->root == NULL) {
        RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
        free(tree);
        return NULL;
    }
    tree->root->left = NULL;
    tree->root->right = NULL;
    tree->root->color = BLACK;
    return tree;
}

void RED_BLACK_TREE_FUNC(destroy)(RED_BLACK_TREE_NAME *tree) {
    if (tree == NULL) return;
    RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
    free(tree);
}


bool RED_BLACK_TREE_FUNC(insert)(RED_BLACK_TREE_NAME *tree, RED_BLACK_TREE_KEY_TYPE key, void *value) {
    /*
    Top-down insertion, no need for stack to store path, makes changes on the way down the tree
    */
    if (tree == NULL) return false;
    RED_BLACK_TREE_NODE *node = tree->root;
    RED_BLACK_TREE_TYPED(node_memory_pool) *pool = tree->pool;

    if (node->left == NULL) {
        // empty tree
        node->left = (RED_BLACK_TREE_NODE *)value;
        node->key = key;
        // root is always black
        node->color = BLACK;
        node->right = NULL;
    } else {
        RED_BLACK_TREE_NODE *current_node, *next_node, *upper_node;
        current_node = node;
        upper_node = NULL;
        while (current_node->right != NULL) {
            if (RED_BLACK_TREE_KEY_LESS_THAN(key, current_node->key)) {
                next_node = current_node->left;
            } else {
                next_node = current_node->right;
            }
            if (current_node->color == BLACK) {
                if (current_node->left->color == BLACK || current_node->right->color == BLACK) {
                    upper_node = current_node;
                    current_node = next_node;
                } else {
                    // both children of current_node are red, need rebalance
                    if (upper_node == NULL) {
                        // current_node is root
                        current_node->left->color = BLACK;
                        current_node->right->color = BLACK;
                        upper_node = current_node;
                    } else if (RED_BLACK_TREE_KEY_LESS_THAN(current_node->key, upper_node->key)) {
                        // current_node is the left child of upper_node
                        if (current_node == upper_node->left) {
                            // case 1, recoloring only
                            current_node->left->color = BLACK;
                            current_node->right->color = BLACK;
                            current_node->color = RED;
                        } else if (current_node == upper_node->left->left) {
                            // case 2, zig zig case, one rotation
                            RED_BLACK_TREE_FUNC(rotate_right)(upper_node);
                            upper_node->left->color = RED;
                            upper_node->right->color = RED;
                            upper_node->left->left->color = BLACK;
                            upper_node->left->right->color = BLACK;
                        } else {
                            // case 3, zig zag case, current_node == upper_node->left->right
                            RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left);
                            RED_BLACK_TREE_FUNC(rotate_right)(upper_node);
                            upper_node->left->color = RED;
                            upper_node->right->color = RED;
                            upper_node->right->left->color = BLACK;
                            upper_node->left->right->color = BLACK;
                        }
                    } else {
                        // current_node->key >= upper_node->key
                        if (current_node == upper_node->right) {
                            // case 1, recoloring only
                            current_node->left->color = BLACK;
                            current_node->right->color = BLACK;
                            current_node->color = RED;
                        } else if (current_node == upper_node->right->right) {
                            // case 2, zig zig case, one rotation
                            RED_BLACK_TREE_FUNC(rotate_left)(upper_node);
                            upper_node->left->color = RED;
                            upper_node->right->color = RED;
                            upper_node->right->left->color = BLACK;
                            upper_node->right->right->color = BLACK;
                        } else {
                            // case 3, zig zag case, two rotations
                            RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right);
                            RED_BLACK_TREE_FUNC(rotate_left)(upper_node);
                            upper_node->left->color = RED;
                            upper_node->right->color = RED;
                            upper_node->right->left->color = BLACK;
                            upper_node->left->right->color = BLACK;
                        }
                    }
                    current_node = next_node;
                    upper_node = current_node;
                }
            } else {
                // current_node is red, move down
                current_node = next_node;
            }
        } // end while, always arrives on black leaf

        if (RED_BLACK_TREE_KEY_EQUALS(key, current_node->key)) {
            // key already exists
            return false;
        }
        // current_node is the leaf that will become the parent of the new leaf
        RED_BLACK_TREE_NODE *old_leaf = RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(get)(pool);
        if (old_leaf == NULL) return false;
        old_leaf->key = current_node->key;
        old_leaf->left = current_node->left;
        old_leaf->right = NULL;
        old_leaf->color = RED;
        RED_BLACK_TREE_NODE *new_leaf = RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(get)(pool);
        if (new_leaf == NULL) {
            RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(release)(pool, old_leaf);
            return false;
        }
        new_leaf->key = key;
        new_leaf->left = (RED_BLACK_TREE_NODE *)value;
        new_leaf->right = NULL;
        new_leaf->color = RED;
                if (RED_BLACK_TREE_KEY_LESS_THAN(current_node->key, key)) {
            current_node->left = old_leaf;
            current_node->right = new_leaf;
            current_node->key = key;
        } else {
            current_node->left = new_leaf;
            current_node->right = old_leaf;
        }
    }
    return true;
}


void *RED_BLACK_TREE_FUNC(delete)(RED_BLACK_TREE_NAME *tree, RED_BLACK_TREE_KEY_TYPE key) {
    if (tree == NULL) return NULL;
    RED_BLACK_TREE_NODE *node = tree->root;
    RED_BLACK_TREE_TYPED(node_memory_pool) *pool = tree->pool;

    RED_BLACK_TREE_NODE *current_node, *upper_node, *next_node;
    void *deleted_value;
    if (node->left == NULL) {
        return NULL;
    } else if (node->right == NULL) {
        // root is a leaf
        if (RED_BLACK_TREE_KEY_EQUALS(key, node->key)) {
            deleted_value = (void *)node->left;
            node->left = NULL;
            return deleted_value;
        } else {
            return NULL;
        }
    } else {
        upper_node = node;
        if (upper_node->left->color == BLACK && upper_node->right->color == BLACK) {
            if (RED_BLACK_TREE_KEY_LESS_THAN(key, upper_node->key)) {
                if (upper_node->left->right == NULL) {
                    if (upper_node->right->right == NULL) {
                        upper_node->left->color = RED;
                        upper_node->right->color = RED;
                    } else {
                        upper_node->right->left->color = BLACK;
                        upper_node->right->right->color = BLACK;
                        upper_node->right->color = RED;
                    }
                } else {
                    if (upper_node->left->left->color == RED || upper_node->left->right->color == RED) {
                        upper_node = upper_node->left;
                    } else if (upper_node->right->right->color == RED) {
                        RED_BLACK_TREE_FUNC(rotate_left)(upper_node);
                        upper_node->right->color = BLACK;
                        upper_node->left->color = BLACK;
                        upper_node->left->left->color = RED;
                        upper_node = upper_node->left;
                    } else if (upper_node->right->left->color == RED) {
                        RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right);
                        RED_BLACK_TREE_FUNC(rotate_left)(upper_node);
                        upper_node->right->color = BLACK;
                        upper_node->left->color = BLACK;
                        upper_node->left->left->color = RED;
                        upper_node = upper_node->left;
                    } else {
                        upper_node->left->color = RED;
                        upper_node->right->color = RED;
                    }
                }
            } else {
                // key >= upper_node->key
                if (upper_node->right->left == NULL) {
                    if (upper_node->left->right == NULL) {
                        upper_node->left->color = RED;
                        upper_node->right->color = RED;
                    } else {
                        upper_node->left->left->color = BLACK;
                        upper_node->left->right->color = BLACK;
                        upper_node->left->color = RED;
                    }
                } else {
                    if (upper_node->right->right->color == RED || upper_node->right->left->color == RED) {
                        upper_node = upper_node->right;
                    } else if (upper_node->left->left->color == RED) {
                        RED_BLACK_TREE_FUNC(rotate_right)(upper_node);
                        upper_node->right->color = BLACK;
                        upper_node->left->color = BLACK;
                        upper_node->right->right->color = RED;
                        upper_node = upper_node->right;
                    } else if (upper_node->left->right->color == RED) {
                        RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left);
                        RED_BLACK_TREE_FUNC(rotate_right)(upper_node);
                        upper_node->right->color = BLACK;
                        upper_node->left->color = BLACK;
                        upper_node->right->right->color = RED;
                        upper_node = upper_node->right;
                    } else {
                        // left and right have only black nodes as neighbors below
                        upper_node->left->color = RED;
                        upper_node->right->color = RED;
                    }
                }
            }
        } // upper node has at least one red neighbor blow

        current_node = upper_node;
        while (!RED_BLACK_TREE_FUNC(node_is_leaf)(current_node)) {
            if (RED_BLACK_TREE_KEY_LESS_THAN(key, current_node->key)) {
                current_node = current_node->left;
            } else {
                current_node = current_node->right;
            }
            if (current_node->color == RED || RED_BLACK_TREE_FUNC(node_is_leaf)(current_node)) {
                continue;
            } else {
                // current_node is black and not a leaf
                if (current_node->left->color == RED || current_node->right->color == RED) {
                    // at least one child of the current node is black
                    upper_node = current_node;
                } else {
                    // both children of the current node are black
                    if (RED_BLACK_TREE_KEY_LESS_THAN(current_node->key, upper_node->key)) {
                        if (current_node == upper_node->left) {
                            if (upper_node->right->left->left->color == BLACK && upper_node->right->left->right->color == BLACK) {
                                                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node);
                                upper_node->left->color = BLACK;
                                upper_node->left->left->color = RED;
                                upper_node->left->right->color = RED;
                                current_node = upper_node = upper_node->left;
                            } else if (upper_node->right->left->left->color == RED) {
                                                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right->left);
                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right);
                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node);
                                upper_node->left->color = BLACK;
                                upper_node->right->left->color = BLACK;
                                upper_node->right->color = RED;
                                upper_node->left->left->color = RED;
                                current_node = upper_node = upper_node->left;
                            } else {
                                // upper_node->right->left->left is black and upper_node->right->left->right is red
                                                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right);
                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node);
                                upper_node->left->color = BLACK;
                                upper_node->right->left->color = BLACK;
                                upper_node->right->color = RED;
                                upper_node->left->left->color = RED;
                                current_node = upper_node = upper_node->left;
                            }
                        } else if (current_node == upper_node->left->left) {
                            if (upper_node->left->right->left->color == BLACK && upper_node->left->right->right->color == BLACK) {
                                                                upper_node->left->left->color = RED;
                                upper_node->left->right->color = RED;
                                upper_node->left->color = BLACK;
                                current_node = upper_node = upper_node->left;
                            } else if (upper_node->left->right->right->color == RED) {
                                                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left);
                                upper_node->left->left->color = BLACK;
                                upper_node->left->right->color = BLACK;
                                upper_node->left->color = RED;
                                upper_node->left->left->color = RED;
                                current_node = upper_node = upper_node->left->left;
                            } else {
                                // upper_node->left->right->left is red and upper_node->left->right->right is black
                                                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->left->right);
                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left);
                                upper_node->left->left->color = BLACK;
                                upper_node->left->right->color = BLACK;
                                upper_node->left->color = RED;
                                upper_node->left->left->left->color = RED;
                                current_node = upper_node = upper_node->left->left;
                            }
                        } else {
                            // current_node == upper_node->left->right
                            if (upper_node->left->left->left->color == BLACK && upper_node->left->left->right->color == BLACK) {
                                                                upper_node->left->left->color = RED;
                                upper_node->left->right->color = RED;
                                upper_node->left->color = BLACK;
                                current_node = upper_node = upper_node->left;
                            } else if (upper_node->left->left->left->color == RED) {
                                                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->left);
                                upper_node->left->left->color = BLACK;
                                upper_node->left->right->color = BLACK;
                                upper_node->left->color = RED;
                                upper_node->left->right->right->color = RED;
                                current_node = upper_node = upper_node->left->right;
                            } else {
                                // upper_node->left->left->left is black and upper_node->left->left->right is red
                                                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left->left);
                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->left);
                                upper_node->left->left->color = BLACK;
                                upper_node->left->right->color = BLACK;
                                upper_node->left->color = RED;
                                upper_node->left->right->right->color = RED;
                                current_node = upper_node = upper_node->left->right;
                            }

                        }
                    } else {
                        // current_node->key >= upper_node->key
                        if (current_node == upper_node->right) {
                            if (upper_node->left->right->right->color == BLACK && upper_node->left->right->left->color == BLACK) {
                                                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node);
                                upper_node->right->color = BLACK;
                                upper_node->right->right->color = RED;
                                upper_node->right->left->color = RED;
                                current_node = upper_node = upper_node->right;
                            } else if (upper_node->left->right->right->color == RED) {
                                                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left->right);
                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left);
                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node);
                                upper_node->right->color = BLACK;
                                upper_node->left->right->color = BLACK;
                                upper_node->left->color = RED;
                                upper_node->right->right->color = RED;
                                current_node = upper_node = upper_node->right;
                            } else {
                                                                // upper_node->left->right->right is black and upper_node->left->right->left is red
                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->left);
                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node);
                                upper_node->right->color = BLACK;
                                upper_node->left->right->color = BLACK;
                                upper_node->left->color = RED;
                                upper_node->right->right->color = RED;
                                current_node = upper_node = upper_node->right;
                            }
                        } else if (current_node == upper_node->right->right) {
                            if (upper_node->right->left->right->color == BLACK && upper_node->right->left->left->color == BLACK) {
                                                                upper_node->right->left->color = RED;
                                upper_node->right->right->color = RED;
                                upper_node->right->color = BLACK;
                                current_node = upper_node = upper_node->right;
                            } else if (upper_node->right->left->left->color == RED) {
                                                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right);
                                upper_node->right->left->color = BLACK;
                                upper_node->right->right->color = BLACK;
                                upper_node->right->color = RED;
                                upper_node->right->right->right->color = RED;
                                current_node = upper_node = upper_node->right->right;
                            } else {
                                //  upper_node->right->left->left is black and upper_node->right->left->right is red
                                                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->right->left);
                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right);
                                upper_node->right->left->color = BLACK;
                                upper_node->right->right->color = BLACK;
                                upper_node->right->color = RED;
                                upper_node->right->right->right->color = RED;
                                current_node = upper_node = upper_node->right->right;
                            }
                        } else {
                            // current_node == upper_node->right->left
                            if (upper_node->right->right->right->color == BLACK && upper_node->right->right->left->color == BLACK) {
                                                                upper_node->right->left->color = RED;
                                upper_node->right->right->color = RED;
                                upper_node->right->color = BLACK;
                                current_node = upper_node = upper_node->right;
                            } else if (upper_node->right->right->right->color == RED) {
                                                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->right);
                                upper_node->right->left->color = BLACK;
                                upper_node->right->right->color = BLACK;
                                upper_node->right->color = RED;
                                upper_node->right->left->left->color = RED;
                                current_node = upper_node = upper_node->right->left;
                            } else {
                                // upper_node->right->right->right is black and upper_node->right->right->left is red
                                                                RED_BLACK_TREE_FUNC(rotate_right)(upper_node->right->right);
                                RED_BLACK_TREE_FUNC(rotate_left)(upper_node->right);
                                upper_node->right->left->color = BLACK;
                                upper_node->right->right->color = BLACK;
                                upper_node->right->color = RED;
                                upper_node->right->left->left->color = RED;
                                current_node = upper_node = upper_node->right->left;
                            }
                        }
                    }
                }
            }
        } // end while, always arrives on black leaf

        if (!RED_BLACK_TREE_KEY_EQUALS(key, current_node->key)) {
            // key doesn't exist
            return NULL;
        } else {
            /*
            upper_node is the black node preceding the leaf to be deleted (current_node)
            One of upper_node's children is red. If the red node is the parent of current_node, the leaf,
            then we delete both the red node and the leaf. Otherwise, we perform a rotation on upper_node
            to bring the red node above the leaf, then delete the leaf and the red node.
            */

            RED_BLACK_TREE_NODE *other_node, *tmp_node;
            deleted_value = (void *)current_node->left;
            if (RED_BLACK_TREE_KEY_LESS_THAN(current_node->key, upper_node->key)) {
                if (current_node == upper_node->left) {
                    // upper_node->right is red
                    tmp_node = upper_node->right;
                    upper_node->key = tmp_node->key;
                    upper_node->left = tmp_node->left;
                    upper_node->right = tmp_node->right;
                } else if (current_node == upper_node->left->left) {
                    // upper_node->left is red
                    tmp_node = upper_node->left;
                    upper_node->left = tmp_node->right;
                } else {
                    // current_node == upper_node->left->right
                    tmp_node = upper_node->left;
                    upper_node->left = tmp_node->left;
                }
            } else {
                if (current_node == upper_node->right) {
                    // upper_node->left is red
                    tmp_node = upper_node->left;
                    upper_node->key = tmp_node->key;
                    upper_node->left = tmp_node->left;
                    upper_node->right = tmp_node->right;
                } else if (current_node == upper_node->right->right) {
                    // upper_node->right is red
                    tmp_node = upper_node->right;
                    upper_node->right = tmp_node->left;
                } else {
                    // current_node == upper_node->right->left
                    tmp_node = upper_node->right;
                    upper_node->right = tmp_node->right;
                }
            }
            RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(release)(pool, tmp_node);
            RED_BLACK_TREE_NODE_MEMORY_POOL_FUNC(release)(pool, current_node);
        }
        return deleted_value;
    }
}

#undef RED_BLACK_TREE_CONCAT_
#undef RED_BLACK_TREE_CONCAT
#undef RED_BLACK_TREE_TYPED
#undef RED_BLACK_TREE_FUNC
#undef RED
#undef BLACK