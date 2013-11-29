#include <Parallel/threading.h>

#include <pthread.h>

int rf_ThreadGetId()
{
    return pthread_self();
}
