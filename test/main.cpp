#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <b15f/b15f.h>

using namespace std::chrono;
using namespace std::chrono_literals;
int main() {
    int rece = 0;
    float duration[3] = {0.0, 0.0, 0.0};

    auto startPC = high_resolution_clock::now();
    std::this_thread::sleep_for(1us);
    auto endPC = high_resolution_clock::now();
    duration[0] = (duration_cast<nanoseconds>(endPC - startPC).count()) / 1000.0;
    std::cout << "PC response time: " <<  duration[0] << " us" << std::endl;
	
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0b00000111);
    auto start2Way = high_resolution_clock::now();
    drv.setRegister(&PORTA, 0b00000001);
    auto startReceive = high_resolution_clock::now();

    while(rece == 0) {
	rece = (drv.getRegister(&PINA) >> 3) & 0b00000001;
    }
    auto end = high_resolution_clock::now();
	
    duration[1] = (duration_cast<nanoseconds>(end - start2Way).count()) / 1000.0;
    std::cout << "B15 response time 2 way: " <<  duration[1] << " us" << std::endl;
    duration[2] = (duration_cast<nanoseconds>(end - startReceive).count()) / 1000.0;
    std::cout << "B15 response time receive: " <<  duration[2] << " us" << std::endl;
    std::cout << "B15 response time send: " <<  duration[1] - duration[2] << " us" << std::endl;

    return 0;
}
