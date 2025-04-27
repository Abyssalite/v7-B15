import serial
import time
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("param1", type=str, help="Port")

    args = parser.parse_args()
    arduino = serial.Serial('/dev/tty'+args.param1, 9600)    
    while(1):
        print(arduino)
        time.sleep(10) 
