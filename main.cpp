#include <iostream>
#include <b15f/b15f.h>
#include <bitset>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>

using namespace std::chrono_literals;

void send(B15F& drv) {
	drv.setRegister(&DDRA, 0b11000111);
	std::cin.clear();
	std::string text;
	
	std::cout << "Input text: ";
	std::getline(std::cin, text);
	text += '\n';

	for (char bin: text) {
		for (int i = 0; i < 3; i++) {    
		  	int part = (bin >> i*3) & 0b00000111;  
			drv.setRegister(&PORTA, part |= 0b11000000);
			
			std::this_thread::sleep_for(1us); //~60 real us
			drv.setRegister(&PORTA, part &= 0b10111111);
			std::this_thread::sleep_for(2us); //~60 real us
    			if (i >= 2) drv.setRegister(&PORTA, part &= 0b00111111);
		}	
	}
	drv.setRegister(&DDRA, 0b00000111);
}

void receive(B15F& drv) {
	int bin;
	std::cout << "Received text: ";
	do {
	        int n = 0;
	        bin = 0b00000000;
	        int tmp = 0b00000000;
	        bool lState = 0;
	        bool cState = 0;

	        while((drv.getRegister(&PINA) >> 7) & 0b00000001) {
		        if(n < 7) {
			        while((drv.getRegister(&PINA) >> 6) & 0b00000001) {          
		          		lState = (drv.getRegister(&PINA) >> 6) & 0b00000001;
				        std::this_thread::sleep_for(1us); //~60 real us
		          		tmp = (drv.getRegister(&PINA) & 0b00111000) >> 3;
		          		bin |= tmp << n;
			        }  
			        cState = (drv.getRegister(&PINA) >> 6) & 0b00000001;
			        
			        if (lState && !cState ){
		          		lState = 0;
		          		n += 3;
			        } 
	         	}       
	        }
	        std::cout << (char)bin;
	} while ((char)bin != '\n');
}

std::vector<std::vector<char>> splitBin(int size) {
	const std::size_t bufferSize = 1024 * size;
	std::vector<std::vector<char>> blocks;

	std::vector<char> buffer(bufferSize);
	while (std::cin.read(buffer.data(), bufferSize) || std::cin.gcount() > 0) {
		blocks.emplace_back(buffer.begin(), buffer.begin() + std::cin.gcount());
	}
	/*std::cout << "Block S " << blocks.size() <<std::endl;
	for(size_t i = 0; i < blocks.size(); i++){
		std::cout << "Block " << i << ' ';
		for(char byte: blocks[i])
			std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << ' ';
		std::cout << std::endl;
	}*/

    return blocks;
}

int parity(std::vector<char> block) { 
	int result = 0;
	for (char byte: block)
		result ^= int(byte);
	return result;
}

void menu(B15F& drv, char* argv[]) {
	        std::string choose = argv[1];

		if(choose == "1") {
			send(drv);
		}
		else if(choose == "2") {
			receive(drv);
		}
		else if(choose == "3") {
			try {
				if (argv[2]== nullptr){
					auto blocks = splitBin(1);
					for (std::vector<char> block: blocks)
						std::cout << parity(block) << std::endl;
				}
				else{
					std::string x = argv[2];
					if (x == "--size") {
						std::string size = argv[3];
						auto blocks = splitBin(std::stoi(size));
						for (std::vector<char> block: blocks)
							std::cout << parity(block) << std::endl;
					}
					else throw std::runtime_error("Argument not available");
				}

					
			} catch (const std::invalid_argument& e) {
				std::cout << "Size must be number\n";
			} catch (const std::logic_error& e) {
				std::cout << e.what() << ". Missing size!\n";
			} catch (const std::runtime_error& e) {
				std::cout << e.what() << "\n";
			}
		}
		else if(choose == "0") {}

		else{
			std::cout << "Function "<< choose << " not available.\n";
		}
}

int main(int argc, char* argv[]) {
	B15F& drv = B15F::getInstance();
	drv.setRegister(&DDRA, 0b00000111);
	if(argc > 1) {
		menu(drv, argv);
		return 0;	
	}
	else {
		std::cout << "No argument provided.\n";
		return 1;
	}
}
