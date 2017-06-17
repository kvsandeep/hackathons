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

int deleteEntry(int num, struct Graph *graph)
{
	if (graph == NULL)
		return -1;
	
	
	struct Node *node = &graph->node[num];
	if (node->pid == 0)
		return -1;
	struct AdjListNode *adjNode = node->head;
	struct AdjListNode *temp; 
	while(adjNode) {
		temp = adjNode;
		adjNode = adjNode->next;
		free(temp);
	}
	node->head = NULL;
	node->pid = 0;
	
	
	for (int i=0; i<graph->entries; i++) {
		struct Node *node = &graph->node[i];
		if (node->pid == 0)
			continue;
		struct AdjListNode *adjNode = node->head;
		struct AdjListNode *temp;
		if (adjNode == NULL)
			continue;
		while (adjNode->dest == num) {
			temp = adjNode;
			node->head = adjNode->next;
			free(temp);
			adjNode = node->head;
		}
		
		while(adjNode) {

			if (adjNode->dest == num) {
				free(temp->next);
				temp->next = adjNode->next;
				adjNode = temp;
			}
			
			temp = adjNode;
			adjNode = adjNode->next;
			
		}
	}

}
int deleteGraph(struct Graph *graph)
{
	if (graph == NULL)
		return -1;
	
	for (int i=0; i<graph->entries; i++) {
		struct Node *node = &graph->node[i];
		if (node->pid == 0)
			continue;
		struct AdjListNode *adjNode = node->head;
		struct AdjListNode *temp; 
		while(adjNode) {
			temp = adjNode;
			adjNode = adjNode->next;
			free(temp);
		}
		node->head = NULL;
		node->pid = 0;
	}	
}

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
	struct Node *src_node, *dst_node;
	if (graph == NULL)
		return -1;
	src_node = &graph->node[src];
	dst_node = &graph->node[dst];
	if (dst_node->pid == 0 || (src == dst)) {
		src_node->pid = getpid();
		printf("destination node %d not present\n", dst);
		return -1;
	}
	src_node->pid = getpid();
	
	struct AdjListNode* newNode = newAdjListNode(dst);
    	newNode->next = src_node->head;
    	src_node->head = newNode;
 
    // Since graph is undirected, add an edge from dest to src also
    	newNode = newAdjListNode(src);
    	newNode->next = dst_node->head;
    	dst_node->head = newNode;
	return 0;
}
	
int updateDbFrmArg(int argc, char **argv, struct Graph *graph)
{
	if (argc < 3)
		return -1;

	for (int i=3; i<argc; i++) {
		int src = atoi(argv[2]);
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
		if (node->pid == 0)
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

void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < graph->entries; ++v)
    {
	struct Node *node = &graph->node[v];
	if (node->pid == 0)
		continue; 

        struct AdjListNode* pCrawl = graph->node[v].head;

        printf(" %d head ", v);
        while (pCrawl)
        {
            printf("-> %d", pCrawl->dest);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

int shortestPath(int src, int dst, struct Graph *graph)
{
	struct Node *src_node, *dst_node;
	if (graph == NULL)
		return -1;
	src_node = &graph->node[src];
	dst_node = &graph->node[dst];
	if ((src_node->pid == 0 )||( dst_node->pid == 0))
		return -1;
	int s_path = -1;	
	struct AdjListNode *adjNode = src_node->head;
	while(adjNode) {
		if (adjNode->dest == dst)
			return 1;
		int ret = shortestPath(adjNode->dest, dst, graph);
		if (ret < 0) {
			adjNode = adjNode->next;
			continue;
		}
		if (s_path == -1 || ret < s_path)
			s_path = ret;
		adjNode = adjNode->next;
	}
	if (s_path < 0)
		return s_path;
	return s_path+1;

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
	printGraph(graph);
	updateDbFrmArg(argc, argv, graph);
	printGraph(graph);
	rewind(fp);
	updateFilefrmDb(fp, graph);
//	printf("Shortest path %d\n", shortestPath(100, 3, graph));
	deleteEntry(3, graph);
	printGraph(graph);
}
