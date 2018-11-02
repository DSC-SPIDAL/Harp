#include <iostream>
#include "Worker.h"
#include "mpi.h"

namespace harp {

    void Worker::init() {
        MPI_Init(NULL, NULL);

        int worldSize;
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        MPI_Comm_rank(MPI_COMM_WORLD, &this->workerId);
    }

    void Worker::start() {
        com::Communicator comm;
        this->execute(this->workerId, &comm);
        MPI_Finalize();
    }
}