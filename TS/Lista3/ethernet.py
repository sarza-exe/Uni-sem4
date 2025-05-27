import random

class packet():
    def __init__(self, symbol, direction):
        self.symbol = symbol
        self.direction = direction

class station():
    def __init__(self, symbol, position, sleep_time, trn_duration, jam_duration, failed_attemps):
        self.symbol = symbol
        self.position = position
        self.sleep_time = sleep_time
        self.trn_duration = trn_duration
        self.jam_duration = jam_duration
        self.failed_attemps = failed_attemps


NETWORK_LENGTH = 30
SIGNAL_DURATION = NETWORK_LENGTH * 2 + 1
SIMULATION_STEPS = 600

working_array = [[] for _ in range(NETWORK_LENGTH)]
output_array = ["_" for _ in range(NETWORK_LENGTH)]

def check_congestion(position) -> bool:
    tile1 = working_array[position-1]
    tile2 = working_array[position]

    if len(tile1) != 0 or len(tile2) != 0:
        return True
    else:
        return False
    
def check_jam(position) -> bool:
    tile1 = working_array[position-1]
    tile2 = working_array[position]

    jam_found = False

    for pkt in tile1:
        if pkt.symbol == "=":
            jam_found = True
            break
    for pkt in tile2:
        if pkt.symbol == "=":
            jam_found = True
            break

    if jam_found:
        return True
    return False


stations = {
    station('A', 10, random.randint(0, SIGNAL_DURATION * 2), SIGNAL_DURATION, SIGNAL_DURATION, 0),
    station('B', 23, random.randint(0, SIGNAL_DURATION * 2), SIGNAL_DURATION, SIGNAL_DURATION, 0)
}

file = open("zad_2.txt", 'w')

for _ in range(SIMULATION_STEPS):

    new_working_array = [[] for _ in range(NETWORK_LENGTH)]
    for index, tile in enumerate(working_array):
        for pkt in tile:
            if pkt.direction == "left":
                new_index = index - 1
            elif pkt.direction == "right":
                new_index = index + 1
            else:
                continue  # Invalid direction

            if 0 <= new_index < NETWORK_LENGTH:
                new_working_array[new_index].append(pkt)
            # else: packet moves out of bounds and is dropped
    working_array = new_working_array


    for stn in stations:
        if stn.jam_duration < SIGNAL_DURATION:
            working_array[stn.position-1].append(packet("=", "left"))
            working_array[stn.position].append(packet("=", "right"))
            stn.jam_duration -= 1
            if stn.jam_duration == 0:
                stn.jam_duration = SIGNAL_DURATION
                stn.sleep_time = random.randint(SIGNAL_DURATION, SIGNAL_DURATION * 2 * stn.failed_attemps)
                stn.sleep_time = stn.sleep_time % SIGNAL_DURATION
        elif stn.sleep_time > 0:
            stn.sleep_time -= 1
            if stn.sleep_time == 0:
                if check_congestion(stn.position):
                    stn.sleep_time += 1
                else:
                    working_array[stn.position-1].append(packet(stn.symbol, "left"))
                    working_array[stn.position].append(packet(stn.symbol, "right"))
                    stn.trn_duration -= 1
        else:
            if stn.trn_duration > 0:
                if check_congestion(stn.position):
                    stn.trn_duration = SIGNAL_DURATION
                    working_array[stn.position-1].append(packet("=", "left"))
                    working_array[stn.position].append(packet("=", "right"))
                    stn.jam_duration -= 1
                    stn.failed_attemps += 1
                else:
                    working_array[stn.position-1].append(packet(stn.symbol, "left"))
                    working_array[stn.position].append(packet(stn.symbol, "right"))
                    stn.trn_duration -= 1

            if stn.trn_duration == 0:
                stn.trn_duration = SIGNAL_DURATION
                stn.sleep_time = random.randint(0, SIGNAL_DURATION)
                stn.failed_attemps = 0
    

    for index, tile in enumerate(working_array):
        if len(tile) == 0:
            output_array[index] = "_"
        elif len(tile) == 1:
            output_array[index] = tile[0].symbol
        else:
            jam_found = False
            for pkt in tile:
                if pkt.symbol == "=":
                    jam_found = True
                    break
            if jam_found:
                output_array[index] = "="
            else:
                output_array[index] = "#"

    for tile in output_array:
        file.write(tile + " ")
    file.write('\n')

file.close()
