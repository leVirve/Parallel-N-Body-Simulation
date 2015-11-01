#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

int num_thread, iters;
double mass, t;
struct body { double x, y, vx, vy; };
struct Vector { double x, y };
int num_body;
body *bodies, *new_bodies;

const double G = 6.67384e-11;
double Gmm;

void diff_v(int index, Vector& v)
{
	body &a = bodies[index];
	double f_sum_x = 0, f_sum_y = 0;
	for (int k = 0; k < num_body; ++k) {
        if (j == k) continue;
        body &b = bodies[k];
        double dx = b.x - a.x, dy = b.y - a.y, f_x , f_y,
               radius_cube_sqrt = pow(pow(dx, 2) + pow(dy, 2), 1.5);
        f_sum_x +=  Gmm * dx / radius_cube_sqrt;
        f_sum_y +=  Gmm * dy / radius_cube_sqrt;
    }
    v = { f_sum_x * t / mass , f_sum_y * t / mass };
}

void calc()
{
    Gmm = G * mass * mass;
    for (int i = 0; i < iters; ++i) {
        for (int j = 0; j < num_body; ++j) {
        	Vector v;
        	diff_v(j, v);
            body &new_b = new_bodies[j], &b = bodies[j];
            new_b.vx = b.vx + v.x;
            new_b.vy = b.vx + v.y;
            new_b.x  = b.x + new_b.vx * t;
            new_b.y  = b.y + new_b.vy * t;
        }
        for (int j = 0; j < num_body; ++j) bodies[j] = new_bodies[j];
    }
}

int main(int argc, char const **argv)
{
	puts("./out #threads m T t input angle enable/disable xmin ymin len L");

	num_thread = stoi(argv[1]), iters = stoi(argv[3]);
	mass = stod(argv[2]), t = stod(argv[4]);
	ifstream input;
	input.open(argv[5]);

	input >> num_body;
	bodies = new body[num_body];
	new_bodies = new body[num_body];

	double x, y, vx, vy;
	for (int i = 0; i < num_body; ++i) {
		body& t = bodies[i];
		input >> t.x >> t.y >> t.vx >> t.vy;
	}
	input.close();

	calc();
	for (int j = 0; j < num_body; ++j)
		cout <<"Body " << j
			 << ": x= " << bodies[j].x <<" y= " << bodies[j].y
			 << " Vx= " << bodies[j].vx << " Vy= " << bodies[j].vy << endl;
	return 0;
}
