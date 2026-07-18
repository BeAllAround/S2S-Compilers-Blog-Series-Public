
compile:
	g++ main02.cpp -o out.out -O3 -Wall --std=c++20 && g++ main01.cpp -o out1.out -O3 -Wall --std=c++20

run_benchmark: compile
	sudo perf stat ./out.out && sudo perf stat ./out1.out
