#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"
#include "treebuilder.h"
#include "string_builder.h"

void build_string_by_tree(char *buffer, struct node *root, node_q **queue)
{
    if (root)
    {
        // printf("root: %s\n", root->value);
        // if (root->p_left)
        //     printf("left: %s\n", root->p_left->value);
        // if (root->p_right)
        //     printf("right: %s\n", root->p_right->value);

        if (root->is_op_node)
        {
            // Keep below for future usage
            // // only keep "Repeat" to compare
            // const size_t len_str_repeat = strlen("Repeat");
            // char subbuff[len_str_repeat + 1];
            // memcpy(subbuff, root->value, len_str_repeat);
            // subbuff[len_str_repeat] = '\0';
            // printf("%s\n", subbuff);

            if (0 == strcmp(root->value, "And"))
            {
                enqueue_tree_node(queue, root->p_right, ADD_FROM_TAIL);
                enqueue_tree_node(queue, root->p_left, ADD_FROM_TAIL);
                // printf("And node, %s, %s, in line %d\n", __FILE__, __func__, __LINE__);
            }
            else if (0 == strcmp(root->value, "Or"))
            {
                struct node *temp = rand_interval(0, 1) ? root->p_left : root->p_right;
                enqueue_tree_node(queue, temp, ADD_FROM_HEAD);
                // printf("Or node chose [%s], %s, %s, in line %d\n", temp->value, __FILE__, __func__, __LINE__);
            }
            else // if (0 == strcmp(subbuff, "Repeat"))
            {
                size_t count = (size_t)processDefInBRA(root->p_right->value);
                // printf("It is a Repeat! count = %zu\n", count);

                for (size_t i = 0; i < count; i++)
                {
                    enqueue_tree_node(queue, root->p_left, ADD_FROM_TAIL);
                }
            }
            while (!queue_is_empty(*queue))
            {
                build_string_by_tree(buffer, dequeue_tree_node(queue), queue);
            }
        }
        else // if (!root->is_op_node)
        {   
            char *output = malloc(len_dst);
            
            execParser(root->value, output);
            strcat(buffer, output);
            // printf("buffer: %s, output: %s, %s, %s, in line %d\n", buffer, output, __FILE__, __func__, __LINE__);
            free(output);
        }
    }
}