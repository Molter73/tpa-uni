#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct avltree_s {
    struct avltree_s* left;
    struct avltree_s* right;
    int content;
    unsigned int height;
} avltree_t;

/**
 * Create a new avltree_t node and place the provided value as its content
 *
 * @param value an integer to be stored in the new node.
 * @return a pointer to the new node. Null if we fail to allocate memory.
 */
avltree_t* avltree_new_node(int value) {
    avltree_t* node = calloc(1, sizeof(avltree_t));
    if (node == NULL) {
        return NULL;
    }

    node->content = value;
    node->height  = 1;
    return node;
}

/**
 * Go through a tree and free all subnodes.
 *
 * @param tree a pointer to a avltree_t node.
 */
void avltree_free(avltree_t* tree) {
    if (tree == NULL) {
        return;
    }

    avltree_free(tree->left);
    avltree_free(tree->right);
    free(tree);
}

/**
 * Search for a node containing the provided value in the tree.
 *
 * @param node a pointer to a avltree_t node.
 * @param value an integer to look for in the tree
 * @return a pointer to the node holding the value if found, NULL otherwise.
 */
avltree_t* avltree_search(avltree_t* node, int value) {
    if (node == NULL) {
        return NULL;
    }

    if (node->content == value) {
        return node;
    }

    if (node->content > value) {
        return avltree_search(node->left, value);
    }
    return avltree_search(node->right, value);
}

/**
 * Get the balance factor for the current node.
 *
 * @param node a pointer to the node we are want to get the balance factor from.
 * @return the balance factor for the node.
 */
int avltree_get_balance_factor(avltree_t* node) {
    if (node == NULL) {
        return -1;
    }

    unsigned int right_height = node->right ? node->right->height : 0;
    unsigned int left_height  = node->left ? node->left->height : 0;
    return right_height - left_height;
}

/**
 * Get the height for a node by recursively calculating the height of child
 * nodes.
 *
 * @param node a pointer to the node we are trying to get the height from.
 * @return the height for the provided node.
 */
unsigned int avltree_get_height(avltree_t* node) {
    if (node == NULL) {
        return 0;
    }

    unsigned int right_depth = 1 + avltree_get_height(node->right);
    unsigned int left_depth  = 1 + avltree_get_height(node->left);

    return right_depth > left_depth ? right_depth : left_depth;
}

typedef enum {
    LEFT,
    RIGHT,
} avltree_rotation_t;

/**
 * Perform a simple rotation on an AVL tree node. The rotate argument will
 * determine if the rotation is LL or RR.
 *
 * @param node a pointer to the node to be rotated.
 * @param rotate the orientation of the rotation to be performed.
 * @return the new node that takes the current node's place in the tree.
 */
avltree_t* avltree_rotate(avltree_t* node, avltree_rotation_t rotate) {
    avltree_t* new_node;
    avltree_t* tmp;

    if (rotate == RIGHT) {
        new_node       = node->right;
        tmp            = new_node->left;
        new_node->left = node;
        node->right    = tmp;
    } else {
        new_node        = node->left;
        tmp             = new_node->right;
        new_node->right = node;
        node->left      = tmp;
    }

    new_node->height = avltree_get_height(new_node);
    node->height     = avltree_get_height(node);

    return new_node;
}

/**
 * Balance an AVL tree node by getting it's balance factor and rotating it
 * accordingly, then replace the node in its parent with the new node.
 *
 * The balance can be achieved in 1 of 4 ways:
 * LL rotation: The left node of the current node is "left heavy", this
 *   happens when both the current and left nodes have negative balance factors.
 * RR rotation: Similar to the LL rotation, but with the current and right
 *   nodes having positive balance factors.
 * LR rotation: This happens when the left node of the current node is "right
 *   heavy", meaning the current node has a negative balance factor and its
 *   left node has a positive one. In this case we first perform a RR rotation
 *   on the left node and then do a LL rotation on the current node.
 * RL rotation: Mirror situation to the LR rotation.
 *
 * @param node a pointer to the node to be balanced.
 * @param parent a pointer to the parent of the node being balanced.
 * @return the node that took the current node's place in the tree.
 *         NULL if no rotation was performed.
 */
