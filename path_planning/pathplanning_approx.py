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
# Generates a random weight between 1 and 10
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

# Reconstructs the path between two nodes
# @njit(fastmath=True)
def reconstruct_path(predecessors, start, end):
    path = []

    point = end
    while point != start:
        path.append(int(point))
        point = int(predecessors[point])

    path.append(int(start))
    return path    

@njit(fastmath=True)
def safe_find_endpoints(num_endpoints, possible_endpoints, dists, all_endpoints, dist_matrix_endpoints, d):

    # new_endpoints = np.empty(0, dtype=np.int64)
    # source_and_endpoints = np.empty((0,2), dtype=np.int64)
    new_endpoints = np.zeros(num_endpoints, dtype=np.int64)
    source_and_endpoints = np.zeros((num_endpoints, 2), dtype=np.int64)
    approx = 1
    count = 0
    min_dist_between_endpoints = 8
    endpoints_found = 0

    # Filter dists such that any distances not within ) approx +- of d are set to infinity
    dists = np.where((dists > d + approx) | (dists < d - approx), np.inf, dists)

    # Filter posible endpoints to exclude any that have all distances to Z set to infinity
    possible_endpoints = np.array([x for x in possible_endpoints if not np.all(np.isinf(dists[:,x]))])
    l = len(possible_endpoints)
    s = np.size(all_endpoints)
    while endpoints_found < num_endpoints:


        # Randomly select an endpoint
        y = possible_endpoints[int(random.random() * l)]

        # # Find the distance to the nearest generator node
        # dist_to_endpoints = np.array([x[y] for x in dist_matrix_endpoints])
        dist_to_endpoints = dist_matrix_endpoints[:,y]

        # Check if the selected endpoint is not too close to another endpoints
        if (s == 0 or np.all(dist_to_endpoints > min_dist_between_endpoints)):
          
            # Select that generator node
            x = np.argmin(dists[:,y])

            new_point = np.array([[x, y]], dtype=np.int64)
            # new_endpoints = np.append(new_endpoints, y)
            # source_and_endpoints = np.append(source_and_endpoints, new_point, axis=0)
            new_endpoints[endpoints_found] = y
            source_and_endpoints[endpoints_found] = new_point
            endpoints_found += 1
        
        count += 1

        # If all possible endpoints have been checked, decrease the minimum distance between endpoints
        if (count == len(possible_endpoints)):
            min_dist_between_endpoints -= 8
            count = 0
    
    return source_and_endpoints, new_endpoints


# @njit(fastmath=True)  
def my_filter(dists, d):
    return np.where((dists > d + 1) | (dists < d - 1), np.inf, dists)

# @njit(fastmath=True)
def find_endpoints(num_endpoints, possible_endpoints, dists, all_endpoints, dist_matrix_endpoints, d):
    
    new_endpoints = np.zeros(num_endpoints, dtype=np.int64)
    source_and_endpoints = np.zeros((num_endpoints, 2), dtype=np.int64)
    # Filter dists such that any distances not within ) approx +- of d are set to infinity
    dists = my_filter(dists, d)
    # Filter posible endpoints to exclude any that have all distances to Z set to infinity
    possible_endpoints = np.array([x for x in possible_endpoints if not np.all(np.isinf(dists[:,x]))])
    l = len(possible_endpoints)
    if possible_endpoints.size == 0:
        return np.empty((0,2), dtype=np.int64), np.empty(0, dtype=np.int64)

    for i in nb.prange(num_endpoints):
        # Randomly select an endpoint
        y = possible_endpoints[int(random.random() * l)]
        # # Find the distance to the nearest generator node
        dist_to_endpoints = dist_matrix_endpoints[:,y]
        # Select that generator node
        x = np.argmin(dists[:,y])
        new_point = np.array([[x, y]], dtype=np.int64)
        new_endpoints[i] = y
        source_and_endpoints[i] = new_point

    return source_and_endpoints, new_endpoints

