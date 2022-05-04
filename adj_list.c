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

void delete_edge(Graph *g, int src, int dst)
{
    for (int i = 0; i < g->size; i++)
    {
        node *n = g->list[i];
        if (n->vertex == src)
        {
            node *temp = n->next;
            printf("%d\n", n->vertex);
            printf("%d\n,", n->next->vertex);
            // while (temp != NULL)
            // {

            //     if (temp->vertex == dst)
            //     {
            //     }
            //     temp = temp->next;
            // }
        }
    }
}
void print_g(Graph *g)
{

    for (int i = 0; i < g->size; i++)
    {
        node *temp = g->list[i];
        printf("\nVertex %d: ", i);
        while (temp)
        {
            printf("%d -> ", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}
node *find_prev(Graph *g, int v, int u)
{
    node *N;
    for (int i = 0; i < g->size; i++)
    {
        if (g->list[i]->vertex == u)
        {

            N = g->list[i];
            printf("here\n");
            while (N->next != NULL)
            {

                if (N->next->vertex == v)
                {
                    return N;
                }
                N = N->next;
            }
            return NULL;
        }
    }
    return NULL;
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
    print_g(g);
    node *n = find_prev(g, 3, 1);
    printf("%d\n", n->vertex);
    // printf("**********\n");
    // delete_edge(g, 3, 1);
    // print_g(g);
    //  int status = isCycle(g);
    //  printf("%d\n", status);
    return 0;
}