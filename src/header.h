#ifndef __HEADER__
#define __HEADER__

struct AdjListNode {
        int dest;
        struct AdjListNode *next;
};

struct Node {
        int pid;
        struct AdjListNode *head;
};

struct Graph
{
    int entries;
    struct Node *node;
};

enum State {
        RUNNING = 0,
        TERMINATING,
        SIGNALLING
}state;


#define MAX_NODE 1000
#define FAIL 1

#endif
