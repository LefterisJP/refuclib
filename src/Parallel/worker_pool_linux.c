/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/

/*------------- Corrensponding Header inclusion -------------*/
#include <Parallel/worker_pool.h>
/*------------- Refu C library inclusion -------------*/
#include <Utils/log.h>
#include <Utils/memory.h>
#include <Data_Structures/intrusive_list.h>
/*------------- System specific inclusion -------------*/
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
/*------------- Libc inclusion -------------*/
#include <string.h>
#include <time.h>
/*------------- End of includes -------------*/


/* ====== RF_WorkerTask -- Start ====== */

typedef struct WorkerTask {
    //! Function pointer to the job to be done
    ptr2task task_ptr;
    //! Pointer to the data passes as argument to the task
    void *task_data;
    //! Node to attach the task to the work queue
    RF_ILNode ln;
} RF_WorkerTask;

/* ====== RF_WorkerTask -- End ====== */

/* ====== RF_WorkerThread -- Start ====== */

typedef struct WorkerThread {
    //! Posix thread
    pthread_t t;
    //! Work queue
    RF_ILHead work_queue;
    //! Signals that the worker must terminate
    bool must_terminate;
    //! Node to attach the worker to the pool
    RF_ILNode ln;
} RF_WorkerThread;

static void *WorkerLoop(void *t)
{
    RF_WorkerThread *worker = t;
    RF_WorkerTask *task;
    /* do all thread specific initialization here*/

    while(!worker->must_terminate) {
        usleep(RF_OPTION_WORKER_SLEEP_MICROSECONDS);
        task = rfIList_Pop(&worker->work_queue, RF_WorkerTask, ln);
        while(task) {
            /* execute and free the task */
            task->task_ptr(task->task_data);
            free(task);
            task = rfIList_Pop(&worker->work_queue, RF_WorkerTask, ln);
        }
    }

    /* do all thread specific freeing here */
    return 0;
}

static bool rfWorkerThread_Init(RF_WorkerThread *thread)
{
    pthread_attr_t attributes;

    pthread_attr_init(&attributes);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);
    if (pthread_create(&thread->t, &attributes, WorkerLoop, thread) != 0) {
        RF_ERROR("pthread create failed with %d", errno);
        pthread_attr_destroy(&attributes);
        return false;
    }
    pthread_attr_destroy(&attributes);

    rfIList_HeadInit(&thread->work_queue);
    thread->must_terminate = false;
    return true;
}

static RF_WorkerThread *rfWorkerThread_Create()
{
    RF_WorkerThread *ret;
    RF_MALLOC(ret, sizeof(*ret), NULL);
    
    if (!rfWorkerThread_Init(ret)) {
        free(ret);
        return NULL;
    }
    return ret;
}

/* ====== RF_WorkerThread -- End ====== */


typedef struct WorkerPool {
    //! The list of workers
    RF_ILHead workers_list;
    //! The number of worker thread
    int workers_num;
    //! The load of each worker in tasks (always incrementing)
    int load[RF_OPTION_MAX_WORKER_THREADS];
} RF_WorkerPool;

/**
 ** Stupid load balancing algorithm for now
 **/
static int rfWorkerPool_DecideWorker(RF_WorkerPool *p)
{
    int i;
    int max_index, max_load;
    max_index = max_load = -1;
    for (i = 0; i < p->workers_num; i++) {
        if (p->load[i] >= max_load) {
            max_index = i;
            max_load = p->load[i];
        }
    }
    return max_index;
}

bool rfWorkerPool_Init(RF_WorkerPool *p, int initial_workers_num)
{
    RF_WorkerThread *worker;
    int i;

    if (initial_workers_num > RF_OPTION_MAX_WORKER_THREADS) {
        RF_ERROR("Provided \"%d\" initial worker number exceeds the "
                 "maximum allowed limit");
        return false;
    }

    p->workers_num = initial_workers_num;
    /* memset(p->load, 0, sizeof(p->load) - 1); */
    for (i = 0; i < RF_OPTION_MAX_WORKER_THREADS; i ++) {
        p->load[i] = 0;
    }

    rfIList_HeadInit(&p->workers_list);
    for (i = 0; i < initial_workers_num; i ++) {
        worker = rfWorkerThread_Create();
        if (!worker) {
            RF_ERROR("Failed to initialize a worker");
            return false;
        }
        rfIList_Add(&p->workers_list, &worker->ln);
    }
    
    return true;
}

RF_WorkerPool *rfWorkerPool_Create(int initial_workers_num)
{
    RF_WorkerPool *ret;
    RF_MALLOC(ret, sizeof(*ret), NULL);
    
    if (!rfWorkerPool_Init(ret, initial_workers_num)) {
        free(ret);
        return NULL;
    }
    return ret;
}

void rfWorkerPool_Destroy(RF_WorkerPool *p)
{
    RF_WorkerThread *worker;
    RF_WorkerThread *tmp;

    /* signal all threads they must terminate */
    rfIList_ForEach(&p->workers_list, worker, ln) {
        worker->must_terminate = true;
    }

    /* wait till they do */
    rfIList_ForEach(&p->workers_list, worker, ln) {
        pthread_join(worker->t, NULL);
    }

    /* and now we can free */
    rfIList_ForEach_safe(&p->workers_list, worker, tmp, ln) {
        free(worker);
    }
    free(p);
}


i_DECLIMEX_ bool rfWorkerPool_AddTask(RF_WorkerPool *p,
                                      ptr2task task_ptr,
                                      void* data)
{
    RF_WorkerTask *task;
    RF_WorkerThread *worker;
    int worker_index, i;

    RF_MALLOC(task, sizeof(*task), false);
    task->task_ptr = task_ptr;
    task->task_data = data;

    worker_index = rfWorkerPool_DecideWorker(p);

    i = 0;
    rfIList_ForEach(&p->workers_list, worker, ln) {
        if (i == worker_index) {
            rfIList_AddTail(&worker->work_queue, &task->ln);
            p->load[worker_index] ++;
            return true;
        }
        i++;
    }

    RF_ERROR("Could not assign task to worker. Should never happen");
    return false;
}
