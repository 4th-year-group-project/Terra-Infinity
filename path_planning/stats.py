import random as r
import numpy as np
import matplotlib.pyplot as plt 
import itertools as it 



weight_range = [i for i in range(1, 11)]



def simulate(n_trials, path_length): 
    path_sums = []
    for i in range(n_trials): 
        path = [r.choice(weight_range) for _ in range(path_length)]
        path_sums.append(sum(path))
    return path_sums

def stats(path_sums, eps=0.05): 
    print(f"Mean {np.mean(path_sums)}, Std Dev. {np.std(path_sums)}")

    mean = np.mean(path_sums)
    counter = 0

    for path_sum in path_sums:
        if abs(path_sum - mean) < eps * mean: 
            counter += 1
    
    print(f"Percentage of paths within {100 * eps} of the mean: {100 * counter / len(path_sums)}")


       

path_sums = simulate(10000, 500)

stats(path_sums)