avltree_t* avltree_balance(avltree_t* node, avltree_t* parent) {
    avltree_t* new_node = NULL;

    int balance_factor = avltree_get_balance_factor(node);
    if (balance_factor > 1) {
        if (avltree_get_balance_factor(node->right) < 0) {
            new_node    = avltree_rotate(node->right, LEFT);
            node->right = new_node;
        }

        new_node = avltree_rotate(node, RIGHT);
    } else if (balance_factor < -1) {
        if (avltree_get_balance_factor(node->left) > 0) {
            new_node   = avltree_rotate(node->left, RIGHT);
            node->left = new_node;
        }

        new_node = avltree_rotate(node, LEFT);
    }

    if (new_node && parent) {
        if (parent->left == node) {
            parent->left = new_node;
        } else {
            parent->right = new_node;
        }
    }

    return new_node != NULL ? new_node : node;
}

/**
 * Inner function used for inserting nodes into an AVL tree recursively. Once
 * done inserting, this function also rebalances the full tree on its way back
 * to the original caller.
 * This is not meant to be used directly, you should use avltree_insert instead.
 *
 * @param node a pointer to the current node that is being iterated.
 * @param parent a pointer to the parent of the current
 * @param value an integer to be used as the content for a new node.
 */
avltree_t* avltree_insert_inner(avltree_t* node, avltree_t* parent, int value) {
    if (node == NULL || node->content == value) {
        return NULL;
    }

    if (node->content > value) {
        if (node->left == NULL) {
            node->left = avltree_new_node(value);
        } else {
            avltree_insert_inner(node->left, node, value);
        }
    } else {
        if (node->right == NULL) {
            node->right = avltree_new_node(value);
        } else {
            avltree_insert_inner(node->right, node, value);
        }
    }

    node->height = avltree_get_height(node);

    return avltree_balance(node, parent);
}

/**
 * Insert a new node into a tree with the value provided as its content.
 *
 * @param tree a pointer to a avltree_t node where the new node will be inserted into.
 * @param value an integer to be used as the content for a new node.
 */
avltree_t* avltree_insert(avltree_t* tree, int value) {
    if (tree == NULL || tree->content == value) {
        // Nothing to do if there is no tree or the tree already has the value
        return tree;
    }

    return avltree_insert_inner(tree, NULL, value);
}

typedef enum {
    SMALLEST,
    BIGGEST,
} avltree_pop_bias_t;

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
 * @param node a pointer to a avltree_t node where we will look for a leaf
 * @param bias a bias parameter for us to choose which branch should be preferred.
 * @return a pointer to the popped leaf.
 */
