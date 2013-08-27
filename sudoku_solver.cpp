/*
 * Sudoku solver. This program reads in 
 * a sudoku from a file and solve it using
 * heuristic techniques. 
 *
 * Dongpu Jin
 * 9/2/2012
 */

#include <fstream> 
#include <iostream> 
#include <string> 
#include <sstream>
#include <stdlib.h> 
#include <time.h>

using namespace std; 

/*
 * Structure that represents an item in the square
 */ 
typedef struct{
	int value; 
	bool fixed; // false: empty cell; true: fixed cell; 
}VAL;

/*
 * Function prototypes
 */
void myprint(VAL** square, int size);
bool parser(VAL** square, int size); 
void swap(VAL* row, int a, int b);
void randomize(VAL* row, int size);
int evaluate(VAL* array, int size);
int fitness(VAL** square, int row, int a, int b, int size);
int init_fitness(VAL** square, int size);

// find time difference
double diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
	return diffms;
}

int main(int argc, char* argv[]){
	// initialize the random seed	
	srand(time(NULL)); 
	
	// create a square
	VAL** square; 
	square = new VAL*[9]; 
	for(int i = 0; i < 9; i++){
		square[i] = new VAL[9]; 
	}
	if(parser(square, 9)){ // successfully parse the file
		// print out the original square
		cout << "*** Original Square ***" << endl; 
		myprint(square, 9);
		cout << endl; 
		
		// randomize the square
		for(int i = 0; i < 9; i++){
			randomize(square[i], 9);
		}
		// cout << "*** Randomized Square ***" << endl; 
		// myprint(square, 9);
		// cout << endl; 
		
		// initialize current cost
		int curr_cost = init_fitness(square, 9); 
		int rand_row, rand_a, rand_b, old_fitness, delta; 
		int cnt = 0; 
		
		clock_t begin = clock(); 
		// swap within a random selected row until reach zero cost
		while(curr_cost != 0){
			cnt++; // increment the counter 
			// pick two random elements in a row to be swap
			rand_row = rand() % 9;
			rand_a = rand() % 9; 
			rand_b = rand() % 9; 
			if(square[rand_row][rand_a].fixed == 0 && square[rand_row][rand_b].fixed == 0){
				old_fitness = fitness(square, rand_row, rand_a, rand_b, 9);
				swap(square[rand_row], rand_a, rand_b); 
				delta = fitness(square, rand_row, rand_a, rand_b, 9) - old_fitness;
				bool badmove = false; 
				if(rand() % 100 == 0){ // probability of badmove
					badmove = true;
				}
				if(delta <= 0 || badmove){	// improvement
					curr_cost += delta;  // update current cost
				}else{
					swap(square[rand_row], rand_b, rand_a); // undo swap
				}
				
				//cout << "row, a, b: " << rand_row << rand_a << rand_b << " delta: " << delta << " curr fit: " << curr_cost << endl; 

			}
		}// end of while loop
		clock_t end = clock(); 
		
		cout << "*** Sudoku Square ***" << endl; 
		myprint(square, 9);
		cout << endl; 
		cout << "Total iterations: " << cnt << endl; 
		cout << "Total time: " << diffclock(end, begin) << "ms" << endl; 
	}else{ // fail to parse the file. 
		return 1; 
	}
	
	return 0; 
}

/*
 * Evaluate the initial square
 */
int init_fitness(VAL** square, int size){
	int init_cost = 0; 
	VAL* temp = new VAL[size]; 
	
	// evaluate each column
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			temp[j] = square[j][i]; 
		}
		init_cost += evaluate(temp, size); 
	}
		
	// evaluate each quadrant
	int index; 
	for(int quad_row = 0; quad_row < 3; quad_row++){
		for(int quad_col = 0; quad_col < 3; quad_col++){
			index = 0; 
			// one quadrant
			for(int m = quad_row * 3; m < quad_row * 3 + 3; m++){
				for(int n = quad_col * 3; n < quad_col * 3 + 3; n++){
					temp[index] = square[m][n]; 
					index++; 
				}
			}
			init_cost += evaluate(temp, size);
		}
	}
	
	return init_cost;
}

/*
 * Calculate the fitness of the given square
 * a, b specify two column index and row
 * specifies the row
 * 
 * param square being looking at
 * param row, looking at which row
 * param a, b, two columns to be swapped
 * param size, size of the square
 * returns the cost difference (delta)
 */
