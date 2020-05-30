#ifndef GLOBALFUNCS_HPP
#define GLOBALFUNCS_HPP

#include "maze_1.hpp"
#include<vector>
#include<utility>

using namespace std;
bool finished = false;
mutex finishedmtx, coeffmtx;
struct data;
struct item;
bool isfinished();
void setfinished();
int i_rand_generator(int, int);
pair<double, vector<int>> splice(pair<double,vector<int>>,pair<double,vector<int>>);
pair<int, int> getendpt(Maze, vector<int> , int, int,int&);
double getfit(Maze, vector<int>,int,int);



#include "globalfuncs.cpp"
#endif
