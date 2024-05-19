#ifndef DEBUG_H
#define DEBUG_H


#define DEBUG_LVL               DEBUG_LVL_NONE

#define DEBUG_LVL_NONE          0
#define DEBUG_LVL_LIGHT         1
#define DEBUG_LVL_MEDIUM        2
#define DEBUG_LVL_FULL          3

#if (DEBUG_LVL == DEBUG_LVL_FULL)
    #define DEBUG_PRINT_TREE_PRETTY 1
#else
    #define DEBUG_PRINT_TREE_PRETTY 0
#endif

#define debug_print(lvl, fmt, ...)                        \
    do                                                    \
    {                                                     \
        if (DEBUG_LVL >= lvl)                             \
            fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                    __LINE__, __func__, __VA_ARGS__);     \
    } while (0)

#endif
