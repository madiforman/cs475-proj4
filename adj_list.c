#include <stdio.h>
#include <stdlib.h>
#include "adj_list.h"

#define NLOCK 3
#define NPROC 3
int SIZE = NLOCK + NPROC;
node *new_node(int v)
{
    node *newNode = malloc(sizeof(struct node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}
RAG *init_graph(int size)
{
    RAG *graph = malloc(sizeof(struct RAG));
    graph->size = size;
    graph->list = malloc(size * sizeof(struct node *));
    for (int i = 0; i < size; i++)
    {
        graph->list[i] = NULL;
    }

    return graph;
}
void delete_edge(RAG *g, int src, int dst)
{
    node *temp = g->list[src], *ptr = g->list[src];
    int pos = 0;
    // printf("%d\n", temp->vertex);
    while (ptr != NULL)
    {
        if (ptr->vertex == dst)
        {
            break;
        }
        pos++;
        ptr = ptr->next;
    }
    if (pos == 0)
    {
        ptr = g->list[src];
        g->list[src] = ptr->next;
        free(ptr);
    }
    else
    {
        ptr = g->list[src];
        for (int i = 0; i < pos; i++)
        {
            temp = ptr;
            ptr = ptr->next;
        }
        temp->next = ptr->next;
        // printf("deleting %d\n", ptr->vertex);
        free(ptr);
    }
}
void rag_request(RAG *graph, int pid, int lockid)
{
    node *new = new_node(lockid);
    new->next = graph->list[pid+NLOCK];
    graph->list[pid+NLOCK] = new;
    new->mode = 'L';
}
void rag_alloc(RAG *g, int pid, int lockid)
{
    node *new = new_node(pid);
    new->next = g->list[lockid];
    g->list[lockid] = new;
    new->mode = 'P';
    delete_edge(g, pid, lockid);
}
void rag_dealloc(RAG *g, int pid, int lockid)
{
    delete_edge(g, pid, lockid);
}
void rag_print(RAG *rag)
{

    for (int i = 0; i < rag->size; i++)
    {
        node *temp = rag->list[i];
        if (i < NLOCK)
        {
            printf("L%d -> ", i);
        }
        else
        {
            printf("P%d -> ", i - NLOCK);
        }
        while (temp)
        {
            printf("%c%d -> ", temp->mode, temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}
int dfs(RAG *g, int v, int *visited, int *recursive)
{
    visited[v] = 1;
    recursive[v] = 1;

    node *cur = g->list[v];
    while (cur != NULL)
    {
        if (!visited[cur->vertex] && dfs(g, cur->vertex, visited, recursive))
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
int deadlock_detect(RAG *g)
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
        if (dfs(g, i, visited, recursive_arr))
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    struct RAG *g = init_graph(6);
    rag_request(g, 1, 0);
    // rag_request(g, 2, 1);
    rag_alloc(g, 1, 0);
    // delete_edge(g, 1,2);
    rag_print(g);

    // delete_edge(g, 3, 1);
    //  print_g(g);
    // int status = has_cycle(g);
    // printf("%d\n", status);
    return 0;
}