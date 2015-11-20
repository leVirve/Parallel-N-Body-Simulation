#include "utils.h"

bool gui;
double mass, t, Gmm, angle;
Body *bodies, *new_bodies;
int num_thread, iters, num_body;

nanoseconds total_time;

int main(int argc, char const **argv)
{
    init_env(argc, argv);
    high_resolution_clock::time_point s;

    Gmm = G * mass * mass;
    for (int i = 0; i < iters; ++i) {
        if (gui) draw_points(0);
        s = high_resolution_clock::now();
        for (int j = 0; j < num_body; ++j) move_nth_body(j);
        Body* t = new_bodies; new_bodies = bodies; bodies = t;
        total_time += timeit(s);
    }
    INFO("Run in " << total_time.count() / 1000 << " us");
    return 0;
}
