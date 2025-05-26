import binascii
import argparse
import sys
   
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
            s = "5e598c47f3c204084c70874c4ba95e5cc1b3d6e90e02c993591c27ce59f6f3110f0a5244d637f301ffb42b28b499c9099399cbe9f79033938214de1aac4c5e8a|94|END"

            #s = arduino.readline().decode('utf-8').strip()
            block,block_parity,end = s.split('|')
            newParity = str(parity(block.encode('utf-8')))
            #print (block + " " + newParity + "|" + block_parity) 
        
            if(block_parity == newParity):
                binascii.unhexlify(block)
                print("arduino.write(('NO_ERROR').encode('utf-8'))")
                data += block
            else: print("arduino.write(('ERR').encode('utf-8'))")
            
        except Exception as e:
            print("arduino.write(('ERR').encode('utf-8'))")
            
    sys.stdout.buffer.write(binascii.unhexlify(data))
    sys.stdout.flush()

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
     
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("function", help="Function")
    parser.add_argument("--size", type=str, required= False, help="Block size in kb")

    args = parser.parse_args()
    menu(args.function, args.size)