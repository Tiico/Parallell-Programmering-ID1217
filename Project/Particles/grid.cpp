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

    memset(grid.grid, 0, sizeof(linkelist**) * size * size);
}

void grid_add(grid_t grid, particle_t p){
     int gridCoord = grid_coord_flat(grid.size, p->x, p->y);

     linkedlist_t * newElement = (linkelist_t *) malloc(sizeof(linkelist));
}
