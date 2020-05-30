#include "ThreadSafeQ.hpp"
#include <iostream>
#include <list>
#include <mutex>
#include<chrono>
#include <condition_variable>


using namespace std;

template<typename T>
bool ThreadSafeQ<T>::push(const T elem){
	try{
		unique_lock<mutex> mylock(m);
		mylist.push_front(elem);
		cv.notify_one();				//wakes up 1 waiting thread that Q isn't empty now
		return true;
	}catch(...){
		return false;
	}
}


template<typename T>
bool ThreadSafeQ<T>::listen(T& elem){
	try{
		unique_lock<mutex> mylock(m);	//important: cannot use lock guard with cv bc we'll be locking.unlocking lots of times
		while(mylist.empty()){			//note: use while() instead of if ()
			cv.wait(mylock);
		}
		elem = mylist.back();			//have to first store, then pop, bc pop_back() is void return type
		mylist.pop_back();
		return true;
	}catch(...){
		return false;
	}

}


template<typename T>
bool ThreadSafeQ<T>::pop(T& elem){
	try{
		unique_lock<mutex>mylock(m);
		if (mylist.empty()){
			return false;
		}
		elem = mylist.back();
		mylist.pop_back();
		mylock.unlock();
		return true;
	}catch(...){
		return false;
	}
}


///////////additional helper methods/////////////////
template<typename T>
void ThreadSafeQ<T>::printq(){

	for (typename list<T>::iterator it = mylist.begin(); it!=mylist.end(); it++){
		cout << *it <<endl;
	}
}

template<typename T>
int ThreadSafeQ<T>::size(){
	return mylist.size();
}

template<typename T>
void ThreadSafeQ<T>::sort(){
	mylist.sort();
}


template<typename T>
void ThreadSafeQ<T>::findmedian(T& median){		//assume sorted by the time you call these methods
	int n = mylist.size();
	if (n %2 !=0){
		list<time_t>::iterator it = mylist.begin();
		advance(it,n/2);
	}
	list<time_t>::iterator it = mylist.begin();
	advance(it,(n-1)/2);
	time_t first = *it;
	advance(it,1);
	time_t second =*it;
	median = (time_t)(first+second)/2.0;
}

template<typename T>
void ThreadSafeQ<T>::findmax(T& max){
	max = mylist.back();
}
template<typename T>
void ThreadSafeQ<T>::findmin(T& min){
	min = mylist.front();
}

template<typename T>
void ThreadSafeQ<T>::findmean(T& mean){
	T max, min;
	findmax(max);
	findmin(min);
	mean =(max+min)/2.0;
}