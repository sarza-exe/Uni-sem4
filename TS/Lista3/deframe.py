import crcmod
import sys

FLAG = '01111110'
# CRC-16 function, same as the manual generator '11000000000000101'
crc16_func = crcmod.mkCrcFun(0x18005, initCrc=0x0000, rev=False)

def bit_unstuff(data_bits: str) -> str:
    unstuffed = []
    count_ones = 0
    i = 0
    while i < len(data_bits):
        bit = data_bits[i]
        unstuffed.append(bit)
        if bit == '1':
            count_ones += 1
            if count_ones == 5:
                # Skip next bit if it's a stuffed '0'
                i += 1
                count_ones = 0
        else:
            count_ones = 0
        i += 1
    return ''.join(unstuffed)

import binascii

def compute_crc(data_bits: str) -> str:
    byte_data = int(data_bits, 2).to_bytes((len(data_bits) + 7) // 8)
    crc = crc16_func(byte_data)
    return f'{crc:016b}'

def deframe_data(input_path: str, output_path: str):
    with open(input_path, 'r') as f:
        raw = f.read().strip()  # oczekujemy tylko '0' i '1'
    frames = [frame for frame in raw.split(FLAG) if frame]
    total_frames = len(frames)
    with open(output_path, 'w') as f_out:
        line_num = 0
        good_frames = 0
        for frame in frames:
            line_num += 1
            content = frame
            unstuffed = bit_unstuff(content)

            if len(unstuffed) < 16:
                print(f"Ramka {line_num} niepoprawna (za krótka)")
                print(frame)
                continue

            data_bits = unstuffed[:-16]
            crc_bits = unstuffed[-16:]

            calculated_crc = compute_crc(data_bits)

            if calculated_crc == crc_bits:
                good_frames += 1
                f_out.write(data_bits)
            else:
                print(f"Ramka {line_num} niepoprawna (CRC)")

        print(f"good frames: {good_frames}")
        print(f"bad frames:  {total_frames-good_frames}")

if len(sys.argv) >= 3:
    arg1 = sys.argv[1]
    arg1 += ".txt"
    arg2 = sys.argv[2]
    arg2 += ".txt"
else:
    print("Podaj plik wejsciowy i wyjsciowy!")

deframe_data(arg1, arg2) # example (w, y)