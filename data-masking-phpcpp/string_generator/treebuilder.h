#ifndef TREEBUILDER_H_
#define TREEBUILDER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "queue.h"
#include "../debug.h"

// struct for node
typedef struct node
{
    char *value; // all void* types replaced by char*
    struct node *p_left;
    struct node *p_right;
    bool is_op_node;
} tree_node;

#ifdef __cplusplus
extern "C"
{
#endif
    // fix: undefined reference to `delete_tree(node**)'
    extern void delete_tree(tree_node **leaf);

#ifdef __cplusplus
}
#endif

void createTreeNode(const char *key, tree_node **leaf);
void treeBuilder(const char *root_name, const char *s1, const char *s2, node_q **queue_head);
tree_node *tree_construct(node_q **queue_head);
#endif // TREEBUILDER_H_