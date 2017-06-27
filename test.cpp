#include <iostream>
#include "run-and-forget.hpp"
#include <unistd.h>


int main() {
	ThreadsOwner owner;
	owner.run([]{
		std::cout << "hello\n";
	});
	sleep(1);
	owner.run([]{
		sleep(1);
		std::cout << "hello2\n";
	});
	
}