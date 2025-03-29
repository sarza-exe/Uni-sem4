import scapy.all as scapy
#from scapy import all as scapy

#pakiet = scapy.Ether() / scapy.IP()

#print(pakiet.show())

#p = scapy.sniff(count=1)

#print(p.summary())

from scapy.all import *

# Sniff 10 packets
packets = sniff(count=10)

# Print out the summary of captured packets
packets.summary()