# Get paths for each pair of generator and endpoints
@njit(fastmath=True)
def get_paths(source_and_endpoints, Z, Z_predecessors, P, paths):
    new_nodes = []
    all_paths = []
    for point in source_and_endpoints:
        index = point[1]

        generator = Z[point[0]]
        predecessors = Z_predecessors[point[0]]
        
        path = reconstruct_path(predecessors, generator, index)
        all_paths.append(path)
        path = np.array(path, dtype=np.int64)
        P = np.concatenate((P, path), axis=0)
        new_nodes.extend(path)
        

    return all_paths, new_nodes, P

@njit(fastmath=True)
def approx(lattice, new_endpoints): 
    n = len(lattice)
    lattice_size = int(n**0.5)
    M = np.zeros((len(new_endpoints), n)) - 1
    e = -1
    for i in  new_endpoints:
        e += 1
        for j in range(n): 
            if i != j and M[e][j] == -1: 
                x1 = i % lattice_size
                y1 = i // lattice_size
                x2 = j % lattice_size
                y2 = j // lattice_size
                d = abs(x1 - x2) + abs(y1 - y2)
            
                M[e][j] = round(5.5 * d)
    return M


# @njit(fastmath=True)
def gen_rst(lattice, i):
    #Generate random spanning tree by simulating a random walk from vertex i
    #and keep track of which edges we see
    n = len(lattice)
    discovered = np.zeros(n)
    discovered[i] = 1
    current_vtx = i
    edges = []
    Z = np.zeros(n)

    while not all(discovered): 
        #Get the neighbors of the current vertex
  
        neighbors = np.nonzero(lattice[current_vtx])[0]
        #Choose a random neighbor
        random_neighbor = neighbors[int(random.random() * len(neighbors))]
        #If we haven't seen this neighbor before, add it to the tree
        if not discovered[random_neighbor]: 
            discovered[random_neighbor] = 1
            edges.append((current_vtx, random_neighbor))
            # Z[random_neighbor] = current_vtx
        current_vtx = random_neighbor

    ##Turn the edges into a tree (for the predecessor matrix)
    for edge in edges: 
        Z[edge[1]] = edge[0]

    return Z

# @njit(fastmath=True)
def approx_with_pred(lattice, new_nodes): 
    n = len(lattice)
    lattice_size = int(n**0.5)
    M = np.zeros((len(new_nodes), n)) - 1
    Z = np.zeros((len(new_nodes), n)) 
    e = -1
    for i in  new_nodes:
        e += 1
        for j in range(n): 
            if i != j and M[e][j] == -1: 
                x1 = i % lattice_size
                y1 = i // lattice_size
                x2 = j % lattice_size
                y2 = j // lattice_size
                d = abs(x1 - x2) + abs(y1 - y2)

                # r = random.random()
                # if r < 0.25:
                #     Z[e][j] = y2 * lattice_size + x2 - 1
                # elif r < 0.5:
                #     Z[e][j] = y2 * lattice_size + x2 + 1
                # elif r < 0.75:
                #     Z[e][j] = (y2 - 1) * lattice_size + x2
                # else:
                #     Z[e][j] = (y2 + 1) * lattice_size + x2

                # s = list("0" * abs(x1 - x2) + "1" * abs(y1 - y2)) 
                # random.shuffle(s)
                # x,y = x1, y1
                # for c in s: 
                #     if c == "0" and x1 < x2: 
                #         x += 1
                #         Z[e][y * lattice_size + x] = y * lattice_size + x - 1
             
                #     elif c == "0" and x1 > x2: 
                #         x -= 1
                #         Z[e][y * lattice_size + x] = y * lattice_size + x + 1
                    
                #     elif c == "1" and y1 < y2: 
                #         y += 1
                #         Z[e][y * lattice_size + x] = (y - 1) * lattice_size + x
                   
                #     elif c == "1" and y1 > y2: 
                #         y -= 1
                #         Z[e][y * lattice_size + x] = (y + 1) * lattice_size + x
            


                M[e][j] = round(5.5 * d)
        Z[e] = gen_rst(lattice, i)
    
    return M, Z

