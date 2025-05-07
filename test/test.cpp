#include <iostream>
#include <bitset>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>

using namespace std::chrono_literals;



std::vector<std::vector<unsigned char>> splitBin(int size) {
	const std::size_t bufferSize = 1024 * size;
	std::vector<std::vector<unsigned char>> blocks;

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

int parity(std::vector<unsigned char> block) { 
	int result = 0;
	for (unsigned char byte: block)
		result ^= int(byte);
	return result;
}

void menu(char* argv[]) {
	        std::string choose = argv[1];

		if(choose == "1") {
            std::cout << "send(drv)\n";
		}
		else if(choose == "2") {
            std::cout << "receive(drv)\n";
		}
		else if(choose == "3") {
			try {
				if (argv[2]== nullptr){
					auto blocks = splitBin(1);
					for (std::vector<unsigned char> block: blocks)
						std::cout << parity(block) << std::endl;
				}
				else{
					std::string x = argv[2];
					if (x == "--size") {
						std::string size = argv[3];
						auto blocks = splitBin(std::stoi(size));
						for (std::vector<unsigned char> block: blocks)
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
	if(argc > 1) {
		menu(argv);
		return 0;	
	}
	else {
		std::cout << "No argument provided.\n";
		return 1;
	}
}