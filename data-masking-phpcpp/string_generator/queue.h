#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdbool.h>

typedef struct nodeQ
{
    struct node *val;
    struct nodeQ *next;
} node_q;

void enqueue_tree_node(node_q **head, struct node *treeNode, bool dir);
char *dequeue(node_q **head);
struct node *dequeue_tree_node(node_q **head);
void print_list(node_q *head);
bool queue_is_empty(node_q *head);
#endif // QUEUE_H_