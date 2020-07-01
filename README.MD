
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

## Optimizations 

Opt1: if multi map is getting too big, truncate so that it’s no bigger than 4* total_number_of_threads — to find a balance between number of new candidate genomes and attempts to improve existing genomes 

Opt2: push several elements from multi map into q even after setfinish() sequence is run — to free up any mutator threads stuck listening() on a potentially empty sequence 

Opt3: if the program hasn’t found a solution after a user-specified number of mutations, terminate the program with the “best” genome (closest to finishing & with fewest wall-hits) — to ensure completion

# A note on another attempted but abandoned optimization

A direction-guessing mutation has been considered (i.e. mutate genome based on whether the previous mutation in that direction—up/down/left/right/static—yielded a better fit). 
However, this idea was abandoned due to the danger of “overshooting” – e.g. even if walking a certain direction gave an improved fit once, does not mean that it would be good again next time. Imagine an “ideal” downward movement of 3. We started with moving down 4. This gave a better fit. However, if we continue to decrease based on “past experience”, we would actually be moving away from our “ideal” number of down 3 steps. 
We could certainly write a prediction algorithm, but a more complex algorithm could increase computation time – especially if we are doing many mutations, computation time could rise significantly, nullifying our original intent of faster computation. 
