#include <pthread.h>
#include "utils.h"

bool gui;
double mass, t, Gmm;
Body *bodies, *new_bodies;
int num_thread, iters, num_body;

int queuing_jobs = 0;
pthread_mutex_t queuing;
pthread_cond_t processing;

void* move_nth_body(void* i)
{
    int index = *(int*) i;
    Body &a = bodies[index], &new_a = new_bodies[index];
    double f_sum_x = 0, f_sum_y = 0;
    for (int i = 0; i < num_body; ++i) {
        if (index == i) continue;
        Body &b = bodies[i];
        double dx = b.x - a.x, dy = b.y - a.y,
               radius_cube_sqrt = pow(pow(dx, 2) + pow(dy, 2), 1.5) + 10e-7;
        f_sum_x +=  Gmm * dx / radius_cube_sqrt;
        f_sum_y +=  Gmm * dy / radius_cube_sqrt;
    }
    new_a.vx = a.vx + f_sum_x * t / mass;
    new_a.vy = a.vy + f_sum_y * t / mass;
    new_a.x  = a.x + new_a.vx * t;
    new_a.y  = a.y + new_a.vy * t;
    delete (int*) i;
    pthread_mutex_lock(&queuing);
    queuing_jobs--;
    pthread_mutex_unlock(&queuing);
    pthread_cond_signal(&processing);
}

int main(int argc, char const **argv)
{
    init_env(argc, argv);

    pthread_t workers[num_body];
    pthread_mutex_init(&queuing, NULL);
    pthread_cond_init(&processing, NULL);

    Gmm = G * mass * mass;
    for (int i = 0; i < iters; ++i) {
        if (gui) draw_points(0);
        for (int j = 0; j < num_body; ++j) {
            pthread_mutex_lock(&queuing);
            while (queuing_jobs >= num_thread)
                pthread_cond_wait(&processing, &queuing);
            queuing_jobs++;
            pthread_mutex_unlock(&queuing);
            int *index = new int; *index = j;
            pthread_create(&workers[j], NULL, move_nth_body, (void*) index);
        }
        for (int j = 0; j < num_body; ++j)
            pthread_join(workers[j], NULL);
        Body* t = new_bodies; new_bodies = bodies; bodies = t;
    }
    return 0;
}
