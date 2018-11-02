#include "Communicator.h"
#include "mpi.h"
#include <map>

namespace harp {
    namespace com {

        template<class SIMPLE>
        void Communicator::barrier() {
            MPI_Barrier(MPI_COMM_WORLD);
        }

        template<class SIMPLE>
        void Communicator::allGather(harp::ds::Table<SIMPLE> *table) {

        }

        template<class SIMPLE>
        void Communicator::broadcast(ds::Table<SIMPLE> *table, int bcastWorkerId) {

            int workerId;
            MPI_Comm_rank(MPI_COMM_WORLD, &workerId);

            //determining number of partitions to bcast
            long partitionCount;
            if (bcastWorkerId == workerId) {
                partitionCount = table->getPartitionCount();
            }
            MPI_Bcast(&partitionCount, 1, MPI_DOUBLE, bcastWorkerId, MPI_COMM_WORLD);

            //broadcasting partition ids and sizes
            long partitionIds[partitionCount * 2];// [id, size]
            int index = 0;
            if (bcastWorkerId == workerId) {
                for (const auto p : table->getPartitions()) {
                    partitionIds[index++] = p.first;
                    partitionIds[index++] = p.second->getData()->getSize();
                }
            }
            MPI_Bcast(&partitionIds, partitionCount * 2, MPI_INT, bcastWorkerId, MPI_COMM_WORLD);

            MPI_Datatype datatype;//todo determine data type.. currently assuming all int

            //now receiving partitions
            for (long i = 0; i < partitionCount; i += 2) {
                long partitionId = partitionIds[i];
                long partitionSize = partitionIds[i + 1];


            }


            printf("%d %d %s\n", partitionCount, partitionIds[partitionCount - 1], typeid(SIMPLE).name());
        }
    }
}