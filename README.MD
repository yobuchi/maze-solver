
## Purpose
This project is an exploration of using a multi-threaded genetic algorithm to solve mazes of different sizes
I'm interested in finding the most efficient way to solve a maze & exploring the relationships between genome length and maze size

Genome = solution path containing (up, down, left, right) directions

## How to compile and test/run

A Makefile is provided. Compile with: make

after compiling, run with:./output **num_threads futility_threshold maze_row maze_col genome_length**

futility_threshold = program terminates after it has failed to make any progress for this many generations


e.g.:./output 4 50000 11 11 100

would spawn 4 threads(in total), have a futility counter of 50000, have a 11x11 maze, and genomes of length 100


## Performance report
Let m = maze size, gl = genome length, t_best = time to best genome

Finding 1: smaller m gives smaller t_best (i.e. solving a smaller maze is faster), because smaller mazes are easier to solve. (e.g. 7x7 maze has an average t_best of 39ms, whereas 11x11 maze takes 1255ms).

Finding 2: increasing gl could negatively impact smaller mazes, but benefit larger mazes. For smaller mazes, a shorter gl could have solved the maze already, so walking a long path wastes time (e.g. 7x7 solves faster with gl 20 than with gl 100). Larger mazes however require longer paths (e.g. it is very difficult to solve a 21x21 maze with just a gl of 20).

Note: results based on running with futility count of 50k