avltree_t* avltree_pop_leaf(avltree_t* node, avltree_pop_bias_t bias) {
    if (node == NULL) {
        return NULL;
    }

    avltree_t* leaf;

    switch (bias) {
    case SMALLEST:
        leaf = node->left != NULL ? node->left : node->right;
        break;
    case BIGGEST:
        leaf = node->right != NULL ? node->right : node->left;
        break;
    default:
        printf("Programmer logic error, invalid avltree pop bias\n");
        exit(-1);
    }

    if (leaf == NULL) {
        // The node itself was a leaf.
        leaf = node;
        goto end;
    }

    if (leaf->left != NULL || leaf->right != NULL) {
        // The leaf is not actually a leaf, we need to go deeper.
        avltree_t* retval = avltree_pop_leaf(leaf, bias);
        leaf->height      = avltree_get_height(leaf);
        return retval;
    }

end:
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
 * @param node a pointer to the avltree_t node to be replaced by a leaf.
 * @param parent a pointer to the parent of the provided node.
 * @return a pointer to the new node in the tree.
 */
avltree_t* avltree_replace_node(avltree_t* node, avltree_t* parent) {
    if (node == NULL) {
        return NULL;
    }

    avltree_t* replacement_node;
    if (node->left != NULL) {
        replacement_node = avltree_pop_leaf(node->left, BIGGEST);
    } else {
        replacement_node = avltree_pop_leaf(node->right, SMALLEST);
    }

    if (replacement_node != NULL) {
        replacement_node->left  = node->left != replacement_node ? node->left : NULL;
        replacement_node->right = node->right != replacement_node ? node->right : NULL;
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
    avltree_free(node);

    return replacement_node;
}

/**
 * Find a node in a tree that contains the provided value and remove it from
 * the tree, replacing it with one of its leaves.
 *
 * This function is used as part of the delete node process. The parent of the
 * node needs to be provided to know where the leaf should be added in the tree.
 *
 * Once the deletion is done, the tree is rebalanced from the deletion point
 * upwards to the root.
 *
 * @param node a pointer to the current node being probed for the value in the tree.
 * @param parent a pointer to the parent for the current node.
 * @param value the integer we are looking for in the tree.
 * @return a pointer to the new node that took this node's place, the passed in node otherwise.
 */
avltree_t* avltree_delete_inner(avltree_t* node, avltree_t* parent, int value) {
    if (node == NULL) {
        return NULL;
    }

    if (node->content == value) {
        node = avltree_replace_node(node, parent);
    } else if (node->content > value) {
        avltree_delete_inner(node->left, node, value);
    } else {
        avltree_delete_inner(node->right, node, value);
    }

    // If the deleted node was a leaf, nothing left to do.
    if (node == NULL) {
        return NULL;
    }

    node->height = avltree_get_height(node);

    avltree_t* new_node = avltree_balance(node, parent);

    return new_node ? new_node : node;
}

/**
 * Convenience macro for deleting nodes in a tree. Looks for a node containing
 * the provided value and removes it from the tree.
 *
 * @param tree a pointer to the root of the tree to look for the value.
 * @param value an integer we are looking for in the tree.
 * @return a pointer to the root of the tree, needed if the root is the node to be removed.
 */
#define avltree_delete(tree, value) avltree_delete_inner(tree, NULL, value)

/**
 * Print a formatted node of a tree. This is an inner function and you should
 * use avltree_print instead.
 *
 * @param tree a pointer to the current node to print.
 * @param pointy a string with the arrow that should be printed next to the node.
 * @param padding a string with the padding needed for the tree to look nice.
 */
void avltree_print_inner(const avltree_t* tree, const char* pointy, char padding[1024]) {
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
        avltree_print_inner(tree->left, "|-> ", padding);
        padding[strlen(padding) - 4] = '\0';
    } else {
        printf("%s%s\n", padding, "|-> ");
    }

    if (tree->right) {
        printf("%s", padding);
        strcat(padding, "    ");
        avltree_print_inner(tree->right, "┗-> ", padding);
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
void avltree_print(const avltree_t* tree) {
    char padding[1024] = {0};
    if (tree == NULL) {
        return;
    }

    avltree_print_inner(tree, "", padding);
}

int main() {
    printf("============================= Starting up ======================================\n");

    avltree_t* root = avltree_new_node(10);
    root            = avltree_insert(root, 5);
    root            = avltree_insert(root, 15);
    root            = avltree_insert(root, 3);
    root            = avltree_insert(root, 8);
    root            = avltree_insert(root, 20);

    printf("Base tree:\n");
    avltree_print(root);
    printf("================================================================================\n");

    printf("Insert node with value 24:\n");
    root = avltree_insert(root, 24);

    avltree_print(root);
    printf("================================================================================\n");

    printf("Delete node with value 20:\n");
    root = avltree_delete(root, 20);

    avltree_print(root);
    printf("================================================================================\n");

    printf("Search for an existing element:\n");
    avltree_print(avltree_search(root, 5));
    printf("================================================================================\n");

    printf("Search for an element that has been deleted:\n");
    avltree_print(avltree_search(root, 20));
    printf("================================================================================\n");

    printf("Balance factor for node 10: %u\n", avltree_get_balance_factor(avltree_search(root, 10)));
    printf("================================================================================\n");

    printf("Height of the tree: %u\n", root->height);
    printf("================================================================================\n");

    printf("Insert node 6 and 7 to force a LR rotation: \n");
    root = avltree_insert(root, 6);
    root = avltree_insert(root, 7);
    avltree_print(root);
    printf("================================================================================\n");

    printf("Insert node 23 to force a RL rotation: \n");
    root = avltree_insert(root, 23);
    avltree_print(root);
    printf("================================================================================\n");

    printf("Remove the root of the tree for testing:\n");
    root = avltree_delete(root, 10);

    avltree_print(root);
    printf("================================================================================\n");

    printf("Remove a leaf of the tree for testing:\n");
    root = avltree_delete(root, 3);

    avltree_print(root);
    printf("================================================================================\n");

    avltree_free(root);

    return 0;
}
