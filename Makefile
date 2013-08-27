sudoku_solver: sudoku_solver.o
	g++ sudoku_solver.cpp -o sudoku_solver

sudoku_solver.o: sudoku_solver.cpp
	g++ -c sudoku_solver.cpp

clean: 
	rm -rf *o sudoku_solver
