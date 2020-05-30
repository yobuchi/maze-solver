#include<iostream>
#include<thread>
#include<random>
#include<vector>
#include<mutex>
#include<chrono>
#include<ctime>
#include<utility>
#include<cstdint>
#include "ThreadSafeQ.hpp"
#include "ThreadSafeMultimap.hpp"
#include "globalfuncs.hpp"
#include "maze_1.hpp"

using namespace std;
ThreadSafeMultimap<double, vector<int>> population;
ThreadSafeQ<pair <double,vector<int>>> q2;
int r,c;	//row, col of maze
uint64_t futility;
mutex futmut, tbest_mut;
chrono::steady_clock::time_point t_best_found;
int nummix, nummut;
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//startmix method -- for mixer threads

void startmix(){
	cout<<"spawned a mixer thread!"<<endl;

	while(!isfinished()){
		//get 2 random items off Population multimap <-- the top 2 have best fit, so we should mutate based on them
		pair parent1 = population[0];
		pair parent2 = population[1];

		pair offspring = splice(parent1,parent2);			//call splice() function, which returns an offspring

		q2.push(offspring);									//push offspring into Offspring queue
	}
	//even after it's finished, should still push several elements in, just to free up any mutator threads stuck listening() on a potentially empty queue
	if (q2.size()<nummut){
		for (int i = 0; i<nummut;i++){
			pair temp =population[0];
			q2.push(temp);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//startmut method -- for mutator threads
void startmut(Maze m, int t, int g, int r, int c, chrono::steady_clock::time_point t_start ){
	cout<<"spawned a mutator thread!"<<endl;
	while(!isfinished()){
		pair best = population[0];				//locally store the fit of row 1 in population (best fit)			
		int best_start = best.first;	

		pair<double,vector<int>> o;				//listen(o) on Offspring
		bool b = false;
		if (!isfinished()){
			b = q2.listen(o);
		}
		bool do_mutate = (rand()%100)<40;
		if (do_mutate){				//do the mutation -- just modify one random element to new val (0-4)
			int r1 = i_rand_generator(0,o.second.size());	//an element at random position r1
			int r2 = i_rand_generator(0,5);					//to be modified to random value 
			o.second.at(r1) = r2;
		}
		o.first = getfit(m,o.second,r,c);			//compute fitness
		population.push(o);

		size_t limit = 4*t;							//check if multimap is getting too big - if yes, truncate
		if (population.size()>limit){population.truncate(limit);}

		pair newbest = population[0];				//check if fitness of the first row has improved

		if (newbest.first ==0){
			setfinished();
			cout<<"most efficient solution found: solved maze & no wall hits"<<endl;
			//track time to best solution
			tbest_mut.lock();
			t_best_found = chrono::steady_clock::now();
			tbest_mut.unlock();
			cout<<"trying to return"<<endl;
			break;
		}
		if (newbest.first<best_start){
			cout<<"found new bestfit:"<<newbest.first<<endl;
		}

		if(newbest.first<best_start){
			futmut.lock();
			futility=0;
			futmut.unlock();
		}else{
			futmut.lock();
			futility++;
			futmut.unlock();
		}
		
		//check value of futility 
		futmut.lock();
		if (futility>g){
			if (isfinished()){return;}		//dont run abort() sequence if you've found best_fit already
			chrono::steady_clock::time_point t_end = chrono::steady_clock::now();
			setfinished();
			futmut.unlock();
			cout<<"----------------------futility_reached------------------------"<<endl;	
			cout<<"futility count: "<<g<<endl;
			cout<<m;
			cout<<endl;
			cout<<"the best genome is: "<<endl;
			pair <double, vector<int>> bestg;
			bestg = population[0];
			for(auto i = bestg.second.begin(); i !=bestg.second.end(); i++){
				cout<<*i<<"|";
			}
			cout<<endl;
			cout<<"the best genome has fit:"<<bestg.first<<endl;

			cout<<endl;
			cout<<"----------------------runtimes--------------------------------"<<endl;	
			cout<<"total runtime (milli-sec): "<<chrono::duration_cast<chrono::milliseconds> (t_end- t_start).count()<<endl;	
			abort();
		}else{
			futmut.unlock();
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Main
int main(int argc, const char *argv[]){
	chrono::steady_clock::time_point t_start = chrono::steady_clock::now();
	int t = stoi(argv[1]);			//t = # of worker threads
	int g = stoi(argv[2]);			//g = threshold, if exceed, terminate prog
	r = stoi(argv[3]);			//r = rows
	c = stoi(argv[4]);			//c = cols
	int gl = stoi(argv[5]);			//gl = genome length

	vector<int> orig_genome_holder; 
	cout<<"----------------------iniital setup--------------------------------"<<endl;	
	int n = 4*t;
	cout<<"generate "<<n<<" genomes"<<endl;
	vector<thread> threads(t);		//* giving size t is important!


	//generate the maze ----------------------------------------------------------------
	random_device rd;
	mt19937 eng(rd());					//seed
	std::srand(std::time(0));			
	Maze m(r, c);
	std::cout << m;

	//fill Population with n=4*num_threads initial genomes ------------------------------------
	for (int i = 0; i <n; i++){			//for the num of genomes
		for (int j=0; j< gl; j++){		//make each genome based on length
			int k = i_rand_generator(0,5);
			orig_genome_holder.push_back(k);
		}

		//insert that pair (fit, genome) into map
		pair <double, vector<int>> x;
		x.second = orig_genome_holder;
		x.first = getfit(m,orig_genome_holder,r,c);

		cout<<"initial genome:"<<endl;
		for(auto i = x.second.begin(); i !=x.second.end(); i++){
			cout<<*i<<"|";
		}	

		population.push(x);
		orig_genome_holder.clear();			//clear orig_genome_holder. now, loop to make a new one
	}
	cout<<endl;
	cout<<"filled Population with "<<population.size()<<" genomes"<<endl;
	if (t>2){
		cout<<"t is: "<<t<<endl;
		nummix = (t/5)+1;
		nummut = t-nummix;
	}else{
		nummix = 1;
		nummut = 1;
	}
	cout<<"spawn "<<nummix<<" mixer threads"<<endl;
	cout<<"spawn "<<nummut<<" mutator threads"<<endl;
	vector<thread> threadsmix(nummix);
	vector<thread>threadsmut(nummut);
	// spawn mixer threads -------------------------------------------
	for (int i = 0; i<nummix; i++){
		threadsmix.at(i) = thread(startmix);
	}
	//spawn mutator threads-------------------------------------------
	for (int i = 0; i<nummut; i++){
		threadsmut.at(i) = thread(startmut,m,nummut,g,r,c, t_start);
	}

	// join threads ---------------------------------------------------
	for (int i = 0; i<nummix; i++){
		if (threadsmix.at(i).joinable()){
			threadsmix.at(i).join();
		}else{
			cout<<"failed to join a a mixer thread!"<<endl;
		}
	}

	for (int i = 0; i<nummut; i++){
		if (threadsmut.at(i).joinable()){
			threadsmut.at(i).join();
		}else{
			cout<<"failed to join a a mutator thread!"<<endl;
		}
	}
	chrono::steady_clock::time_point t_end = chrono::steady_clock::now();

	cout<<"----------------------best_found--------------------------------"<<endl;	//runs if you don't reach futility>g (i.e. not run abort())
	cout<<m;
	cout<<endl;
	cout<<"the best genome is: "<<endl;
	pair <double, vector<int>> bestg;
	bestg = population[0];
	for(auto i = bestg.second.begin(); i !=bestg.second.end(); i++){
		cout<<*i<<"|";
	}
	cout<<endl;
	cout<<"the best genome has fit:"<<bestg.first<<endl;

	cout<<endl;
	cout<<"----------------------runtimes--------------------------------"<<endl;	
	cout<<"total runtime (milli-sec): "<<chrono::duration_cast<chrono::milliseconds> (t_end- t_start).count()<<endl;	
	cout<<"time to best solution (milli-sec): "<<chrono::duration_cast<chrono::milliseconds> (t_best_found- t_start).count()<<endl;		

	return 0;
}