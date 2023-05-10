#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct btree_s {
    struct btree_s* left;
    struct btree_s* right;
    int content;
} btree_t;

/**
 * Create a new btree_t node and place the provided value as its content
 *
 * @param value an integer to be stored in the new node.
 * @return a pointer to the new node. Null if we fail to allocate memory.
 */
btree_t* btree_new_node(int value) {
    btree_t* node = calloc(1, sizeof(btree_t));
    if (node == NULL) {
        return NULL;
    }

    node->content = value;
    return node;
}

/**
 * Go through a tree and free all subnodes.
 *
 * @param tree a pointer to a btree_t node.
 */
void btree_free(btree_t* tree) {
    if (tree == NULL) {
        return;
    }

    btree_free(tree->left);
    btree_free(tree->right);
    free(tree);
}

/**
 * Search for a node containing the provided value in the tree.
 *
 * @param tree a pointer to a btree_t node.
 * @param value an integer to look for in the tree
 * @return a pointer to the node holding the value if found, NULL otherwise.
 */
btree_t* btree_search(btree_t* tree, int value) {
    if (tree == NULL) {
        return NULL;
    }

    if (tree->content == value) {
        return tree;
    }

    if (tree->content > value) {
        return btree_search(tree->left, value);
    }
    return btree_search(tree->right, value);
}

/**
 * Insert a new node into a tree with the value provided as its content.
 *
 * @param tree a pointer to a btree_t node where the new node will be inserted into.
 * @param value an integer to be used as the content for a new node.
 */
void btree_insert(btree_t* tree, int value) {
    if (tree == NULL || tree->content == value) {
        // Nothing to do if there is no tree or the tree already has the value
        return;
    }

    if (tree->content > value) {
        if (tree->left == NULL) {
            tree->left = btree_new_node(value);
        } else {
            btree_insert(tree->left, value);
        }
    } else {
        if (tree->right == NULL) {
            tree->right = btree_new_node(value);
        } else {
            btree_insert(tree->right, value);
        }
    }
}

typedef enum {
    SMALLEST,
    BIGGEST,
} btree_pop_bias_t;

/**
 * Look for a leaf and "pop it" from the tree.
 *
 * This function is used as part of the process for deleting a node, the deleted
 * node will be replaced by the popped leaf. In order to reach the leaf, we can
 * tell the function to look for the leaf on the lower or higher values with the
 * bias argument. By popped leaf, we refer to a node on the lowest level of the
 * tree that is removed from the tree for the caller to use without the
 * possibility of the node to be pointed by multiple points in the tree.
 *
 * @param node a pointer to a btree_t node where we will look for a leaf
 * @param bias a bias parameter for us to choose which branch should be preferred.
 * @return a pointer to the popped leaf.
 */
btree_t* btree_pop_leaf(btree_t* node, btree_pop_bias_t bias) {
    if (node == NULL) {
        return NULL;
    }

    btree_t* leaf;

    switch (bias) {
    case SMALLEST:
        leaf = node->left != NULL ? node->left : node->right;
        break;
    case BIGGEST:
        leaf = node->right != NULL ? node->right : node->left;
        break;
    default:
        printf("Programmer logic error, invalid btree pop bias\n");
        exit(-1);
    }

    if (leaf == NULL) {
        // The node itself was a leaf.
        return NULL;
    }

    if (leaf->left != NULL || leaf->right != NULL) {
        // The leaf is not actually a leaf, we need to go deeper.
        return btree_pop_leaf(leaf, bias);
    }

    // We found the leaf, pop it from the tree and return it.
    if (node->right == leaf) {
        node->right = NULL;
    } else {
        node->left = NULL;
    }
    return leaf;
}

/**
 * Replace a node from a tree with a leaf.
 *
 * This function is used as part of the delete node process. The provided node
 * will be replaced by a popped leaf, keeping the tree balanced.
 *
 * @param node a pointer to the btree_t node to be replaced by a leaf.
 * @param parent a pointer to the parent of the provided node.
 * @return a pointer to the new node in the tree.
 */
