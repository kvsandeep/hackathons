#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

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

#define MAX_NODE 1000
#define FAIL 1
FILE *fp;
struct Graph *graph;

struct AdjListNode* newAdjListNode(int dest)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}

int updateGraph(int src, int dst, struct Graph *graph)
{
	struct Node *node;
	if (graph == NULL)
		return -1;
	node = &graph->node[src];
	node->pid = getpid();
	
	struct AdjListNode* newNode = newAdjListNode(dst);
    	newNode->next = node->head;
    	node->head = newNode;
 
    // Since graph is undirected, add an edge from dest to src also
    	newNode = newAdjListNode(src);
    	newNode->next = node->head;
    	node->head = newNode;
	return 0;
}
	
int updateDbFrmArg(int argc, char **argv, struct Graph *graph)
{
	if (argc < 3)
		return -1;

	for (int i=3; i<argc; i++) {
		int src = atoi(argv[0]);
		int dest = atoi(argv[i]);
		updateGraph(src, dest, graph);
	}

}


int createGraph(int type, int data, struct Graph *graph)
{
	static struct Node *node;
	struct AdjListNode *adjNode;
	if (graph == NULL || type < 0)
		return -1;
		
	switch (type) {
		case 0:
			node = &graph->node[data];
			break;
		case 1:
			node->pid = data;
			break;
		default :
			adjNode = (struct AdjListNode *) malloc(sizeof(struct AdjListNode));
			adjNode->dest = data;
			adjNode->next = node->head;
			node->head = adjNode;
	}
}


int updateDbFrmFile(FILE *fp, struct Graph *graph)
{
	if (graph == NULL || fp == NULL)
		return -1;
	char buf[1000];

	while(fgets(buf, 1000, fp) != NULL) {
		int type = 0;
		int i, j;
		for (i=0, j=0; buf[j]; j++) {
			if (buf[j] == ' ') {
				buf[j] = '\0';
				int num = atoi(buf+i);
				createGraph(type, num, graph);
				type++;
				i = j+1;
			}
		}
	}
}

int updateFilefrmDb(FILE *fp,  struct Graph *graph)
{
	
	if (graph == NULL || fp == NULL)
		return -1;
	for (int i=0; i < graph->entries; i++) {
		struct Node *node = &graph->node[i];
		if (node == NULL)
			continue;
		fprintf(fp, "%d %d ", i, node->pid);
		struct AdjListNode *adjNode = node->head;
		while (adjNode) {
			fprintf(fp, "%d ", adjNode->dest);
			adjNode = adjNode->next;
		}
		fprintf(fp, "\n");		
	}
}


int main(int argc, char **argv)
{
		
	
	if (argc < 4) {
		printf("invalid arg\n");
		exit(FAIL);
	}
	
	fp = fopen(argv[1], "r+");
	if (fp == NULL) {
		perror("fopen");
		exit(FAIL);
	}

	graph = (struct Graph *) malloc(sizeof(struct Graph));
	graph->entries = MAX_NODE;
	graph->node = (struct Node *) malloc(sizeof(struct Node)*MAX_NODE);
	memset(graph->node, 0x00, sizeof(struct Node)*MAX_NODE);
	
	updateDbFrmFile(fp, graph);
	updateDbFrmArg(argc, argv, graph);
}
	



