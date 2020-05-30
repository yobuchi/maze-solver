#include "ThreadSafeMultimap.hpp"
#include<map>
#include<shared_mutex>
#include<utility>


using namespace std;

template<typename K, typename V>
void ThreadSafeMultimap<K,V>::truncate(int n){
	unique_lock<shared_mutex> lk(rwlock);
	//erase based on range
	typename multimap<K,V>::iterator it;
	it = mymap.begin();
	for (int i =0; i<n; i++){	//increment iterator n-1 times
		it++;
	}
	mymap.erase(it, mymap.end());
}



template<typename K, typename V>
pair<K,V> ThreadSafeMultimap<K,V>::operator[](int i){				
	shared_lock<shared_mutex> lk(rwlock);
	//if out of bounds, return the last element of map
	if (i >= mymap.size()){
		cout<<"index out of bound, returning the last element of map"<<endl;
		typename multimap<K,V>::iterator it;
		it = mymap.end();
		-- it;
		return *it;
	}
	//otherwise, return element at index i
	int counter = 0;
	typename multimap<K,V>::iterator it2;
	for (auto it2 = mymap.begin(); it2 !=mymap.end(); it2 ++){
		if (counter == i){
			return *it2;
		}
		counter ++;
	}
	//just to avoid control reaching end of non-void function
	typename multimap<K,V>::iterator it3;
	it3 = mymap.end();
	-- it3;
	return *it3;
}

template<typename K, typename V>
bool ThreadSafeMultimap<K,V>::push(const pair<K,V>& elem){
	try{
		unique_lock<shared_mutex> lk(rwlock);		
		mymap.insert(elem);
		return true;
	}catch(...){
		return false;
	}
}

template<typename K, typename V>
bool ThreadSafeMultimap<K,V>::pop(pair<K,V>& elem){
	try{
		unique_lock<shared_mutex> lk(rwlock);	
		if (mymap.empty()){
			return false;
		}

		//store the last element
		typename multimap<K,V>::iterator it;
		it = mymap.end();
		--it;			//this is v. important!!! otherwise, segfault.
		elem = *it;

		// pop the last element
		mymap.erase(it);

		return true;
	}catch(...){
		return false;
	}
}


///////////additional helper methods/////////////////
template<typename K, typename V>
void ThreadSafeMultimap<K,V>::printmap(){

	for (typename multimap<K,V>::iterator it = mymap.begin(); it!=mymap.end(); it++){
		cout << *it <<endl;
	}
}

template<typename K, typename V>
size_t ThreadSafeMultimap<K,V>::size(){
	return mymap.size();
}

