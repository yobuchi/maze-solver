#ifndef THREADSAFEQ_HPP
#define THREADSAFEQ_HPP
#include<list>
#include<mutex>
#include<chrono>
#include <condition_variable>


using namespace std;


template<typename T>
class ThreadSafeQ{
	private:
		mutex m;
		condition_variable cv;
		list<T> mylist;
	public:
		bool push(const T);
		bool pop(T&);
		bool listen(T&);
		//additional helper methods
		void printq();
		int size();
		void sort();
		void findmedian(T&);
		void findmax(T&);
		void findmin(T&);
		void findmean(T&);

};

#include "ThreadSafeQ.cpp"
#endif