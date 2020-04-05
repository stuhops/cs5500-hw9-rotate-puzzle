/* Project 1 Rotation Puzzle.cpp : Defines the entry point for the console application.
 * 
 * Author: Stuart Hopkins (A02080107)
 * Last updated: 1/24/2018
*/

#include <mpi.h>
#include <iostream>
#include <string>
#include <vector>
#include "Board.h"
#include "Queue.h"
#include "Board.cpp"
#include "Queue.cpp"

using namespace std;

#define MCW MPI_COMM_WORLD


// ------------------------------- Functions ----------------------------------------
bool sortBoards(Board b1, Board b2) { return b1.getRank() < b1.getRank(); }
Board initialize();

void print(string message, vector<int> arr);
void print(string message, int x);
void printBreak();


// --------------------------------- Main -------------------------------------------
int main(int argc, char **argv) {
  int rank, size;
  int data;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);

	if(!rank) {
		//Variables:
		int primary_board_sum = 0;
		int num_of_levels = 0;
		int state1 = 0;
		int again = 0;
		std::string history_string = "";
		int board_int = 0;
		Board primary_board;
		Board temp1;
		Queue the_queue;

		primary_board = initialize();
		the_queue.addBoard(primary_board, state1);

		//Check to see if they started with a perfect board.
		if (!primary_board.getRank())
			std::cout << "You started with a perfect board." << endl;
		//If not then solve for the starting puzzle.


		else {
			while (true) {
				num_of_levels++;
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 4; j++) {
						state1++;
						temp1 = the_queue.getHeadBoardInfo();
						temp1.move(4 * i + j);
						std::cout << "State " << state1 << " from state " << the_queue.getHeadBoardState() << " History " << temp1.history() << endl
											<< temp1.getRank() << endl
											<< temp1.toString() << endl;
						the_queue.addBoard(temp1, state1);
						if (!temp1.getRank()) {
							break;
						}
					}
					if (!temp1.getRank()) break;
				}
				the_queue.deleteFirst();
				if (!temp1.getRank()) break;

				// if num_of_levels % 3 receive all, do work then send more out
			}
		}
		ident:
		std::cout << "YOU WIN!!! Original Board:" << endl << primary_board.toString() << endl;
		the_queue.clear();
		int to_send = 0;
		for(int i = 1; i < size; i++) {
			MPI_Send(&to_send, 1, MPI_INT, i, 0, MCW);
		}
	}

	// ----------------------------------- Slave Processes -------------------------------------
	else {
		int data;
		MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
	}

	MPI_Finalize();
	return 0;
}


// --------------------------------------- Functions -----------------------------------------
Board initialize() {
	int input = 0;
	Board start1;
	Board start2;
	Board start3;
	Board start4;

	//start1 set up
	start1.rotateEast(1);
	start1.rotateEast(2);

	//start2 set up
	start2.rotateNorth(0);
	start2.rotateSouth(2);

	//start 3 set up
	start3.rotateNorth(2);
	start3.rotateSouth(0);
	start3.rotateEast(0);
	start3.rotateEast(2);

	//start 4 set up
	start4.makeBoard(4);
	//Print out the to the terminal to give the user options of boards to start from.
	std::cout << endl << "Option 1: " << endl << start1.toString() << endl;
	std::cout << endl << "Option 2: " << endl << start2.toString() << endl;
	std::cout << endl << "Option 3: " << endl << start3.toString() << endl;
	std::cout << endl << "Option 4: " << endl << start4.toString() << endl;
		
	std::cout << "Please enter the number of the board you want to start with: " << endl;
	while (true) {
		std::cin >> input;

		if (input >= 1 && input <= 4)
			break;
		std::cout << "You have entered an incorrect value please enter a valid number of option: ";
	}
	switch (input)
	{
		case 1:  return start1;
		case 2:  return start2;
		case 3:  return start3;
		case 4:  return start4;
	}
	return start1;
}


// Print functions
void print(string message, vector<int> vect) {
  cout << message << ": " << endl;

  cout << vect[0];
  for(int i = 1; i < vect.size(); i++) {
    cout << ", " << vect[i];
  }
  cout << endl;
}

void print(string message, int x) {
  cout << message << ": " << x << endl;
}

void printBreak() {
  cout << "\n--------------------------------\n";
}