btree_t* btree_replace_node(btree_t* node, btree_t* parent) {
    if (node == NULL) {
        return NULL;
    }

    btree_t* replacement_node;
    if (node->left != NULL) {
        replacement_node = btree_pop_leaf(node->left, BIGGEST);
    } else {
        replacement_node = btree_pop_leaf(node->right, SMALLEST);
    }

    if (replacement_node != NULL) {
        replacement_node->left  = node->left;
        replacement_node->right = node->right;
    }

    if (parent) {
        if (parent->right == node) {
            parent->right = replacement_node;
        } else {
            parent->left = replacement_node;
        }
    }

    // Free the memory for the node
    node->right = node->left = NULL;
    btree_free(node);

    return replacement_node;
}

/**
 * Find a node in a tree that contains the provided value and remove it from
 * the tree, replacing it with one of its leaves.
 *
 * This function is used as part of the delete node process. The parent of the
 * node needs to be provided to know where the leaf should be added in the tree.
 *
 * @param node a pointer to the current node being probed for the value in the tree.
 * @param parent a pointer to the parent for the current node.
 * @param value the integer we are looking for in the tree.
 * @return a pointer to the new node that took this node's place, the passed in node otherwise.
 */
btree_t* btree_delete_inner(btree_t* node, btree_t* parent, int value) {
    if (node == NULL) {
        return NULL;
    }

    if (node->content == value) {
        return btree_replace_node(node, parent);
    }

    if (node->content > value) {
        btree_delete_inner(node->left, node, value);
    } else {
        btree_delete_inner(node->right, node, value);
    }
    return node;
}

/**
 * Convenience macro for deleting nodes in a tree. Looks for a node containing
 * the provided value and removes it from the tree.
 *
 * @param tree a pointer to the root of the tree to look for the value.
 * @param value an integer we are looking for in the tree.
 * @return a pointer to the root of the tree, needed if the root is the node to be removed.
 */
#define btree_delete(tree, value) btree_delete_inner(tree, NULL, value)

/**
 * Print a formatted node of a tree. This is an inner function and you should
 * use btree_print instead.
 *
 * @param tree a pointer to the current node to print.
 * @param pointy a string with the arrow that should be printed next to the node.
 * @param padding a string with the padding needed for the tree to look nice.
 */
void btree_print_inner(const btree_t* tree, const char* pointy, char padding[1024]) {
    if (tree == NULL) {
        return;
    }

    printf("%s%d\n", pointy, tree->content);

    if (tree->left == NULL && tree->right == NULL) {
        return;
    }

    if (tree->left) {
        printf("%s", padding);
        strcat(padding, "|   ");
        btree_print_inner(tree->left, "|-> ", padding);
        padding[strlen(padding) - 4] = '\0';
    } else {
        printf("%s%s\n", padding, "|-> ");
    }

    if (tree->right) {
        printf("%s", padding);
        strcat(padding, "    ");
        btree_print_inner(tree->right, "┗-> ", padding);
        padding[strlen(padding) - 4] = '\0';
    } else {
        printf("%s%s\n", padding, "┗-> ");
    }
}

/**
 * Print a tree in a nice way.
 *
 * @param tree a pointer to the tree to be printed.
 */
void btree_print(const btree_t* tree) {
    char padding[1024] = {0};
    if (tree == NULL) {
        return;
    }

    btree_print_inner(tree, "", padding);
}

int main(int argc, char* argv[]) {
    printf("============================= Starting up ======================================\n");

    btree_t* root = btree_new_node(8);
    btree_insert(root, 3);
    btree_insert(root, 10);
    btree_insert(root, 1);
    btree_insert(root, 6);
    btree_insert(root, 4);
    btree_insert(root, 7);
    btree_insert(root, 14);
    btree_insert(root, 13);
    btree_insert(root, 20);

    printf("Base tree:\n");
    btree_print(root);
    printf("================================================================================\n");

    printf("Insert nodes with values 24 and 5:\n");
    btree_insert(root, 24);
    btree_insert(root, 5);

    btree_print(root);
    printf("================================================================================\n");

    printf("Delete nodes with values 6 and 10:\n");
    root = btree_delete(root, 6);
    root = btree_delete(root, 10);

    btree_print(root);
    printf("================================================================================\n");

    printf("Remove the root of the tree for testing:\n");
    root = btree_delete(root, 8);

    btree_print(root);
    printf("================================================================================\n");

    printf("Search for an existing element:\n");
    btree_print(btree_search(root, 5));
    printf("================================================================================\n");

    printf("Search for an element that has been deleted:\n");
    btree_print(btree_search(root, 10));
    printf("================================================================================\n");

    btree_free(root);

    return 0;
}
