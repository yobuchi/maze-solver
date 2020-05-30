#ifndef THREADSAFEMULTIMAP_HPP
#define THREADSAFEMULTIMAP_HPP
#include<map>
#include<shared_mutex>
#include<utility>

using namespace std;


template<typename K, typename V>
class ThreadSafeMultimap{
	private:
		multimap<K,V> mymap;
		shared_mutex rwlock;
	public:
		pair<K,V> operator [] (int);	//overloading
		bool push(const pair<K,V>&); 	//w-lock
		bool pop(pair<K,V>&);			//w-lock
		void truncate(int);				//w-lock

		//additional helper methods
		void printmap();				//r-lock
		size_t size();					//r-lock
};

#include "ThreadSafeMultimap.cpp"
#endif