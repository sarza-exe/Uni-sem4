import crcmod

FLAG = '01111110'
GENERATOR = '11000000000000101'
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
    byte_data = int(data_bits, 2).to_bytes((len(data_bits) + 7) // 8, 'big')
    crc = crc16_func(byte_data)
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
            stuffed = bit_stuff(chunk)
            frame = FLAG + stuffed + crc + FLAG
            f_out.write(frame + '\n')  # Each frame on a new line

frame_data("z.txt", "w.txt")
print(len("11010101100000110011111010111110111000110100011010"))