/*
 A parallel Jacobi solver for the Laplacian equation in 2D
 Written by Jean M. Favre, Swiss National Supercomputing Center
 Last tested on Thu Mar  1 11:42:25 CET 2018 with Sensei

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PARALLEL
#include <mpi.h> 
#endif

#include "solvers.h"

#ifdef ENABLE_SENSEI
#include "senseiConfig.h"
#include "Bridge.h"
#endif

int main(int argc, char *argv[])
{
  simulation_data sim;
  SimInitialize(&sim);

#ifdef PARALLEL
  sim.cart_dims[0] = sim.cart_dims[1] = 0;
  int PartitioningDimension = 2; // want a 2D MPI partitioning. otherwise set to 1.
  MPI_Init(&argc, &argv);                       /* starts MPI */
  MPI_Comm_rank(MPI_COMM_WORLD, &sim.par_rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &sim.par_size); /* get # procs from env or */

  MPI_Partition(PartitioningDimension, &sim);

  neighbors(&sim);
#endif

// We use (bx + 2) grid points in the X direction, i.e. interior points plus 2 b.c. points
// We use (by + 2) grid points in the Y direction, i.e. interior points plus 2 b.c. points
  // decompose the domain

  AllocateGridMemory(&sim);

  set_initial_bc(&sim);

#ifdef ENABLE_SENSEI
  bridge_initialize(MPI_COMM_WORLD,
                    sim.m+1, sim.m+1, 1,  // global length, not extents!
                    sim.bx+2, sim.by+2, 1, // length of each sub-domain
                    sim.rankx * (sim.bx+1), sim.ranky * (sim.by+1), 0, // start_extents
                    "/users/jfavre/Projects/InSitu/Jacobi/Sensei/jacobi.xml");
#endif


  while ((sim.gdel > TOL) && (sim.iter <= MAXSTEPS))
    {  // iterate until error below threshold
    simulate_one_timestep(&sim);
#ifdef ENABLE_SENSEI
   bridge_update(sim.iter, sim.iter*1.0, sim.Temp);
#endif
    }

#ifdef PARALLEL
  if (!sim.par_rank)
#endif
    fprintf(stdout,"Stopped at iteration %d\nThe maximum error = %f\n", sim.iter, sim.gdel);

  WriteFinalGrid(&sim);

  FreeGridMemory(&sim);

#ifdef ENABLE_SENSEI
  bridge_finalize();
#endif

#ifdef PARALLEL
  MPI_Finalize();
#endif

  return (0);
}
