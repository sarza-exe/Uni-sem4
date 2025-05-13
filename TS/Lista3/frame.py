import crcmod
import sys

FLAG = '01111110'
# CRC-16 function, same as the manual generator '11000000000000101'
crc16_func = crcmod.mkCrcFun(0x18005, initCrc=0x0000, rev=False)

def bit_stuff(data_bits: str) -> str:
    stuffed = []
    count_ones = 0
    for b in data_bits:
        stuffed.append(b)
        if b == '1':
            count_ones += 1
            if count_ones == 5:
                stuffed.append('0')
                count_ones = 0
        else:
            count_ones = 0
    return ''.join(stuffed)

def compute_crc(data_bits: str) -> str:
    # int(d, 2) change string into binary, convert to bytes, len() // 8 round up to full byte
    byte_data = int(data_bits, 2).to_bytes((len(data_bits) + 7) // 8)
    crc = crc16_func(byte_data)
    # add 0's at the beggining to get length = 16
    return f'{crc:016b}'

def frame_data(input_path: str, output_path: str):
    with open(input_path) as f:
        raw = f.read().strip()  # oczekujemy tylko '0' i '1'

    with open(output_path, 'w') as f_out:
        for i in range(0, len(raw), 32):
            chunk = raw[i:i+32]
            if len(chunk) < 32:
                # Optionally pad with zeros to make it 32 bits
                chunk = chunk.ljust(32, '0')

            crc = compute_crc(chunk)
            data = chunk + crc
            stuffed = bit_stuff(data)
            frame = FLAG + stuffed + FLAG
            f_out.write(frame + '\n')  # Each frame on a new line

if len(sys.argv) >= 3:
    arg1 = sys.argv[1]
    arg1 += ".txt"
    arg2 = sys.argv[2]
    arg2 += ".txt"
else:
    print("Podaj plik wejsciowy i wyjsciowy!")

frame_data("z.txt", "w.txt") # example (z, w)
print(len("11010101100000110011111010111110111000110100011010"))