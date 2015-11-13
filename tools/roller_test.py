#! /usr/bin/python
import re
import subprocess

titles = ['passengers', 'capacity', 'interval', 'cycles']
log_format = '{:12}' * 4
TIMES = 10


class Testcase:
    def __init__(self, n, c, t, cycles):
        self.num_passenger = str(n)
        self.capacity = str(c)
        self.time_interval = str(t)
        self.cycles = str(cycles)

    def __str__(self):
        s = self
        return log_format.format(s.num_passenger, s.capacity, s.time_interval, s.cycles)

combines = list()
combines += [(10, i + 1, 10, 5) for i in range(10)]
combines += [(10, 5, (i+1) * 10, 10) for i in range(20)]

timeit = re.compile(r'Total waiting time of all passengers: (\d+) millisec')


def report(s):
    t = 0
    for _ in range(TIMES):
        log = subprocess.check_output(['./roller', s.num_passenger, s.capacity, s.time_interval, s.cycles])
        time = timeit.findall(log)
        t += int(time[0])
    print('{} {:8} millisec'.format(s, float(t) / TIMES / int(s.num_passenger)))


def main():
    testcases = [Testcase(*c) for c in combines]
    print(log_format.format(*titles))
    for i, s in enumerate(testcases):
        report(s)

if __name__ == '__main__':
    main()
