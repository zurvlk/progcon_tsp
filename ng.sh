gcc solve.c -o nng -lm
./nng in_data2/st70.tsp
mv tour1.dat st70.dat
./check_validity st70.dat
