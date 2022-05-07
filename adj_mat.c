#include <stdio.h>
#include <stdlib.h>
#include "adj_mat.h"

#define NLOCK 3
#define NPROC 3

int SIZE = NLOCK + NPROC;

void init_mat(){
    int i, j;
    matrix = malloc(SIZE*sizeof(int*));;
    for(i=0;i<=SIZE;++i){
        matrix[i] = (int*)malloc(SIZE*sizeof(int));
    }
    for (i = 0; i < SIZE; i++){
        for (j = 0; j < SIZE; j++){
            matrix[i][j] =0;
        }
    }
    
}

void rag_request(int pid, int lockid){

    matrix[(SIZE/2)+pid][lockid] = 1;
}

void rag_alloc(int pid, int lockid){
    matrix[lockid][(SIZE/2)+pid] = 1;
    matrix[(SIZE/2)+pid][lockid] = 0;
}

void rag_dealloc(int pid, int lockid){
    matrix[lockid][(SIZE/2)+pid] = 0;
    matrix[(SIZE/2)+pid][lockid] = 0;
}

void rag_print(){
    int i, j;
    for (i = 0; i < SIZE; i++){
        for (j = 0; j < SIZE; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    for (i = 0; i < SIZE; i++){
        if(i<=2){
            printf("L%d -> ", matrix[i][j]);
        }else{
            printf("P%d -> ", matrix[i][j]);
        }
        for (j = 0; j < SIZE; j++){
            if(matrix[i][j]==1){
                if(j<=2){
                printf("L%d -> ", matrix[i][j]);
            }else{
                printf("P%d -> ", matrix[i][j]);
            }
            }
        }
        printf("\n");
        
    }
    
}

// int dfs(int i, int *visited, int *recursive){
//     int j;
//     visited[i] = 1;
//     printf("%d-> ", i++);
//     for (j = 0; j < SIZE; j++){
//         if(visited[j]==0&&matrix[i][j]==1){
//             dfs(j, visited, recursive);
//         }
//     }
// }

// int deadlock_detect(){
//     int visited[SIZE];
//     int recursive[SIZE];

//     for (int i = 0; i < SIZE; i++){
//         visited[i] = 0;
//         recursive[i] = 0;
//     }

//     for (int i = 0; i < SIZE; i++)
//     {
//         if (dfs(i, visited, recursive))
//         {
//             return 1;
//         }
//     }
//     return 0;
// }

int main(int argc, char *argv[]){

    init_mat();
    rag_request(0,1);
    rag_request(1,0);
    rag_print(matrix);
    // FILE* file;
    // if(argc != 2) // if there are not enough command arguments
    // {
    //     printf("Usage: ./tests <file>\n");
    // }
    // else
    // {
    //     init_mat();
    //     char *arg = argv[1];
    //     file = fopen(arg, "r");
    //     if(file != NULL)
    //     {
    //         struct RAG *graph = init_graph(6);
    //         char event;
    //         int pid, lockid;
    //         while(fscanf(file, "%d %c %d\n", &pid, &event, &lockid) != EOF)
    //         {
    //             if(event == 'R')
    //             {
    //                 rag_request(pid, lockid);
    //             }
    //             else if(event == 'A')
    //             {   
    //                 rag_alloc(pid, lockid);
    //             }
    //             else if(event == 'D')
    //             {
    //                 rag_dealloc(pid, lockid);
    //             }
    //         }
    //         int status = deadlock_detect(graph);
    //         printf("status: %d\n", status);
    //         rag_print();
    //     }
    //     else
    //     {
    //         printf("Error: File cannot be found\n");
    //     }
    // }
    // return 0;
}