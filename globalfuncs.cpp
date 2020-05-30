    
#include "globalfuncs.hpp"
#include<random>
#include<mutex>
#include<utility>
#include<cmath>

using namespace std;


struct data{
	double x;
	double y;
};


bool isfinished(){		//for main to signal to workers to stop
	unique_lock<mutex> mylock(finishedmtx);	//important: cannot use lock guard with cv bc we'll be locking.unlocking lots of times
	if (finished){
		return true;
	}else{return false;}
}
void setfinished(){
	unique_lock<mutex> mylock(finishedmtx);	
	finished = true;
}

int i_rand_generator(int lower, int upper){
	try{
		random_device rd;
		mt19937 eng(rd());//seed
		uniform_real_distribution<double> distr(lower,upper);
		int result = distr(eng);
		return result;			
	}catch(...){
		cout<<"problem in rand_generator"<<endl;
		return -INFINITY;
	}
} 

pair<double, vector<int>> splice(pair<double,vector<int>> parent1 ,pair<double,vector<int>> parent2){
	size_t parentsize = parent1.second.size();
	pair<double, vector<int>> offspring;
	size_t s = i_rand_generator(1,parentsize);		//splicepoint [1,parentsize)
	copy(parent1.second.begin(),parent1.second.begin()+s,back_inserter(offspring.second));
	copy(parent2.second.begin()+s,parent2.second.end(),back_inserter(offspring.second));
	return offspring;
}



pair<int, int> getendpt(Maze m, vector<int> genome, int r, int c, int& hit_wall){	
	pair<int,int> mazestartpt,mazeendpt;
	mazestartpt.first = m.getStart().row;		
	mazeendpt.first = m.getFinish().row;		
	mazeendpt.second = m.getFinish().col;

	pair<int,int> endpt, trypt;
	trypt = mazestartpt;
 
	for (int i =0; i<genome.size();i++){	//for every step within the genome....
		if (genome[i]==1){
			trypt.second--;
			if (trypt.second<1 || m.get(trypt.second,trypt.first)){	//y-value cannot be 0 or -ve
				hit_wall++;
				trypt.second++;		//resetting (i.e. doing nothing at this step)
			}
		}else if (genome[i] ==2){
			trypt.second++;	
			if (trypt.second>= r ||m.get(trypt.second,trypt.first)){	//y-value cannot r or more
				hit_wall++;
				trypt.second--;		//resetting 
			}
		}else if (genome[i] ==3){
			trypt.first--;
			if (trypt.first <1 || m.get(trypt.second,trypt.first)){	//x-value cannot be 0 or -ve
				hit_wall++;
				trypt.first++;		//resetting
			}
		} else if (genome[i] ==4){
			trypt.first ++;
			if (trypt.first>= c ||m.get(trypt.second,trypt.first)){	//x-value cannot be c or more
				hit_wall++;
				trypt.first--;		//resetting
			}
		}
	}
	endpt = trypt;
	return endpt;
}


double getfit(Maze m,vector<int> genome, int r, int c){
	pair<int,int> mazestartpt, mazeendpt;
	mazeendpt.first = m.getFinish().row;		
	mazeendpt.second = m.getFinish().col;
	int hit_wall = 0;

	pair<int,int> endpt = getendpt(m,genome,r,c,hit_wall);
	double fit, taxicab;

	taxicab = abs((endpt.first - mazeendpt.first)+(endpt.second-mazeendpt.second));
	fit = 2*taxicab + hit_wall;
	return fit;

}

