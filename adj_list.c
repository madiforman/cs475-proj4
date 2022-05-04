#include <stdio.h>
#include <stdlib.h>
#define nodes 5
typedef struct node
{
    int vertex;
    struct node *next;
} node;

typedef struct Graph
{
    int size;
    struct node **list;
} Graph;

node *new_node(int v)
{
    node *newNode = malloc(sizeof(struct node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

Graph *init_graph(int size)
{
    Graph *graph = malloc(sizeof(struct Graph));
    graph->size = size;
    graph->list = malloc(size * sizeof(struct node *));
    for (int i = 0; i < size; i++)
    {
        graph->list[i] = NULL;
    }

    return graph;
}

// Add edge
void rag_request(Graph *graph, int pid, int lockid)
{
    node *new = new_node(lockid);
    new->next = graph->list[pid];
    graph->list[pid] = new;
}
void rag_alloc(Graph *graph, int pid, int lockid)
{
    node *new = new_node(pid);
    new->next = graph->list[lockid];
    graph->list[lockid] = new;
}

int cycle_util(Graph *g, int v, int *visited, int *recursive)
{
    visited[v] = 1;
    recursive[v] = 1;

    node *cur = g->list[v];
    while (cur != NULL)
    {
        if (!visited[cur->vertex] && cycle_util(g, cur->vertex, visited, recursive))
        {
            return 1;
        }
        else if (recursive[cur->vertex])
        {
            return 1;
        }
        cur = cur->next;
    }
    recursive[v] = 0;
    return 0;
}
int isCycle(Graph *g)
{
    int visited[g->size];
    int recursive_arr[g->size];
    for (int i = 0; i < g->size; i++)
    {
        visited[i] = 0;
        recursive_arr[i] = 0;
    }

    for (int i = 0; i < g->size; i++)
    {
        if (cycle_util(g, i, visited, recursive_arr))
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    struct Graph *g = init_graph(4);
    rag_request(g, 0, 1);
    rag_request(g, 1, 2);
    rag_request(g, 2, 3);
    rag_request(g, 3, 1);
    int status = isCycle(g);
    printf("%d\n", status);
    return 0;
}