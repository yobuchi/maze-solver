output: driver.o
	g++ -g --std=c++17 driver.o -o output

driver.o: driver.cpp
	g++ -g --std=c++17 -c driver.cpp

clean:
	rm *.o output