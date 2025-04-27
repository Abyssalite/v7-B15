#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <b15f/b15f.h>

using namespace std::chrono;
using namespace std::chrono_literals;
int main() {
	B15F& drv = B15F::getInstance();
	drv.setRegister(&DDRA, 0b00000111);
    	int rece = 0;
    	float duration = 0.0;

        auto start = high_resolution_clock::now();
        drv.setRegister(&PORTA, 0b00000001);
	while(rece == 0) {
		rece = (drv.getRegister(&PINA) >> 3) & 0b00000001;
	}

        auto end = high_resolution_clock::now();
        duration = (duration_cast<nanoseconds>(end - start).count()) / 1000.0;
	std::cout << "B15 response time: " <<  duration << " us" << std::endl;

        auto start1 = high_resolution_clock::now();
	std::this_thread::sleep_for(1us);
        auto end1 = high_resolution_clock::now();
        duration = (duration_cast<nanoseconds>(end1 - start1).count()) / 1000.0;
	std::cout << "PC response time: " <<  duration << " us" << std::endl;

	return 0;
}
