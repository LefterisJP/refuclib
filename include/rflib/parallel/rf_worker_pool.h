/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_WORKER_POOL_H
#define RF_WORKER_POOL_H

#include <rflib/defs/imex.h>
#include <stdbool.h> 

typedef void (*ptr2task)(void*);

struct WorkerPool;
typedef struct WorkerPool RFworker_pool;


i_DECLIMEX_ RFworker_pool *rf_workerpool_create(int initial_workers_num);

i_DECLIMEX_ void rf_workerpool_destroy(RFworker_pool *p);

i_DECLIMEX_ bool rf_workerpool_add_task(
    RFworker_pool *p,
    ptr2task task_ptr,
    void* data
);


#endif
