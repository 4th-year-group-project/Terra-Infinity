// A sequential DLA simulation in C 
//Uses approximate techniques


#include <set>
#include <random> // Include the <random> header
#include <iostream>
#include <list>
#include <utility>
using namespace std;


const int n = 10; // do not put above 10!!
const int grid_size = 1024;
const int n_particles = 100000;

int check_adjacent1(int grid[grid_size][grid_size], int x, int y) {
    // See if any of the 8 adjacent cells are occupied
 
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (grid[(x + i) % grid_size][ (y + j) % grid_size] == 1) {
                return 1;
            }
        }
    }
    return 0;
}

int compute_grid(int grid[grid_size][grid_size], int n_particles, int arrivals[grid_size][2]) {
    //setup 
    list<pair<int, int> > particle_tracker;
    for (int i = 0; i < n_particles; i++) { 
        // Generate random numbers within the desired range
        int x = rand() % grid_size;
        int y = rand() % grid_size;

        particle_tracker.push_back(make_pair(x, y));
    }

    int i = 0;
    while (particle_tracker.size() > 0) {

        list<pair<int, int> > new_particle_tracker;

         for (auto &[x, y] : particle_tracker) {

          

            if (check_adjacent1(grid, x, y)) {
                // Fill in the particle
                grid[x][y] = 1;
                arrivals[i][0] = x;
                arrivals[i][1] = y;
   
                
                i++;
                if (i % 100 == 0) {
                    cout << i << endl;
                }
            }
            else {
                // Randomly move 
                if ((rand() % 10) < 5) {
                    if ((rand() % 10) < 5) {
                        x++;
                        if (x == grid_size) x = grid_size - 1; 
                    }
                    else {
                        x--;
                        if (x < 0) x = 0;
                    }
                }
                else {
                    if ((rand() % 10) < 5) {
                        y++;
                        if (y == grid_size) y = grid_size - 1;
                    }
                    else {
                        y--;
                        if (y < 0) y = 0;
                    }
                }
    
               
                new_particle_tracker.push_back(make_pair(x, y));
            }
          
            }

        //Save the new particle tracker to the old one
        particle_tracker = new_particle_tracker;


    
   
}
 return 0;
}
                

int main() {
    //Initialise the grid
    int grid[grid_size][grid_size] = {{0}};

    //Initialise the seed
    grid[grid_size / 2][grid_size / 2] = 1;

    //Store the particles in order of arrival
    int arrivals[n_particles][2] = {{0}};

    compute_grid(grid, n_particles, arrivals);

    //Write arrivals to file
    FILE *f = fopen("positions.txt", "w");
    for (int i = 0; i < n_particles; i++) {
        fprintf(f, "%d %d\n", arrivals[i][0], arrivals[i][1]);
    }

    return 0;
}