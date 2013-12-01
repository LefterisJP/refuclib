#include <refu.h>
#include <Parallel/worker_pool.h>
#include <Utils/constcmp.h>
#include "../refu_tests.h"

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

static unsigned int circle_count;
static const int POINTS_NUMBER = 240000;
static const int THREADS_NUMBER = 4;

/*
 * We inscribe a circle in a square and approximate PI
 * using multiple threads. The square is 2r with a side
 * of 1. So the center of the circle is (0.5, 0.5)
 */

static inline bool pointInCircle(double x, double y)
{
    //The circle has r = 0.5 and center (0.5, 0.5)
    return ((pow(x - 0.5, 2) + pow(y - 0.5, 2)) <= 0.25);
}
static void calculatePi(void* data)
{
    unsigned int points_num = POINTS_NUMBER/THREADS_NUMBER;
    double x, y;
    do {
        x = (double)rand() / (double)RAND_MAX;
        y = (double)rand() / (double)RAND_MAX;

        if (pointInCircle(x, y)) {
            circle_count += 1;
        }
        points_num --;
    } while(points_num != 0);
}


int main()
{
    RFworker_pool *pool;
    int i;
    double pi;
    circle_count = 0;

    DEFAULT_LIB_INIT();

    srand((unsigned int)time(NULL));
    pool = rf_workerpool_create(THREADS_NUMBER);
    EXPECT_NOT(pool, NULL);

    /* add tasks to the pool */
    for (i =0; i < THREADS_NUMBER; i++) {
        rf_workerpool_add_task(pool, calculatePi, NULL);        
    }
    /* destroy the pool and wait till all is done */
    rf_workerpool_destroy(pool);

    /* use the results of the calculation to approximate pi */
    pi = 4.0f * ((float)circle_count / (float)POINTS_NUMBER);
    /* TODO: make it work with the checks in constcmp.h */
    EXPECT_TRUE(pi >= 3.12 && pi <= 3.15);
    return 0;
}