# Path planning algorithm
# @njit(fastmath=True)
def path_planning(lattice, Z, a, b, d, e, num_endpoints, n):

    # Convert lattice to a sparse matrix
    lattice = csr_matrix(lattice)
    my_lattice = lattice.toarray()

    print("Finding shortest paths...")

    paths = []
    paths.append(Z)
    Z = np.array(Z, dtype='int')
    P = np.array([], dtype='int')
    new_nodes = []
    new_endpoints = np.empty((0,1), dtype=int)
    all_endpoints = np.empty((0,1), dtype=int)
    dist_matrix_endpoints = np.empty((0, n*n), dtype=float)

    # Add the initial generator nodes to the connected component
    P = np.concatenate((P,Z), axis=0)

    # Find the shortest paths from the generator nodes to all other nodes
    dist_matrix, Z_predecessors = csgraph.shortest_path(lattice, indices = Z, directed=False, return_predecessors=True, method='auto')
    
    print("Finding endpoints...")
    possible_endpoints = np.array([*range(n*n)])
    # Keep expanding the dendrite until the distance is below the threshold
    endpoint_times = 0 
    rebuild_times = 0
    scipy_times = 0
    approx_times = 0
    while (d > e):
        print("Distance: ", d) 

        source_and_endpoints = np.empty((0,2), dtype=int)

        t1 = time.time()
        # Calculate the distances from all nodes to the new nodes on the connected component
        if (np.size(new_nodes) > 1):
            # dist_matrix_new, predecessors_new = csgraph.shortest_path(lattice, indices = new_nodes, directed=False, return_predecessors=True, method='auto')
            dist_matrix_new, predecessors_new = approx_with_pred(my_lattice, new_nodes)
            dist_matrix = np.concatenate((dist_matrix, dist_matrix_new), axis=0)
            Z_predecessors = np.concatenate((Z_predecessors, predecessors_new), axis=0)
        t2 = time.time()
        scipy_times += t2 - t1
        
        t1 = time.time()
        # Calculate the distances from all nodes to the new endpoints
        if (np.size(new_endpoints) > 0):
            # dist_matrix_endpoints_new= csgraph.shortest_path(lattice, indices = new_endpoints, directed=False, return_predecessors=False, method='D')
            dist_matrix_endpoints_new = approx(my_lattice, new_endpoints)
            dist_matrix_endpoints = np.concatenate((dist_matrix_endpoints, dist_matrix_endpoints_new), axis=0)
        t2 = time.time()
        approx_times += t2 - t1
       

        all_endpoints = np.append(all_endpoints, new_endpoints)
        possible_endpoints = np.setdiff1d(possible_endpoints, Z)
        new_nodes = []


        t1 = time.time()
        source_and_endpoints, new_endpoints = find_endpoints(num_endpoints, possible_endpoints, dist_matrix, all_endpoints, dist_matrix_endpoints, d);
        t2 = time.time()
        endpoint_times += t2 - t1

        t1 = time.time()
        for point in source_and_endpoints:
            index = point[1]

            generator = Z[point[0]]
            predecessors = Z_predecessors[point[0]]

            path = reconstruct_path(predecessors, generator, index)

            new_nodes.extend(path)
            P = np.concatenate((P, path), axis=0);
            paths.append(path)
            
        t2 = time.time()
        rebuild_times += t2 - t1
        # Determine the paths from the generator nodes to the new endpoints
        num_endpoints = num_endpoints * b
        d = d / a
        Z = P
    
    print("Scipy times: ", scipy_times)
    print("Approx times: ", approx_times)
    print("Endpoint times: ", endpoint_times)
    print("Rebuild times: ", rebuild_times)

        
    return paths


# Convert a node index to a coordinate on the grid
def get_coordinate(index, r, c):
    row = index // c
    col = index % c
    return col , row      

