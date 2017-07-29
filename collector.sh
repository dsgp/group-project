#!/bin/bash

SEED='1'
SIM_CYCLES=2000000
OUTPUT_FILE="results.m"

echo "data = [" > $OUTPUT_FILE

for i in $(seq 1 6); do
	data_ber=$(echo "5 * 10 ^ -$i" | bc -l)
	echo "./spacewire $SEED $SIM_CYCLES 0$data_ber ."
	./spacewire $SEED $SIM_CYCLES 0$data_ber . >> $OUTPUT_FILE

	data_ber=$(echo "1 * 10 ^ -$i" | bc -l)
	echo "./spacewire $SEED $SIM_CYCLES 0$data_ber ."
	./spacewire $SEED $SIM_CYCLES 0$data_ber . >> $OUTPUT_FILE
done

echo "];" >> $OUTPUT_FILE

