#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "header.h"

char *file;
struct Graph *graph;
int this_node;
int this_pid;

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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
}


int updateDbFrmFile(char *file, struct Graph *graph)
{
	FILE *fp = fopen(file, "r+");
	if (fp == NULL) {
		perror("fopen");
		exit(FAIL);
	}

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
	fclose(fp);
	return 0;
}

int updateFilefrmDb(char *file,  struct Graph *graph)
{	
	FILE *fp = fopen(file, "w+");
	if (fp == NULL) {
		perror("fopen");
		exit(FAIL);
	}
	
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
	fclose(fp);
	return 0;
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

void sendSignal(struct Graph *graph) 
{
	printf("sendSignal\n");
	if (graph == NULL)
		return;

	for (int i=0; i < graph->entries; i++) {
        	struct Node *node = &graph->node[i];
                if (node->pid == 0 || node->pid == this_pid)
                        continue;
		if (kill(node->pid, SIGUSR1) < 0) {
			perror("killall");
		}
	}

}


void *handler(void *ptr)
{
	printf("handler\n");
	while(1) {
	switch (state) {
		case RUNNING:
			sleep(1);
			break;
		case TERMINATING:
			printf("terminating\n");
			deleteEntry(this_node, graph);
      			updateFilefrmDb(file, graph);
			sendSignal(graph);
			state = RUNNING;
        		exit(1);
			break;
		case SIGNALLING:
			printf("updating\n");
			deleteEntry(this_node, graph);
			deleteGraph(graph);
       	 		updateDbFrmFile(file, graph);
			state = RUNNING;
			break;
		default:
			printf("unknown state\n");
			sleep(1);
		}	
	}
}


void terminate_isr(int val)
{
	printf("in Term Sig Handler\n");
	state = TERMINATING;

}

void userSignal_isr(int val)
{
	printf("in User Sig Handler\n");
	state = SIGNALLING;
}

int main(int argc, char **argv)
{
	int ret;	
	pthread_t tid;
	if (argc < 4) {
		printf("invalid arg\n");
		exit(FAIL);
	}
	
	file = argv[1];

	
	if (SIG_ERR == signal(SIGINT, terminate_isr)) {
                perror ("sigal:");
                exit(EXIT_FAILURE);
        }
	if (SIG_ERR == signal(SIGUSR1, userSignal_isr)) {
                perror ("sigal:");
                exit(EXIT_FAILURE);
        }

	if (ret = pthread_create(&tid, NULL, handler, NULL)) {
                perror("thread ");
                printf("return %d\n", ret);
                exit(1);
        }

	graph = (struct Graph *) malloc(sizeof(struct Graph));
	graph->entries = MAX_NODE;
	graph->node = (struct Node *) malloc(sizeof(struct Node)*MAX_NODE);
	memset(graph->node, 0x00, sizeof(struct Node)*MAX_NODE);
	
	updateDbFrmFile(file, graph);
	this_node = atoi(argv[2]);
	this_pid = getpid();
	updateDbFrmArg(argc, argv, graph);
	updateFilefrmDb(file, graph);
	sendSignal(graph);
	
	int option;
	while(1) {
		printf(	"1: Print graph\n" 
			"2: show distance\n");
		scanf("%d", &option);
		int src, dst;	
		switch(option) {
			case 1:
				printGraph(graph);
				break;
			case 2:
				printf("i/p format: src_id <space> dst_id\n");
				scanf("%d %d", &src, &dst);
				 printf("Shortest path %d\n", shortestPath(src, dst, graph));
				break;
			default:
				printf("invalid option\n");
		}
	}
}
