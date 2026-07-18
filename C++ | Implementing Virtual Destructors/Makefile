
compile:
	g++ main02.cpp -o switch.out -O3 -Wall --std=c++20 && g++ main01.cpp -o virtual.out -O3 -Wall --std=c++20

run_benchmark: compile
	sudo perf stat ./switch.out && sudo perf stat ./virtual.out

verify_all_optimization_levels_main02:
	g++ main02.cpp -o out.out -Og -Wall --std=c++20 && valgrind -s ./out.out
	g++ main02.cpp -o out.out -O0 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main02.cpp -o out.out -O1 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main02.cpp -o out.out -O2 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main02.cpp -o out.out -O3 -Wall --std=c++20 && valgrind -s ./out.out
verify_all_optimization_levels_main01:
	g++ main01.cpp -o out.out -Og -Wall --std=c++20 && valgrind -s ./out.out
	g++ main01.cpp -o out.out -O0 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main01.cpp -o out.out -O1 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main01.cpp -o out.out -O2 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main01.cpp -o out.out -O3 -Wall --std=c++20 && valgrind -s ./out.out

verify_all_optimization_levels_main03:
	g++ main03.cpp -o out.out -Og -Wall --std=c++20 && valgrind -s ./out.out
	g++ main03.cpp -o out.out -O0 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main03.cpp -o out.out -O1 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main03.cpp -o out.out -O2 -Wall --std=c++20 && valgrind -s ./out.out
	g++ main03.cpp -o out.out -O3 -Wall --std=c++20 && valgrind -s ./out.out
