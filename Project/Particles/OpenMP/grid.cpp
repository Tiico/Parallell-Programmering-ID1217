#include <stdlib.h>
#include <string.h>
#include <omp.h>
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

    grid.lock = (omp_lock_t*)malloc(size * size * sizeof(omp_lock_t));

    if (grid.lock == NULL)
    {
      fprintf(stderr, "Error: Could not allocate memory for the locks!\n");
      exit(2);
    }

    for (int i = 0; i < size*size; i++) {
      omp_init_lock(&grid.lock[i]);
    }
}

void grid_add(grid_t &grid, particle_t* p){
     int gridCoord = grid_coord_flat(grid.size, p->x, p->y);

     linkedlist_t * newElement = (linkedlist_t *) malloc(sizeof(linkedlist));
     newElement->value = p;

     omp_set_lock(&grid.lock[gridCoord]);
     //Crit sec
     newElement->next = grid.grid[gridCoord];
     grid.grid[gridCoord] = newElement;
     //end of Crit sec
     omp_unset_lock(&grid.lock[gridCoord]);
}

bool grid_remove(grid_t &grid, particle_t * p, int gridCoord) {
    if (gridCoord == -1) {
        gridCoord = grid_coord_flat(grid.size, p->x, p->y);
    }

    //If no elements
    if (grid.grid[gridCoord] == 0) {
        return false;
    }

    //Crit sec
    omp_set_lock(&grid.lock[gridCoord]);
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
    omp_unset_lock(&grid.lock[gridCoord]);
    //end of crit sec

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
