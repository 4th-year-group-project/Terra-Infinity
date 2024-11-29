##Comparing APSP algorithms

import random
import time
import scipy.sparse.csgraph as csgraph
from scipy.sparse import csr_matrix as csr_matrix
import numpy as np
import matplotlib.pyplot as plt
from numba import njit
import numba as nb
import functools as ft


@njit(fastmath=True)
# Generates a random weight between 1 and 11
def random_weight():
    return 1 + int(10 * random.random())

# Generates a lattice with random weights between nodes
@njit(fastmath=True)
def gen_lattice(n):
    size = n**2
    lattice = np.zeros((size, size))
    for i in range(n):
        for j in range(n):
            index = (i * n) + j
            if j > 0:
                lattice[index-1][index] = lattice[index][index-1] = random_weight()
            if i > 0:
                lattice[index - n][index] = lattice[index][index-n] = random_weight()

    return lattice

def run_scipy():
    # Generate a lattice
    n = 100
    lattice = gen_lattice(n)

    # Convert lattice to a sparse matrix
    lattice = csr_matrix(lattice)

    # Find all pairs shortest paths
    start = time.time()
    x = csgraph.shortest_path(lattice, directed=False, method='auto')

    end = time.time()
    return end - start

@njit(fastmath=True)
def dijkstra(M, lattice, adj_list, s, t):
    n = len(lattice)
    not_visited = [i for i in range(n)]
    dist = np.full(n, np.inf)
    dist[s] = 0
    for i in range(n):
        u = -1
        for j in not_visited:
            if u == -1 or dist[j] < dist[u]:
                u = j

        not_visited.remove(u)
    


        if lattice[u][adj_list[u][0]] != 0:
            dist[adj_list[u][0]] = min(dist[adj_list[u][0]], dist[u] + lattice[u][adj_list[u][0]])
        if lattice[u][adj_list[u][1]] != 0:
            dist[adj_list[u][1]] = min(dist[adj_list[u][1]], dist[u] + lattice[u][adj_list[u][1]])
        if lattice[u][adj_list[u][2]] != 0:
            dist[adj_list[u][2]] = min(dist[adj_list[u][2]], dist[u] + lattice[u][adj_list[u][2]])
        if lattice[u][adj_list[u][3]] != 0:
            dist[adj_list[u][3]] = min(dist[adj_list[u][3]], dist[u] + lattice[u][adj_list[u][3]])
           
    # ##Find the path from s to t
    # path = np.zeros(n, dtype=np.int64)
    # u = t
    # length = 0
    # while u != s:
    #     path[length] = u
    #     for i in range(n):
    #         if lattice[u][i] != 0 and dist[u] == dist[i] + lattice[u][i]:
    #             u = i
    #             break
    #     length += 1
    # path[length] = s

    # for i in range(length - 1):
    #     M[path[i]][path[i+1]] = dist[t] - dist[path[i+1]]
    #     M[path[i+1]][path[i]] = dist[t] - dist[path[i+1]]
    
    # for i in range(len(path)): 
    #     M[path[i]][t] = dist[t] - dist[path[i]]

    ##update M with the new distances
    for i in range(n): 
        if dist[i] != np.inf:
            M[s][i] = dist[i]
            M[i][s] = dist[i]
    #         # print(dist[s], dist[t], dist[i])
    #         # M[t][i] = dist[t] - dist[i]
            # M[i][t] = dist[t] - dist[i]


@njit(fastmath=True)
def dijkstra_heuristic(M, lattice, adj_list, s, t):
    n = len(lattice)
    d = 0
    current_vtx = s
    for i in range(n):
    

        if random.random() < 0.8:
            # Move towards t

        else:
            # Move randomly
        
       
  

@njit(fastmath=True, parallel=True)
def apsp(lattice):
    n = len(lattice)
    vertex_lookup = np.zeros((n, 4), dtype=np.int64)
    lattice_size = int(n**0.5)

    for u in range(n):
        x = [u-1,u+1,u-lattice_size,u+lattice_size]
        x = np.array(sorted([i for i in x if i >= 0 and i < n]))
        for j in range(len(x)):
            vertex_lookup[u][j] = int(x[j])
    
    # print(vertex_lookup)

    M = np.zeros((n, n)) - 1
    for i in nb.prange(n):
        for j in range(n):
            if i == j:
                M[i][j] = 0
            elif i > j and M[i][n - j - 1] == -1:
                dijkstra(M, lattice, vertex_lookup, i, n - 1 - j)


    return M

