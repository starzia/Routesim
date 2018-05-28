import argparse
import datetime
import math
import random


MAX_LATENCY = 10


def random_weight():
    return random.randint(1, MAX_LATENCY)

def generate_simulation(n, degree, time, filename):
    if degree > math.log(n,2)-1:
        raise Exception("Degree must be smaller than log(n) where n is the number of nodes.")

    links = []

    print("writing %s.topo" % filename)
    with open("%s.topo" % filename, "w") as file:
        # create nodes
        for i in range(n):
            file.write("0 ADD_NODE %d 0 0\n" % i)
        # create random edges for each node
        for i in range(n):
            # don't make links truly random, favor nodes with nearby indexes
            possible_neighbors = []
            for j in range(int(math.floor(math.log(n,2)))):
                offset = 1<<j
                for neighbor in [i+offset, i-offset]:
                    if neighbor >= 0 and neighbor < n:
                        already_exists = any([l[0] == i and l[1] == neighbor for l in links])
                        if not already_exists:
                            possible_neighbors.append(neighbor)
            # choose random links
            for j in range(min(degree, len(possible_neighbors))):
                neighbor = random.choice(possible_neighbors)
                possible_neighbors.remove(neighbor)
                link = (i, neighbor, random_weight())
                reverse_link = (neighbor, i, link[2])
                links.extend([link, reverse_link])
                file.write("0 ADD_LINK %d %d %d 1\n" % link)
                file.write("0 ADD_LINK %d %d %d 1\n" % reverse_link)

    print("writing %s.event" % filename)
    with open("%s.event" % filename, "w") as file:
        file.write("1 DRAW_TOPOLOGY\n");
        for t in range(time):
            # link change events are a poisson process.
            # we want the time between events to be roughly 10 * MAX_LATENCY
            if 0 == random.randint(0, 10 * MAX_LATENCY):
                link_to_change = random.choice(links)
                file.write("%d CHANGE_LINK %d %d %d 1\n" %
                           (t, link_to_change[0], link_to_change[1], random_weight()))
        # print routing results
        for i in range(n):
            file.write("%d DUMP_TABLE %d\n" % (10*time, i))


if __name__ == "__main__":
    current_time = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S-%f')
    parser = argparse.ArgumentParser(
        description='Generate random network simulation data files (.topo and .event) for Routesim.')
    parser.add_argument('--nodes', dest='n', action='store',
                        default=20, help='number of nodes in the graph')
    parser.add_argument('--degree', dest='degree', action='store',
                        default=3, help='number of edges connected to each node')
    parser.add_argument('--time', dest='time', action='store',
                        default=1000, help='time, in seconds, to run the simulation')
    parser.add_argument('--out', dest='filename', action='store',
                        default=current_time, help='output filename prefix')
    args = parser.parse_args()
    generate_simulation(n=int(args.n), degree=int(args.degree), time=int(args.time),
                        filename=args.filename)
