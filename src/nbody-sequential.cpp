#include "utils.h"

bool gui;
double mass, t, Gmm;
Body *bodies, *new_bodies;
int num_thread, iters, num_body;

void move_nth_body(int index)
{
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
}

int main(int argc, char const **argv)
{
    init_env(argc, argv);

    Gmm = G * mass * mass;
    for (int i = 0; i < iters; ++i) {
        if (gui) draw_points();
        for (int j = 0; j < num_body; ++j) move_nth_body(j);
        Body* t = new_bodies; new_bodies = bodies; bodies = t;
    }
    return 0;
}
