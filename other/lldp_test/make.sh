#!/bin/sh

set -e
gcc -Wall -O2 -DNO_LWIP *.c -o lldp_test
./lldp_test
tshark -r lldp_test.pcap -PV
