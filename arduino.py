import serial
import binascii
import argparse
import sys
import time
 
def menu(function, arduino, block_size = None):
    try:
        if function == '1':
            send(arduino)
        elif function == '2':
            receive(arduino)
        elif function == '3':

            if block_size is None:
                binTransfer(1, arduino)
            else:
               binTransfer(int(block_size), arduino)
        elif function == '4':
            binReceive(arduino)
        else:
            print("Function " + function + " not available\n")
    except ValueError:
            print("Size must be number\n")

def send(arduino): 
    s = sys.stdin.readline()
    arduino.write((s + '\n').encode('utf-8'))
        
def receive(arduino): 
    s = arduino.readline().decode('utf-8')
    sys.stdout.write(s)

def confirmation(arduino):
    while True:
        s = arduino.readline().decode('utf-8').strip()
        if s == "NE":
            return True
        else: return False
   
def parity(block):
    result = 0
    for byte in block:
        result ^= int(byte)
    return result

def splitBin(size):
    block_size = 32 if (size is None) else int(size) * 32
    blocks = []
    data = sys.stdin.buffer.read(block_size)

    while data:
        blocks.append(data)
        data = sys.stdin.buffer.read(block_size)        
    return blocks
            
def binTransfer(size, arduino): 
    blocks = splitBin(size)
    i = 0
    end = ""
    while i < len(blocks): 
        end = "END" if i == len(blocks) - 1 else str(i)
        
        s = binascii.hexlify(blocks[i]).decode('utf-8')
        block_parity = str(parity(s.encode('utf-8')))
        data = s + '|' + block_parity + '|' + end
        print("Block " + end + "| parity: " + block_parity + "\n")
        
        arduino.write((s).encode('utf-8'))
        arduino.write((data + '\n').encode('utf-8'))
        if(confirmation(arduino)): i += 1
        
def binReceive(arduino):   
    data = ""
    end = ""
    while (end != "END"):
        try:
            s = arduino.readline().decode('utf-8').strip()
            block,block_parity,end = s.split('|')
            newParity = str(parity(block.encode('utf-8')))

            time.sleep(0.1) #second
            if(block_parity == newParity):
                binascii.unhexlify(block)
                arduino.write(("NE\n").encode('utf-8'))
                data += block
            else: 
                arduino.write(("ER\n").encode('utf-8'))
            
        except Exception as e:
            arduino.write(("ER\n").encode('utf-8'))
            
    sys.stdout.buffer.write(binascii.unhexlify(data))
    sys.stdout.flush()
     
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=str, help="Port")
    parser.add_argument("function", help="Function")
    parser.add_argument("--size", type=str, required= False, help="Block size in kb")

    args = parser.parse_args()
    arduino = serial.Serial('/dev/tty'+args.port, 9600)
    menu(args.function, arduino, args.size)
