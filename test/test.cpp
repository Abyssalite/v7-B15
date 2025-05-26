#include <iostream>
#include <bitset>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <sstream>
#include <optional>
#include <fstream>

using namespace std::chrono_literals;

std::string to_hex(const std::vector<unsigned char>& data) {
    std::ostringstream oss;
    for (unsigned char byte : data)
        oss << std::hex << std::setfill('0') << std::setw(2) << (int)byte;
    return oss.str();
}

std::optional<std::vector<unsigned char>> from_hex(const std::string& hex) {
    if (hex.length() % 2 != 0) return std::nullopt;

    std::vector<unsigned char> bytes;
    try {
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
            bytes.push_back(byte);
        }
    } catch (...) {
        return std::nullopt;
    }
    return bytes;
}
   
bool confirmation(){
    while (true){
		//s = arduino.readline().decode('utf-8')
        std::string s = "NO_ERROR\n";
        if (s == "NO_ERROR\n")
            return true;
        else return false;
	}
}

int parity(std::vector<unsigned char> block) { 
	int result = 0;
	for (unsigned char byte: block)
		result ^= int(byte);
	return result;
}

std::vector<std::vector<unsigned char>> splitBin(int size) {
	const std::size_t bufferSize = 64 * size;
	std::vector<std::vector<unsigned char>> blocks;

	std::vector<char> buffer(bufferSize);
	while (std::cin.read(buffer.data(), bufferSize) || std::cin.gcount() > 0) {
		blocks.emplace_back(buffer.begin(), buffer.begin() + std::cin.gcount());
	}
    return blocks;
}

void binTransfer(int size) {
	auto blocks = splitBin(size);
	int i = 0;
	std::string end = "";
	while(i < blocks.size()) {
		if(i == blocks.size() - 1) end = "END";

		std::string s = to_hex(blocks[i]);
		std::vector<unsigned char> encoded(s.begin(), s.end());
		int block_parity = parity(encoded);

		std::string data = s + '|' + std::to_string(block_parity) + '|' + end;
		std::cout << data + '\n';
		if(confirmation()) i += 1;
	}
}

void binReceive() {
	std::string end = "";
	std::string data = "";
	while(end != "END") {
		try {
			std::string s = "5e598c47f3c204084c70874c4ba95e5cc1b3d6e90e02c993591c27ce59f6f3110f0a5244d637f301ffb42b28b499c9099399cbe9f79033938214de1aac4c5e8a|94|END";

			size_t p1 = s.find('|');
			size_t p2 = s.rfind('|');	
			if (p1 == std::string::npos || p2 == std::string::npos || p1 == p2) {
        		throw std::logic_error("ERR");
			}
			std::string block = s.substr(0, p1);
			std::string block_parity = s.substr(p1 + 1, p2 - p1 - 1);
			end = s.substr(p2 + 1);
			std::vector<unsigned char> encoded(block.begin(), block.end());
			int new_parity = parity(encoded);
			//std::cout << block << block_parity << end << "\n";

			if(block_parity == std::to_string(new_parity)) {
				if (!from_hex(block)) {
        			throw std::logic_error("ERR");
				}	
				std::cout << "NO_ERROR\n";
				data.append(block);
			}
			else std::cout << "ERR\n";


		} catch(...) {
			std::cout << "ERR\n";
		}
	}
	auto out = from_hex(data).value();
	std::cout.write(reinterpret_cast<const char*>(out.data()), out.size());
    std::cout.flush();
}

void menu(char* argv[]) {
	    std::string choose = argv[1];

		if(choose == "1") {
            std::cout << "send(drv)\n";
		}
		else if(choose == "2") {
            std::cout << "receive(drv)\n";
		}

		else if(choose == "5") {
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

		else if(choose == "3") {
			try {
				if (argv[2]== nullptr){
					binTransfer(1);
				}
				else{
					std::string x = argv[2];
					if (x == "--size") {
						std::string size = argv[3];
						binTransfer(std::stoi(size));
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
		else if(choose == "4") {
			binReceive();

		}
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