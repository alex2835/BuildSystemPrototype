#pragma once 

#include <functional>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <iostream>

struct Target
{
	size_t mId;
	std::function<void()> mTask;
};

inline Target LoadTestTarget( size_t id )
{
	static bool init_rand = [](){ 
		srand( (unsigned int)time( 0 ) ); 
		return true; 
	}();
	return Target{ id, [=](){
		std::cout << "Complite " << id << std::endl;
		std::this_thread::sleep_for( std::chrono::seconds( rand() % 3 ) );
	} };
}
