#include <iostream>
#include <b15f/b15f.h>
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

void send(B15F& drv, std::string text) {
	drv.setRegister(&DDRA, 0b11000111);

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

std::string receive(B15F& drv) {
	int bin;
	std::string text = "";
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
	        text += (char)bin;
	} while ((char)bin != '\n');

	return text;
}

bool confirmation(B15F& drv){
	std::string s = receive(drv);
	if (s == "NE\n")
		return true;
	else return false;
}

int parity(std::vector<unsigned char> block) { 
	int result = 0;
	for (unsigned char byte: block)
		result ^= int(byte);
	return result;
}

std::vector<std::vector<unsigned char>> splitBin(int size) {
	const std::size_t bufferSize = 128 * size;
	std::vector<std::vector<unsigned char>> blocks;

	std::vector<char> buffer(bufferSize);
	while (std::cin.read(buffer.data(), bufferSize) || std::cin.gcount() > 0) {
		blocks.emplace_back(buffer.begin(), buffer.begin() + std::cin.gcount());
	}
    return blocks;
}

void binTransfer(B15F& drv, int size) {
	auto blocks = splitBin(size);
	std::size_t i = 0;
	std::string end = "";
	while(i < blocks.size()) {
		end = (i == blocks.size() - 1)?  "END" : std::to_string(i);
		

		std::string s = to_hex(blocks[i]);
		std::vector<unsigned char> encoded(s.begin(), s.end());
		int block_parity = parity(encoded);

		std::string data = s + '|' + std::to_string(block_parity) + '|' + end;
		std::cout << data;
		send(drv, data + '\n');
		if(confirmation(drv)) {
			i+=1;
			std::cout << "NE";
		} else 	std::cout << "ER";
	}
}

void menu(B15F& drv, char* argv[]) {
	        std::string choose = argv[1];

		if(choose == "1") {
			std::cin.clear();
			std::string text;
			
			std::getline(std::cin, text);
			text += '\n';
			send(drv, text);
		}
		else if(choose == "2") {
			std::cout << receive(drv);
		}
		else if(choose == "3") {
			try {
				if (argv[2]== nullptr){
					binTransfer(drv, 1);
				}
				else{
					std::string x = argv[2];
					if (x == "--size") {
						std::string size = argv[3];
						binTransfer(drv, std::stoi(size));
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
			//binReceive();

		}
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