int fitness(VAL** square, int row, int a, int b, int size){
	// original square
	int missing = 0; 
	VAL* temp; 
	temp = new VAL[size]; 
	
	// evaluate column a
	for(int i = 0; i < size; i++){
		temp[i] = square[i][a];
	}
	missing += evaluate(temp, size);
	
	// evaluate column b
	for(int j = 0; j < size; j++){
		temp[j] = square[j][b];
	}
	missing += evaluate(temp, size);
	
	// evaluate quadrants 
	int quad_a; 
	int quad_b; 
	int quad_row; 
	
	quad_a = a / 3; 
	quad_b = b / 3; 
	quad_row = row / 3; 
	
	int index = 0; 
	for(int m = quad_row * 3; m < quad_row * 3 + 3; m++){
		for(int n = quad_a * 3; n < quad_a * 3 + 3; n++){
			temp[index] = square[m][n]; 
			index++; 
		}
	}
	missing += evaluate(temp, size);
	
	index = 0; 
	for(int p = quad_row * 3; p < quad_row * 3 + 3; p++){
		for(int q = quad_b * 3; q < quad_b * 3 + 3; q++){
			temp[index] = square[p][q]; 
			index++; 
		}
	}
	missing += evaluate(temp, size); 
	
	return missing;
}

/*
 * Evaluate the given array to find out 
 * how many digits are missing. 
 */
int evaluate(VAL* array, int size){
	int* cnt;
	cnt = new int[size]; 
	
	// initialize cnt to zero
	for(int i = 0; i < size; i++){ 
		cnt[i] = 0; 
	}
	
	// find out what digits exist
	for(int j = 0; j < size; j++){ 
		cnt[array[j].value - 1]++;  // notice the - 1 here!
	}
	
	// find out what digits are missing
	int cost = 0; 
	for(int k = 0; k < size; k++){
		if (cnt[k] == 0){
			cost++; 
		}
	}
	
	return cost;
}

/*
 * Parse input file and initialize square
 */
bool parser(VAL** square, int size){
	string filename, line; 
	// prompt user for input file name
	cout << "Please enter input file name: "; 
	cin >> filename;  
	
	ifstream infile(filename.c_str());
	string item_str;
	int item_int; 
	if(infile.is_open()){ // file is open
		for(int i = 0; i < size; i++){ // read line by line
			getline(infile, line); 
			stringstream stream(line); // convert each line to ss
			for(int j = 0; j < size; j++){ // parse each line
				getline(stream, item_str, ' '); 
				item_int = atoi(item_str.c_str()); // convert to int
				square[i][j].value = item_int;
				if(item_int == 0){
					square[i][j].fixed = false;  // empty cell
				}else{
					square[i][j].fixed = true;  // fixed cell
				}
			}
		}
		infile.close(); // don't forget to close the file
		return true;
	}else{
		cout << "File does not exist. Exit. " << endl; 
		return false; 
	}
}

/*
 * Randomize a given row
 */
void randomize(VAL* row, int size){
	// an array of avaliable digits
	int* array = NULL; 
	array = new int[size]; 
	for(int i = 0; i < size; i++){
		array[i] = i + 1; 
	}
	// initialize random array to zeros
	int * random = NULL; 
	random = new int[size]; 
	for(int i = 0; i < size; i++){
		random[i] = 0; 
	}
	// randomize
	for(int i =0; i < size; i++){
		int index = rand() % size;
		while(random[index] != 0){
			index = rand() % size;
		}
		random[index] = array[i]; 
	}
	
	bool exist;
	for(int i = 0; i < size; i++){
		exist = false; 
		for(int j = 0; j < size; j++){
			if(row[j].value == random[i]){
				exist = true; 
				break;
			}
		}
		if(!exist){ //doesn't exist, find an empty cell to insert
			for(int k = 0; k < size; k++){
				if(row[k].value == 0){
					row[k].value = random[i];
					break;
				}
			}
		}
	}

}

/*
 * Swap two elements in a row
 */ 
void swap(VAL* row, int a, int b){
	VAL temp; 
	temp.value = row[a].value;
	row[a].value = row[b].value;
	row[b].value = temp.value;
}

/*
 * Print out the square
 */
void myprint(VAL** square, int size){
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			cout << square[i][j].value << " "; 
		}
		cout << endl; 
	}
}
