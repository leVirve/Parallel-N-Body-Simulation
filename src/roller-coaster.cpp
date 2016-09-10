#include <vector>
#include <string>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
using namespace std;
using namespace std::chrono;

pthread_mutex_t output, queuing;
pthread_cond_t running_cond, playing_cond;
bool finish = false;
milliseconds wait_time;
high_resolution_clock::time_point s;

string suffix(const int n)
{
    int d = n % 10;
    return to_string(n) +
        ((d == 1) ? "st" : (d == 2) ? "nd" : (d == 3) ? "rd" : "th") + " ";
}

void print(const string str)
{
    pthread_mutex_lock(&output);
    cout << str << endl;
    pthread_mutex_unlock(&output);
}

milliseconds time(high_resolution_clock::time_point s)
{
    auto t = duration_cast<milliseconds>(high_resolution_clock::now() - s);
    cout << t.count() << " millisec ";
    return t;
}

class RollerCoaster;

class Passenger {
public:
    int index;
    RollerCoaster& coaster_car;
    Passenger(int i, RollerCoaster& c) : index(i), coaster_car(c) {}

    void wander() {
        print(suffix(index) + "passenger wanders around park");
        int wander_time = rand() % 10;
        usleep(wander_time * 1000);
    }

    void oncar() {
        pthread_mutex_lock(&queuing);
        pthread_cond_wait(&playing_cond, &queuing);
        pthread_mutex_unlock(&queuing);
    }
};

class RollerCoaster {
public:
    int capacity, time_interval, cycles, runs;
    vector<Passenger> seats;
    RollerCoaster(int sz, int t, int cc)
        :capacity(sz), time_interval(t), cycles(cc) { runs = 0; }

    void run() {
        pthread_mutex_lock(&queuing);
        while (seats.size() != capacity)
            pthread_cond_wait(&running_cond, &queuing);
        cout << "car depatures at " << time(s).count() << " millisec ";
        print(passengers_name() + "passengers are in the car");
        runs++;
        pthread_mutex_unlock(&queuing);
        usleep(time_interval * 1000);
        release();
    }

    void enqueue(Passenger p) {
        pthread_mutex_lock(&queuing);
        if (runs >= cycles) { finish = true;  go_away(p); }
        print(suffix(p.index) + "passenger returns for another ride");
        while (seats.size() == capacity) {
            auto a = high_resolution_clock::now();
            pthread_cond_wait(&playing_cond, &queuing);
            wait_time += time(a);
        }
        if (finish) go_away(p);
        seats.push_back(p);
        if (seats.size() == capacity)
            pthread_cond_signal(&running_cond);
        pthread_mutex_unlock(&queuing);
    }
private:
    void go_away(Passenger p) {
        print(suffix(p.index) + "passanger leave");
        pthread_mutex_unlock(&queuing);
        pthread_exit(NULL);
    }

    string passengers_name() {
        string s;
        for(auto p : seats) s += suffix(p.index);
        return s;
    }

    void release(){
        pthread_mutex_lock(&queuing);
        cout << "car arrives at " << time(s).count() << " millisec ";
        print(passengers_name() + "passengers get off");
        seats.clear();
        pthread_cond_broadcast(&playing_cond);
        pthread_mutex_unlock(&queuing);
    }
};

RollerCoaster* coaster_car;

void* passenger(void* p)
{
    Passenger passenger = *(Passenger*) p;
    while (true) {
        passenger.wander();
        passenger.coaster_car.enqueue(passenger);
        passenger.oncar();
        if (finish) break;
    }
    print(suffix(passenger.index) + "leave");
}

void* roller_coaster(void* c)
{
    RollerCoaster* car = (RollerCoaster*) c;
    for (int i = 0; i < car->cycles; ++i) car->run();
}

void initialize()
{
    srand(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count());
    pthread_mutex_init(&output, NULL);
    pthread_mutex_init(&queuing, NULL);
    pthread_cond_init(&playing_cond, NULL);
    pthread_cond_init(&running_cond, NULL);
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        puts("Usage: ./out $num_passengers $capacity $time_interval $cycles");
        exit(1);
    }

    int num_passengers = stoi(argv[1]),
        capacity       = stoi(argv[2]),
        time_interval  = stoi(argv[3]),
        cycles         = stoi(argv[4]);
    pthread_t passenger_threads[num_passengers];
    pthread_t coaster_thread;
    s = high_resolution_clock::now();

    RollerCoaster coaster_car = RollerCoaster(capacity, time_interval, cycles);
    Passenger* passengers[num_passengers];
    pthread_create(&coaster_thread, NULL, roller_coaster, (void*) &coaster_car);
    for (int i = 0; i < num_passengers; ++i) {
        passengers[i] = new Passenger(i + 1, coaster_car);
        pthread_create(&passenger_threads[i], NULL, passenger, (void*) passengers[i]);
    }
    for (int i = 0; i < num_passengers; ++i) pthread_join(passenger_threads[i], NULL);
    pthread_join(coaster_thread, NULL);

    cout << "Total waiting time of all passengers: " << wait_time.count() << " millisec\n";

    return 0;
}
