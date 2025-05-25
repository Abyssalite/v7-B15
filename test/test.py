import binascii
import argparse
import sys
 
def menu(function, block_size = None):
    try:
        if function == '1':
            print("send(arduino)")
        elif function == '2':
            print("receive(arduino)")

        elif function == '5':
            if block_size is None:
                blocks = splitBin(1)
                for block in blocks:
                    print(parity(block))
            else:
                blocks = splitBin(int(block_size))
                for block in blocks:
                    print(parity(block))

        elif function == '3':
            if block_size is None:
                binTransfer(1)
            else:
                binTransfer(int(block_size))
        elif function == '4':
            binReceive()
        else:
            print("Function " + function + " not available\n")
    except ValueError:
            print("Size must be number\n")
    
            
def binTransfer(size): 
    blocks = splitBin(size)
    i = 0
    end = ""
    while i < len(blocks): 
        if i == len(blocks) - 1: end = "END"
        
        s = binascii.hexlify(blocks[i]).decode('utf-8')
        block_parity = str(parity(s.encode('utf-8')))
        data = s + '|' + block_parity + '|' + end
        print(data)
        #arduino.write((data + '\n').encode('utf-8'))
        if(confirmation()): i += 1
        
def binReceive():   
    data = ""
    end = ""
    while (end != "END"):
        try:
            s = '33a2875ea602a96640e05fbc158e75583090c45d18835fc838853d55997ca3a4489a39cf2d4e53a61126d4179c3c07d9c5459f3b3dd0b1fe39bc25d154d692a86b805610e94446a8710ba20c4f7f1f10a85fcb6592c8c5f7fca3a8dcbb4e5cd8cf530c2f09a382c9808d3b40a4a775940dc73d8d0d289e3af24ec37870644e6575d57d8352196f20a3eb4f4c2f707d1b05001015ce66af9f82e430854ec5cacd19af8ea485f977e9314afd4e39d5e05c667377a13dc7d09f7e36a4571f02b777eefe4581ea79c2f054b79c0cfb77c031411b3c385b490e2f42828f9786313eecf8caefe0edab47cf1deb77c62a5b83cf48b64c7cd8636d9cf1284d809811c65ef9f38d403834dfca38ea321f0dadd66456081d0bc07cb6cc4b9a165f6c0afa71251607caea04b98ce0531543b800e8818a65dd81f722b5e88df3f50497f4f07c9235e887e2f933a489a9216aed08ebb94ca23d0ba3a2ac88aa6249e8d8477b215c38208f26f77eade0bd828e42368e0feda611cfb7ab91fab6c5579498c27fbdaecf0cca7d8fee0d0b3befd5c4d5c4360df0d90e2e3ffc45f805d15f9ed8e5892e281264967ff4ca04e1964ea138b430697e1c9c832a8305cde6a65d85f65476a4285c6d6735f211ffc5722bd730b55e09aaf2db940fce260e925d492d8897fcd89dd472990d3698f49fe8d1bc8e85b4a87e7360c14e4ed0f410ca26424de2f63b480370598788cf6d4786749b22a848f6c794da2cff3146cf0a10fa0965d24016e87377eea3da34e244c18397bad8cc142fa0601f15e4236e9ef6802988eeff451839223cf884c383d1991333b20e8e9211e920cc451db75d5d16c710f2ab22962880e0d363c12e2aed62e942ebb56c66e6cae83c1bb12b0dce94f9705c521d2b16baeb8cf76b47aa0b0a8a2c784b1c106e3cd8a46ae1d1e283785f70039ed3d05e93b2ab38112800141712c6860f5711e90805c4e7be7aa4167a3eb063fde10ac7b904f9c129c0cf6c408e4238ca6a3d553c1f342c10b655499d37d802ff0b20cbbe86c51c923484513c05d89f4399c70fc5defc712736b2f4bd690e9326f5c416f983dd64afefdefadc86593d6f1ae9ae333ad222a6a1a27d8eb8c027a49a6441d0213578c267b07372c87641f9aa54e6fdf71b4cc0f28811dbe93e09f1a7d1466eb51e51d24983013445f35cf4f9571efd638af6a7cf820b8d78561dd31d3f4cd3f8926cb87994b27c22f118174f1a4dce66f6b4e758876040a84169e9f3d60c36750e575f2a1775e1eb38d11c46c5229cf988b273fd91cdb371970eb5b0706f14d3a3c040e10338bc5c134c088a7b06692234f3e3df51255ac5ac5b6e1b74079c630d671a76765cfbf36ca8593594e0a1c1571ac2fba1dbe2bc5e67d29ecdc53335af609a9f75e35978cb72aab69e4c31b40812b309e6c14c5994a93c74|8|END'
            ("arduino.readline().decode('utf-8').strip()")
            block,block_parity,end = s.split('|')
            newParity = str(parity(block.encode('utf-8')))
            #print (block + " " + newParity + "|" + block_parity) 
        
            if(block_parity == newParity):
                #arduino.write(('NO_ERROR\n').encode('utf-8'))
                binascii.unhexlify(block)
                data += block
            #else: arduino.write(('ERR').encode('utf-8'))
            
        except UnicodeDecodeError as e:
            print("arduino.write(('ERR').encode('utf-8'))")
        except binascii.Error as e:
            print("arduino.write(('ERR').encode('utf-8'))")
        except ValueError as e:
            print("arduino.write(('ERR').encode('utf-8'))")
            
    sys.stdout.buffer.write(binascii.unhexlify(data))
    sys.stdout.flush()
   
def confirmation():
    while True:
        #s = arduino.readline().decode('utf-8')
        s = "NO_ERROR\n"
        if s == "NO_ERROR\n":
            return True
        else: return False
   
def parity(block):
    result = 0
    for byte in block:
        result ^= int(byte)
    return result

def splitBin(size):
    if size is None:
        block_size = 64  # Convert KB to bytes
    else: block_size = int(size) * 64
        
    blocks = []

    data = sys.stdin.buffer.read(block_size)
    while data:
        blocks.append(data)
        data = sys.stdin.buffer.read(block_size)
            
    return blocks
     
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("function", help="Function")
    parser.add_argument("--size", type=str, required= False, help="Block size in kb")

    args = parser.parse_args()
    menu(args.function, args.size)