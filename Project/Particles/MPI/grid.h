#ifndef __GRID_H_
#define __GRID_H_

#include "common.h"

struct linkedlist{
        linkedlist * next;
        int particle_id;
};

typedef struct linkedlist linkedlist_t;

struct grid{
        int size;
        linkedlist_t ** grid;
};

typedef struct grid grid_t;

/* Grid Routines */

void grid_init(grid_t &grid, int gridsize);
void grid_add(grid_t &grid, particle_t * particle);
bool grid_remove(grid_t &grid, particle_t * p, int gridCoord = -1);
void grid_clear(grid_t &grid);
void grid_clear_row(grid_t &grid, int r);
int grid_size(grid_t &grid);

/* Calculate grid coordinates based on the real coordinates */

inline static int grid_coord(double c){
    return (int)floor(c / cutoff);
}

inline static int grid_coord_flat(int size, double x, double y){
    return grid_coord(x) * size + grid_coord(y);
}

#endif