# Refine the path betweeen each pair of nodes on the path
def refine_path(path, n, iters):
    sub_n = 3
    size = (sub_n*2) * sub_n
    prev_coords = []
    
    for q in range(iters):
        coord_paths = []
        index_paths  = []

        for i in range(0, np.size(path)-1):
            node = path[i]

            next_node = path[i+1]
            if (node == next_node):
                continue
            
            # Determine the coordinates of the two nodes on the path
            if (q == 0):
                x,y = get_coordinate(node, n, n)
                s,t = get_coordinate(next_node, n, n)
            else:
                # If this path has been refined before, use the known coordinates
                x,y = prev_coords[i]
                s,t = prev_coords[i+1]

            coord_mapping = [[1,1],[1,1]]

            rows = sub_n
            cols = sub_n

            map_x = 0
            map_y = 0
            diff_x = 1
            diff_y = 1
            
            # Determine the direction of the path and thus the placement of the subgrid for the nodes
            if t < y or t > y:
                rows = sub_n*2
                if t < y:
                    coord_mapping[0][1] = 1
                    coord_mapping[1][1] = 1+sub_n
                    map_y = y
                    diff_y = y - t
                elif y < t:
                    coord_mapping[0][1] = 1+sub_n
                    coord_mapping[1][1] = 1
                    map_y = t
                    diff_y = t - y
                map_x = x
            
            if s < x or s > x:
                cols = sub_n*2
                if s < x:
                    coord_mapping[0][0] = 1+sub_n
                    coord_mapping[1][0] = 1
                    map_x = s
                    diff_x = x - s

                elif x < s:
                    coord_mapping[0][0] = 1
                    coord_mapping[1][0] = 1+sub_n
                    map_x = x
                    diff_x = s - x

                map_y = y

            size = rows*cols

            # Create a new subgrid for the nodes where each node is replaced by a subgrid of size sub_n x sub_n
            joined_lattice = np.zeros((size, size))
            for j in range(rows):
                for k in range(cols):
                    index = j*cols + k
                    if k > 0:
                        joined_lattice[index-1][index] = joined_lattice[index][index-1] = random_weight()
                    if j > 0:
                        joined_lattice[index - cols][index] = joined_lattice[index][index-cols] = random_weight()

            x,y = coord_mapping[0]
            s,t = coord_mapping[1]
            
            index1 = y*cols + x
            index2 = t*cols + s

            # On this subgrid, find the shortest path between the two nodes
            dist_matrix, predecessors = csgraph.shortest_path(csr_matrix(joined_lattice), directed=False, return_predecessors=True)

            # Replace the path between the two nodes with the shortest path on the subgrid
            new_path = reconstruct_path(predecessors[index2], index2, index1)

            coord_path = [get_coordinate(m, rows, cols) for m in new_path]

            new_coord_path = [(map_x + diff_x*((a - 1)/sub_n), map_y - diff_y*((b - 1)/sub_n)) for a,b in coord_path]

            coord_paths.extend(new_coord_path)

            new_index_path = [((d)*n) + c for c,d in new_coord_path]

            index_paths .extend(new_index_path)

        path = index_paths 
        prev_coords = coord_paths


    return coord_paths





        

def display_grid(n, paths, num_iters):

    grid = np.zeros((n,n))
    
    # Display the dendrite with path refinement

    plt.imshow(grid, cmap='Greys', extent=(0, n, 0, n), origin='upper')
    
    plt.xlim(-1, n)
    plt.ylim(-1, n)
    
    plt.gca().set_aspect('equal', adjustable='box')

    for path in paths:
        refined = refine_path(path, n, num_iters)

        for j in range(len(refined) - 1):
            x1, y1 = refined[j]
            x2, y2 = refined[j+1]
            plt.plot([x1 + 0.5,x2 + 0.5],[y1 + 0.5,y2 + 0.5], 'k-', linewidth=1)
    
    # plt.show()
    plt.savefig('dendrite.png', dpi=300)



def main():
    n = 50
    midpoint = n // 2
    init_num_endpoints = 7
    midpoint_index = midpoint * n + midpoint        
    lattice = gen_lattice(n)

    Z = [midpoint_index]
    a = 1.8
    b = 2
    d = 100
    e = 3
    iters = 2

    start=time.time()
    print("Planning paths...")

    output = path_planning(lattice, Z, a, b, d, e, init_num_endpoints, n)
    end=time.time()
    print("Time taken: ", end-start)
    print("Number of nodes in dendrite: ", len(output))
    print("Displaying dendrite...")


    display_grid(n, output, iters)

if __name__ == "__main__":
    main()