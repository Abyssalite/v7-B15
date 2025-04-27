import serial
import binascii
import os
import argparse
import sys
 
def menu(function, arduino, block_size = "1"):
	try:
		if function == '1':
		    send(arduino)
		elif function == '2':
		    receive(arduino)
		elif function == '3':
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
    
            
def binTransfer(size, arduino): 
    blocks = splitBin(size)
    i = 0
    end = ''
    while i < len(blocks): 
        if i == len(blocks) - 1: 
            end = "END"
        
        s = binascii.hexlify(blocks[i]).decode('utf-8')
        block_parity = str(parity(s.encode('utf-8')))
        data = s + '|' + block_parity + '|' + end
        print(i)
        arduino.write((data + '\n').encode('utf-8'))
        if(confirmation()):
            i += 1
        
def binReceive(arduino):   
    data = end = ''
    while (end != "END"):
        try:
            s = arduino.readline().decode('utf-8').strip()
            block,block_parity,end = s.split('|')
            newParity = str(parity(block.encode('utf-8')))
            #print (newParity + "|" + block_parity) 
        
            if(block_parity == newParity):
                arduino.write(("NO_ERROR\n").encode('utf-8'))
                binascii.unhexlify(block)
                data += block
                #print(f"Block: {end}")
            else: arduino.write(('\n').encode('utf-8'))
            
        except UnicodeDecodeError as e:
            arduino.write(('\n').encode('utf-8'))
        except binascii.Error as e:
            arduino.write(('\n').encode('utf-8'))
        except ValueError as e:
            arduino.write(('\n').encode('utf-8'))
            
    sys.stdout.buffer.write(binascii.unhexlify(data))
    sys.stdout.flush()
   
def confirmation():
    while True:
        s = arduino.readline().decode('utf-8')
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
        block_size = 1024  # Convert KB to bytes
    else: block_size = int(size) * 1024
        
    blocks = []

    data = sys.stdin.buffer.read(block_size)
    while data:
        blocks.append(data)
        data = sys.stdin.buffer.read(block_size)
            
    return blocks
     
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("port", type=str, help="Port")
    parser.add_argument("function", help="Function")
    parser.add_argument("--size", type=str, required= False, help="Block size in kb")

    args = parser.parse_args()
    arduino = serial.Serial('/dev/tty'+args.port, 9600)
    menu(args.function, arduino, args.size)
   
