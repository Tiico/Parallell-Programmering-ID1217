#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

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
        printf( "-n <int> to set number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        printf( "-p <int> To specify the amount of threads to use\n" );
        printf( "-s <int> to set the number of steps in the simulation\n" );
        return 0;
    }

    int n = read_int( argc, argv, "-n", 1000 );
    char *savename = read_string( argc, argv, "-o", NULL );
    int n_threads = read_int(argc, argv, "-p", 2);
    int nsteps = read_int(argc, argv, "-s", NSTEPS);

    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;

    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    double size = set_size( n );
    init_particles( n, particles );

    //Grid optimization
    int gridSize = (size/cutoff) + 1;
    grid_t grid;
    grid_init(grid, gridSize);
      for (int i = 0; i < n; ++i) {
        grid_add(grid, &particles[i]);
      }

    omp_set_num_threads(n_threads);

    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );

    #pragma omp parallel
    for( int step = 0; step < nsteps; step++ )
    {
        //
        //  compute all forces
        //
        #pragma omp for
        for( int i = 0; i < n; i++ )
        {
            particles[i].ax = particles[i].ay = 0;
            int gx = grid_coord(particles[i].x);
            int gy = grid_coord(particles[i].y);

            for(int x = max(gx - 1, 0); x <= min(gx + 1, gridSize-1); x++)
            {
                for(int y = max(gy - 1, 0); y <= min(gy + 1, gridSize-1); y++)
                {
                    linkedlist_t * curr = grid.grid[x * grid.size + y];
                    while(curr != 0)
                    {
                        apply_force(particles[i], *(curr->value));
                        curr = curr->next;
                    }
                }
            }
        }

        //
        //  move particles
        //
        #pragma omp for
        for( int i = 0; i < n; i++ ){
          int gc = grid_coord_flat(grid.size, particles[i].x, particles[i].y);
          move( particles[i] );

            // Re-add the particle if it has changed grid position
          if (gc != grid_coord_flat(grid.size, particles[i].x, particles[i].y))
          {
              if (! grid_remove(grid, &particles[i], gc))
              {
                  fprintf(stdout, "Error: Failed to remove particle '%p'. Code must be faulty. Blame source writer.\n", &particles[i]);
                  exit(3);
              }
              grid_add(grid, &particles[i]);
          }
        }

        //
        //  save if necessary
        //
        #pragma omp master
        if( fsave && (step%SAVEFREQ) == 0 )
            save( fsave, n, particles );
    }
    simulation_time = read_timer( ) - simulation_time;

    printf("n = %d,\tsimulation time = %g seconds\n", n, simulation_time );

    grid_clear(grid);
    free( particles );
    if( fsave )
        fclose( fsave );

    return 0;
}
