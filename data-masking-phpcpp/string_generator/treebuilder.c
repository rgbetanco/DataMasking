#include "treebuilder.h"

extern const bool ADD_FROM_HEAD;
extern const bool ADD_FROM_TAIL;

// define DEBUG_TREE
#ifdef DEBUG_TREE
#define MAXLEN 9
//use typedef to make calling the compare function easier
typedef int (*Compare)(const char *, const char *);

//inserts elements into the tree
void insert(char *key, tree_node **leaf, Compare cmp)
{
    int res;
    if (*leaf == NULL)
    {
        *leaf = (tree_node *)malloc(sizeof(tree_node));
        (*leaf)->value = malloc(strlen(key) + 1); // memory for key
        strcpy((*leaf)->value, key);              // copy the key
        (*leaf)->p_left = NULL;
        (*leaf)->p_right = NULL;
        //printf(  "\nnew node for %s" , key);
    }
    else
    {
        res = cmp(key, (*leaf)->value);
        if (res < 0)
            insert(key, &(*leaf)->p_left, cmp);
        else if (res > 0)
            insert(key, &(*leaf)->p_right, cmp);
        else // key already exists
            printf("Key '%s' already in tree\n", key);
    }
}

//compares value of the new node against the previous node
int CmpStr(const char *a, const char *b)
{
    return (strcmp(a, b)); // string comparison instead of pointer comparison
}

char *input(void)
{
    static char line[MAXLEN + 1]; // where to place key
    printf("Please enter a string : ");
    fgets(line, sizeof line, stdin);
    return (strtok(line, "\n")); // remove trailing newline
}

//recursive function to print out the tree inorder
void in_order(tree_node *root)
{
    if (root != NULL)
    {
        in_order(root->p_left);
        printf("   %s\n", root->value); // string type
        in_order(root->p_right);
    }
}

//searches elements in the tree
void search(char *key, tree_node *leaf, Compare cmp) // no need for **
{
    int res;
    if (leaf != NULL)
    {
        res = cmp(key, leaf->value);
        if (res < 0)
            search(key, leaf->p_left, cmp);
        else if (res > 0)
            search(key, leaf->p_right, cmp);
        else
            printf("\n'%s' found!\n", key); // string type
    }
    else
        printf("\nNot in tree\n");
    return;
}
#endif // DEBUG_TREE

#if DEBUG_PRINT_TREE_PRETTY
void padding(char ch, int n)
{
    int i;

    for (i = 0; i < n; i++)
        putchar(ch);
}

void print_tree_pretty(tree_node *root, int level)
{
    if (root == NULL)
    {
        padding('\t', level);
        puts("~");
    }
    else
    {
        print_tree_pretty(root->p_right, level + 1);
        padding('\t', level);
        printf("%s\n", root->value);
        print_tree_pretty(root->p_left, level + 1);
    }
}
#endif

void delete_tree(tree_node **leaf)
{
    if (*leaf != NULL)
    {
        delete_tree(&(*leaf)->p_left);
        delete_tree(&(*leaf)->p_right);
        free((*leaf)->value); // free the key
        free((*leaf));
    }
}

void createTreeNode(const char *key, tree_node **leaf)
{
    *leaf = (tree_node *)malloc(sizeof(tree_node));
    (*leaf)->value = malloc(strlen(key) + 1); // memory for key
    strcpy((*leaf)->value, key);              // copy the key
    (*leaf)->p_left = NULL;
    (*leaf)->p_right = NULL;
    (*leaf)->is_op_node = false;
}

void treeBuilder(const char *root_name, const char *s1, const char *s2, node_q **queue_head)
{
    // printf("---Start---%s %s %s %s---\n", __func__, root_name, s1, s2);
    tree_node *p_root = NULL;
    createTreeNode(root_name, &p_root);

    if (0 == strcmp("Or", root_name))
    {
        p_root->is_op_node = true;
        p_root->p_left = dequeue_tree_node(queue_head);
        p_root->p_right = dequeue_tree_node(queue_head);
        enqueue_tree_node(queue_head, p_root, ADD_FROM_HEAD);
    }
    else if (0 == strcmp("Repeat()", root_name))
    {
        p_root->is_op_node = true;
        p_root->p_left = dequeue_tree_node(queue_head);
        createTreeNode(s2, &p_root->p_right);
        enqueue_tree_node(queue_head, p_root, ADD_FROM_HEAD);
    }
    else if (s1[0] == '\0' && s2[0] == '\0')
    {
        enqueue_tree_node(queue_head, p_root, ADD_FROM_HEAD);
    }
    else
    {
        p_root->is_op_node = true;
        createTreeNode(s1, &p_root->p_left);
        createTreeNode(s2, &p_root->p_right);
        enqueue_tree_node(queue_head, p_root, ADD_FROM_HEAD);
    }
    //in_order(p_root);
    // printf("---End-----%s------\n", __func__);
}

tree_node *tree_construct(node_q **queue_head)
{
    printf("\n");
    tree_node *temp = NULL;
    tree_node *tree_root = NULL;
    while (!queue_is_empty(*queue_head))
    {
        temp = dequeue_tree_node(queue_head);
        if (!queue_is_empty(*queue_head))
        {
            tree_node *p_root = NULL;
            createTreeNode("And", &p_root);
            p_root->p_left = temp;
            p_root->p_right = dequeue_tree_node(queue_head);
            p_root->is_op_node = true;
            enqueue_tree_node(queue_head, p_root, ADD_FROM_TAIL);
        }
        else // Tree node queue is empty now, we can print the tree completely.
        {
            tree_root = temp;
            #if DEBUG_PRINT_TREE_PRETTY
            print_tree_pretty(temp, 0);
            #endif
        }
    }
    return tree_root;
}

#ifdef DEBUG_TREE
//displays menu for user
void menu()
{
    printf("\nPress 'i' to insert an element\n");
    printf("Press 's' to search for an element\n");
    printf("Press 'p' to print the tree inorder\n");
    printf("Press 'f' to destroy current tree\n");
    printf("Press 'q' to quit\n");
}

int main()
{
    tree_node *p_root = NULL;
    char *value;
    char option = 'x';

    while (option != 'q')
    {
        //displays menu for program
        menu();

        //gets the char input to drive menu
        option = getchar(); // instead of two getchar() calls

        if (option == 'i')
        {
            value = input();
            printf("Inserting %s\n", value);
            insert(value, &p_root, (Compare)CmpStr);
        }
        else if (option == 's')
        {
            value = input();
            search(value, p_root, (Compare)CmpStr); // no need for **
        }
        else if (option == 'p')
        {
            in_order(p_root);
        }
        else if (option == 'f')
        {
            delete_tree(&p_root);
            printf("Tree destroyed");
            p_root = NULL;
        }
        else if (option == 'q')
        {
            printf("Quitting");
        }
    }
    return 0;
}
#endif // DEBUG_TREE
