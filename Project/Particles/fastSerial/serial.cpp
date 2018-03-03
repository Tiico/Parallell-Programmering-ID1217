#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <vector>

#include "common.h"
#include "grid.h"

using namespace std;

//
//  benchmarking program
//
int main( int argc, char **argv )
{
    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        return 0;
    }

    int n = read_int( argc, argv, "-n", 1000 );
    char *savename = read_string( argc, argv, "-o", NULL );

    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    double size = set_size(n);
    init_particles( n, particles );

    /*
        Create the grid that will be used to optimize the interaction between particles.
    */
    int gridSize = (size/cutoff) + 1;
    grid_t grid;
    grid_init(grid, gridSize);
    for (int i; i < n; ++i) {
        grid_add(grid, &particles[i]);
    }

    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
    for( int step = 0; step < NSTEPS; step++ )
    {
        //
        //  compute forces
        //
        for( int i = 0; i < n; i++ )
        {
            particles[i].ax = particles[i].ay = 0;
            int gx = grid_coord(particles[i].x);
            int gy = grid_coord(particles[i].y);

            for (int x = max(gx-1, 0); x <= min(gx + 1, gridSize - 1); x++) {
                for (int y = max(gy - 1, 0); y <= min(gy + 1, gridSize - 1); y++) {
                    linkedlist_t * curr = grid.grid[x * grid.size + y];
                      while (curr != 0) {
                          apply_force( particles[i], *(curr->value));
                          curr = curr->next;
                    }
                }
            }
        }

        //
        //  move particles
        //
        for( int i = 0; i < n; i++ ){
          int gc = grid_coord_flat(grid.size, particles[i].x, particles[i].y);
            move( particles[i] );

            // Add particles back if it's changed position.

            if (gc != grid_coord_flat(grid.size, particles[i].x, particles[i].y)) {
                if (! grid_remove(grid, &particles[i], gc)) {
                  fprintf(stdout, "Error: Failed to remove particle '%p'. Code must be faulty. Blame source writer.\n", &particles[i]);
                    exit(3);
                }
                grid_add(grid, &particles[i]);
            }
          }
        //
        //  save if necessary
        //
        if( fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
    }
    simulation_time = read_timer( ) - simulation_time;

    printf( "n = %d, simulation time = %g seconds\n", n, simulation_time );

    grid_clear(grid);
    free( particles );
    if( fsave )
        fclose( fsave );

    return 0;
}
