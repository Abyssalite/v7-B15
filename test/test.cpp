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
	const std::size_t bufferSize = 1024 * size;
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
			std::string s = "33a2875ea602a96640e05fbc158e75583090c45d18835fc838853d55997ca3a4489a39cf2d4e53a61126d4179c3c07d9c5459f3b3dd0b1fe39bc25d154d692a86b805610e94446a8710ba20c4f7f1f10a85fcb6592c8c5f7fca3a8dcbb4e5cd8cf530c2f09a382c9808d3b40a4a775940dc73d8d0d289e3af24ec37870644e6575d57d8352196f20a3eb4f4c2f707d1b05001015ce66af9f82e430854ec5cacd19af8ea485f977e9314afd4e39d5e05c667377a13dc7d09f7e36a4571f02b777eefe4581ea79c2f054b79c0cfb77c031411b3c385b490e2f42828f9786313eecf8caefe0edab47cf1deb77c62a5b83cf48b64c7cd8636d9cf1284d809811c65ef9f38d403834dfca38ea321f0dadd66456081d0bc07cb6cc4b9a165f6c0afa71251607caea04b98ce0531543b800e8818a65dd81f722b5e88df3f50497f4f07c9235e887e2f933a489a9216aed08ebb94ca23d0ba3a2ac88aa6249e8d8477b215c38208f26f77eade0bd828e42368e0feda611cfb7ab91fab6c5579498c27fbdaecf0cca7d8fee0d0b3befd5c4d5c4360df0d90e2e3ffc45f805d15f9ed8e5892e281264967ff4ca04e1964ea138b430697e1c9c832a8305cde6a65d85f65476a4285c6d6735f211ffc5722bd730b55e09aaf2db940fce260e925d492d8897fcd89dd472990d3698f49fe8d1bc8e85b4a87e7360c14e4ed0f410ca26424de2f63b480370598788cf6d4786749b22a848f6c794da2cff3146cf0a10fa0965d24016e87377eea3da34e244c18397bad8cc142fa0601f15e4236e9ef6802988eeff451839223cf884c383d1991333b20e8e9211e920cc451db75d5d16c710f2ab22962880e0d363c12e2aed62e942ebb56c66e6cae83c1bb12b0dce94f9705c521d2b16baeb8cf76b47aa0b0a8a2c784b1c106e3cd8a46ae1d1e283785f70039ed3d05e93b2ab38112800141712c6860f5711e90805c4e7be7aa4167a3eb063fde10ac7b904f9c129c0cf6c408e4238ca6a3d553c1f342c10b655499d37d802ff0b20cbbe86c51c923484513c05d89f4399c70fc5defc712736b2f4bd690e9326f5c416f983dd64afefdefadc86593d6f1ae9ae333ad222a6a1a27d8eb8c027a49a6441d0213578c267b07372c87641f9aa54e6fdf71b4cc0f28811dbe93e09f1a7d1466eb51e51d24983013445f35cf4f9571efd638af6a7cf820b8d78561dd31d3f4cd3f8926cb87994b27c22f118174f1a4dce66f6b4e758876040a84169e9f3d60c36750e575f2a1775e1eb38d11c46c5229cf988b273fd91cdb371970eb5b0706f14d3a3c040e10338bc5c134c088a7b06692234f3e3df51255ac5ac5b6e1b74079c630d671a76765cfbf36ca8593594e0a1c1571ac2fba1dbe2bc5e67d29ecdc53335af609a9f75e35978cb72aab69e4c31b40812b309e6c14c5994a93c74|8|END";

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
        		std::cout << "NO_ERROR\n";
				auto maybe_bytes = from_hex(block);
				if (!maybe_bytes) {
        			throw std::logic_error("ERR");
				}			
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
