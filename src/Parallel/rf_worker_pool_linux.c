/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
/*------------- Corrensponding Header inclusion -------------*/
#include <Parallel/rf_worker_pool.h>
/*------------- Module related inclusion -------------*/
#include <Parallel/rf_threading.h>
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


/* ====== RFworker_task -- Start ====== */

typedef struct WorkerTask {
    //! Function pointer to the job to be done
    ptr2task task_ptr;
    //! Pointer to the data passes as argument to the task
    void *task_data;
    //! Node to attach the task to the work queue
    RFilist_node ln;
} RFworker_task;

/* ====== RFworker_task -- End ====== */

/* ====== RFworker_thread -- Start ====== */

typedef struct WorkerThread {
    //! Posix thread
    pthread_t t;
    //! Work queue
    RFilist_head work_queue;
    //! Signals that the worker must terminate
    bool must_terminate;
    //! Node to attach the worker to the pool
    RFilist_node ln;
} RFworker_thread;

static void *WorkerLoop(void *t)
{
    RFworker_thread *worker = t;
    RFworker_task *task;
    /* do all thread specific initialization here */
    if (!rf_init_thread_specific()) {
        return 0;
    }

    while(!worker->must_terminate) {
        usleep(RF_OPTION_WORKER_SLEEP_MICROSECONDS);
        task = rf_ilist_pop(&worker->work_queue, RFworker_task, ln);
        while(task) {
            /* execute and free the task */
            task->task_ptr(task->task_data);
            free(task);
            task = rf_ilist_pop(&worker->work_queue, RFworker_task, ln);
        }
    }

    /* do all thread specific freeing here */
    rf_deinit_thread_specific();
    return 0;
}

static bool rf_workerthread_init(RFworker_thread *thread)
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

    rf_ilist_head_init(&thread->work_queue);
    thread->must_terminate = false;
    return true;
}

static RFworker_thread *rf_workerthread_create()
{
    RFworker_thread *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    
    if (!rf_workerthread_init(ret)) {
        free(ret);
        return NULL;
    }
    return ret;
}

/* ====== RFworker_thread -- End ====== */


typedef struct WorkerPool {
    //! The list of workers
    RFilist_head workers_list;
    //! The number of worker thread
    int workers_num;
    //! The load of each worker in tasks (always incrementing)
    int load[RF_OPTION_MAX_WORKER_THREADS];
} RFworker_pool;

/**
 ** Stupid load balancing algorithm for now
 **/
static int rf_workerpool_decide_worker(RFworker_pool *p)
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

bool rf_workerpool_init(RFworker_pool *p, int initial_workers_num)
{
    RFworker_thread *worker;
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

    rf_ilist_head_init(&p->workers_list);
    for (i = 0; i < initial_workers_num; i ++) {
        worker = rf_workerthread_create();
        if (!worker) {
            RF_ERROR("Failed to initialize a worker");
            return false;
        }
        rf_ilist_add(&p->workers_list, &worker->ln);
    }
    
    return true;
}

RFworker_pool *rf_workerpool_create(int initial_workers_num)
{
    RFworker_pool *ret;
    RF_MALLOC(ret, sizeof(*ret), return NULL);
    
    if (!rf_workerpool_init(ret, initial_workers_num)) {
        free(ret);
        return NULL;
    }
    return ret;
}

void rf_workerpool_destroy(RFworker_pool *p)
{
    RFworker_thread *worker;
    RFworker_thread *tmp;

    /* signal all threads they must terminate */
    rf_ilist_for_each(&p->workers_list, worker, ln) {
        worker->must_terminate = true;
    }

    /* wait till they do */
    rf_ilist_for_each(&p->workers_list, worker, ln) {
        pthread_join(worker->t, NULL);
    }

    /* and now we can free */
    rf_ilist_for_each_safe(&p->workers_list, worker, tmp, ln) {
        free(worker);
    }
    free(p);
}


bool rf_workerpool_add_task(RFworker_pool *p,
                                      ptr2task task_ptr,
                                      void* data)
{
    RFworker_task *task;
    RFworker_thread *worker;
    int worker_index, i;

    RF_MALLOC(task, sizeof(*task), return false);
    task->task_ptr = task_ptr;
    task->task_data = data;

    worker_index = rf_workerpool_decide_worker(p);

    i = 0;
    rf_ilist_for_each(&p->workers_list, worker, ln) {
        if (i == worker_index) {
            rf_ilist_add_tail(&worker->work_queue, &task->ln);
            p->load[worker_index] ++;
            return true;
        }
        i++;
    }

    RF_ERROR("Could not assign task to worker. Should never happen");
    return false;
}
