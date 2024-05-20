#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "treebuilder.h"
#include "queue.h"

const char *null_indicator = "NULL";
const bool ADD_FROM_HEAD = true;
const bool ADD_FROM_TAIL = false;

void enqueue(node_q **head, const char *val)
{
    node_q *new_node = malloc(sizeof(node_q));
    if (!new_node)
    {
        free(new_node);
        return;
    }
    createTreeNode(val, &new_node->val);
    new_node->next = *head;

    *head = new_node;
    
    free(new_node);
}

void enqueue_tree_node(node_q **head, struct node *treeNode, bool dir)
{
    // init a new node to contain treeNode
    node_q *new_node = malloc(sizeof(node_q));
    new_node->next = NULL;  // fix Segmentation fault (core dumped)
    
    if (!new_node)
    {
        free(new_node);
        return;
    }
    new_node->val = treeNode;

    if (ADD_FROM_HEAD == dir)
    {
        new_node->next = *head;
        *head = new_node;
    }
    else // ADD_FROM_TAIL == dir
    {
        if (queue_is_empty(*head))
        {
            *head = new_node;
        }
        else
        {
            node_q *current = *head;
            
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = new_node;
        }
    }
}

char *dequeue(node_q **head)
{
    node_q *current, *prev = NULL;
    char *retval = "";

    if (*head == NULL)
        return (char *)null_indicator;

    current = *head;
    while (current->next != NULL)
    {
        prev = current;
        current = current->next;
    }

    retval = current->val->value;
    free(current);

    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return retval;
}

struct node *dequeue_tree_node(node_q **head)
{
    node_q *current, *prev = NULL;
    struct node *ret_tree_node = NULL;

    if (*head == NULL)
        return ret_tree_node;

    current = *head;
    while (current->next != NULL)
    {
        prev = current;
        current = current->next;
    }

    ret_tree_node = current->val;
    free(current);

    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return ret_tree_node;
}

void print_list(node_q *head)
{
    printf("---%s in line %d---\n", __func__, __LINE__);
    node_q *current = head;
    while (current != NULL)
    {
        printf("%s\n", current->val->value);
        current = current->next;
    }
    printf("---%s in line %d---\n", __func__, __LINE__);
}

bool queue_is_empty(node_q *head)
{
    return head == NULL;
}

//#define QUEUE_DEBUG
#ifdef QUEUE_DEBUG
int main()
{
    node_q *head = NULL;
    char *ret = "";
    enqueue(&head, "11");
    enqueue(&head, "22");
    enqueue(&head, "33");
    enqueue(&head, "44");

    struct node *p_root = NULL;
    createTreeNode("55", &p_root);
    enqueue_tree_node(&head, p_root, ADD_FROM_TAIL);

    struct node *p_root_1 = NULL;
    createTreeNode("66", &p_root_1);
    enqueue_tree_node(&head, p_root_1, ADD_FROM_HEAD);

    print_list(head);

    while (1)
    {
        ret = dequeue(&head);
        if (strcmp(null_indicator, ret) == 0)
            break;
        printf("dequeued %s\n", ret);
    }
    printf("done. head=%p\n", head);

    return 0;
}
#endif
