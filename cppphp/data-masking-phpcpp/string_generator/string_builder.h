#ifndef STRING_BUILDER_H_
#define STRING_BUILDER_H_

#include "queue.h"
extern const size_t len_dst;
extern const bool ADD_FROM_HEAD;
extern const bool ADD_FROM_TAIL;
extern int processDefInBRA(char *s);
extern bool queue_is_empty(node_q *head);
extern struct node *dequeue_tree_node(node_q **head);
extern unsigned int rand_interval(unsigned int min_num, unsigned int max_num);

void build_string_by_tree(char *buffer, struct node *root, node_q **queue);
#endif // STRING_BUILDER_H_