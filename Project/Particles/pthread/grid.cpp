#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "grid.h"

/* Initialize grid and fill with particles */

void grid_init(grid_t &grid, int size){
    grid.size = size;

    //initialize grid

    grid.grid = (linkedlist**) malloc(sizeof(linkedlist*) * size * size);

    if (grid.grid == NULL) {
      fprintf(stderr, "Error: Could not allocate space in memory for the grid\n");
      exit(1);
    }

    memset(grid.grid, 0, sizeof(linkedlist**) * size * size);

    //initialize locks

    grid.lock = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t*) * size * size);

    if (grid.lock == NULL) {
      fprintf(stderr, "Error: Could not allocate space in memory for the locks\n");
      exit(2);
    }

    for (int i = 0; i < size*size; i++) {
        pthread_mutex_init(&grid.lock[i], NULL);
    }
}

void grid_add(grid_t &grid, particle_t* p){
     int gridCoord = grid_coord_flat(grid.size, p->x, p->y);

     linkedlist_t * newElement = (linkedlist_t *) malloc(sizeof(linkedlist));
     newElement->value = p;


     pthread_mutex_lock(&grid.lock[gridCoord]);  //Crit sec
     newElement->next = grid.grid[gridCoord];
     grid.grid[gridCoord] = newElement;
     pthread_mutex_unlock(&grid.lock[gridCoord]);  //end of Crit sec

}

bool grid_remove(grid_t &grid, particle_t * p, int gridCoord) {
    if (gridCoord == -1) {
        gridCoord = grid_coord_flat(grid.size, p->x, p->y);
    }

    //If no elements
    if (grid.grid[gridCoord] == 0) {
        return false;
    }


    pthread_mutex_lock(&grid.lock[gridCoord]); //Crit sec
    linkedlist_t ** nodePointer = &(grid.grid[gridCoord]);
    linkedlist_t * current = grid.grid[gridCoord];

    while (current && (current->value != p))
    {
      nodePointer = &(current->next);
      current = current->next;
    }
    if (current) {
        *nodePointer = current->next;
        free(current);
    }

    pthread_mutex_unlock(&grid.lock[gridCoord]); //end of crit sec


    return !!current;
}

void grid_clear(grid_t &grid){
    for (int i = 0; i < grid.size*grid.size; ++i) {
        linkedlist_t * curr = grid.grid[i];
        while(curr != 0){
            linkedlist_t * tmp = curr->next;
            free(curr);
            curr = tmp;
        }
    }
    free(grid.grid);
}

int grid_size(grid_t &grid){
    int count = 0;
    for (int i = 0; i < grid.size * grid.size; ++i) {
        linkedlist_t * curr = grid.grid[i];
        while (curr != 0) {
            count++;
            curr = curr->next;
        }
    }
    return count;
}