@njit(fastmath=True,parallel=True)
def apsp_long(lattice):
    n = len(lattice)
    adj_list = np.zeros((n, 4), dtype=np.int64)
    lattice_size = int(n**0.5)

    for u in range(n):
        x = [u-1,u+1,u-lattice_size,u+lattice_size]
        x = np.array(sorted([i for i in x if i >= 0 and i < n]))
        for j in range(len(x)):
            adj_list[u][j] = int(x[j])
    

    M = np.zeros((n, n)) - 1
    for i in nb.prange(n):
        for j in nb.prange(n):
            if i == j:
                M[i][j] = 0
            elif i > j and M[i][n - j - 1] == -1:
                s, t = i, n - 1 - j

                not_visited = [y for y in range(n)]
                dist = np.full(n, np.inf)
                dist[s] = 0
                for _ in range(n):
                    u = -1
                    for x in not_visited:
                        if u == -1 or dist[x] < dist[u]:
                            u = x

                    not_visited.remove(u)
                
                    if lattice[u][adj_list[u][0]] != 0:
                        dist[adj_list[u][0]] = min(dist[adj_list[u][0]], dist[u] + lattice[u][adj_list[u][0]])
                    if lattice[u][adj_list[u][1]] != 0:
                        dist[adj_list[u][1]] = min(dist[adj_list[u][1]], dist[u] + lattice[u][adj_list[u][1]])
                    if lattice[u][adj_list[u][2]] != 0:
                        dist[adj_list[u][2]] = min(dist[adj_list[u][2]], dist[u] + lattice[u][adj_list[u][2]])
                    if lattice[u][adj_list[u][3]] != 0:
                        dist[adj_list[u][3]] = min(dist[adj_list[u][3]], dist[u] + lattice[u][adj_list[u][3]])

                ##update M with the new distances
                for x in range(n): 
                    if dist[x] != np.inf:
                        M[s][x] = dist[x]
                        M[x][s] = dist[x]


    return M

@njit(fastmath=True, parallel=True)
def approx(lattice): 
    n = len(lattice)
    adj_list = np.zeros((n, 4), dtype=np.int64)
    lattice_size = int(n**0.5)
    small = int(lattice_size ** 0.5)

    for u in range(n):
        x = [u-1,u+1,u-lattice_size,u+lattice_size]
        x = np.array(sorted([i for i in x if i >= 0 and i < n]))
        for j in range(len(x)):
            adj_list[u][j] = int(x[j])
    M = np.zeros((n, n)) - 1
    for i in nb.prange(n): 
        for j in range(n): 
            if i != j and M[i][j] == -1: 
                x1 = i % lattice_size
                y1 = i // lattice_size
                x2 = j % lattice_size
                y2 = j // lattice_size
                d = abs(x1 - x2) + abs(y1 - y2)
                if d <= 2: 
                    dijkstra(M, lattice,adj_list, i, j) 
                else:
                    M[i][j] = round(5.5 * d)
    return M

def run_basic():
    n = 100
    lattice = gen_lattice(n)
    start = time.time()
    apsp(lattice)
    end = time.time()
    return end - start

def run_all():
    n = 50
    lattice = gen_lattice(n)

    # print(lattice)
    t1 = time.time()
    sci_matrix = csr_matrix(lattice)
    sci_paths = csgraph.shortest_path(sci_matrix, directed=False, method='auto')
    t2 = time.time()

    t3 = time.time()
    basic_matrix = apsp(lattice)
    t4 = time.time()

    for i in range(n):
        for j in range(n): 
            if basic_matrix[i][j] != sci_paths[i][j]:
                print(f'Error at {i}, {j}')
                print(f'Basic: {basic_matrix[i][j]} SciPy: {sci_paths[i][j]}')

    # t5 = time.time()
    # approx_matrix = approx(lattice)
    # t6 = time.time()

    # eps = 0.1
    # counter = 0

    # for i in range(n):
    #     for j in range(n): 
    #         if abs(approx_matrix[i][j] - sci_paths[i][j]) > eps * sci_paths[i][j]:
    #             counter += 1
                # print(f'Over {eps * 100}% out at {i}, {j}')
                # print(f'Basic: {approx_matrix[i][j]} SciPy: {sci_paths[i][j]}')


    # print(f"Perc. of paths over {eps * 100}% out: {100 * counter / (n**2)}")

    print(f'Scipy: {t2 - t1}')
    print(f'Basic: {t4 - t3}')
    # print(f'Approx: {t6 - t5}')



run_all()