/* Project 1 Rotation Puzzle.cpp : Defines the entry point for the console application.
 * 
 * Author: Stuart Hopkins (A02080107)
 * Last updated: 1/24/2018
*/

#include <mpi.h>
#include <bits/stdc++.h> 
#include <iostream>
#include <string>
#include <vector>
#include "Board.h"
#include "Queue.h"
#include "Board.cpp"

using namespace std;

#define MCW MPI_COMM_WORLD


// ------------------------------- Functions ----------------------------------------
Board initialize();

vector<Board> prioritizeBoards(vector<Board> vect);

void print(string message, vector<int> arr);
void print(string message, int x);
void printQueue(vector<Board> queue);
void printQueueRanks(vector<Board> queue);
void printBreak();


// --------------------------------- Main -------------------------------------------
int main(int argc, char **argv) {
  int rank, size;
  int data;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);

	const int ROWS = 3;
	const int DIRECTIONS = 4;  // N, E, S, W
	int primary_board_sum = 0;
	int num_of_levels = 0;
	int state1 = 0;
	int again = 0;
	std::string history_string = "";
	int board_int = 0;
	Board primary_board;
	Board temp1;
	vector<Board> queue;

	if(!rank) {
		primary_board = initialize();
		queue.push_back(primary_board);

		if (!primary_board.getRank()) {
			std::cout << "You started with a perfect board." << endl;
		}
		else {
			while (true) {
				num_of_levels++;
				for (int i = 0; i < ROWS; i++) { 
					for (int j = 0; j < DIRECTIONS; j++) {
						state1++;
						temp1 = queue[0];
						temp1.move(DIRECTIONS * i + j);
						std::cout << "State " << state1 << " from state " << queue[0].getState() << " History " << temp1.history() << endl
											<< temp1.getRank() << endl
											<< temp1.toString() << endl;
						queue.push_back(temp1);
						if (!temp1.getRank()) {
							break;
						}
					}
					if (!temp1.getRank()) break;
				}
				queue.erase(queue.begin());
				if (!temp1.getRank()) break;

				// if num_of_levels % 3 receive all, do work then send more out
				if(num_of_levels == 1) {
					bool flag = false;
					vector<Board> new_queue;
					new_queue.push_back(queue[0]);
					for(int i = 1; i < queue.size(); i++) {
						flag = false;
						for(int j = 0; j < new_queue.size(); j++) {
							// cout << endl << queue[i].getRank() << endl;
							if(queue[i].getRank() < new_queue[j].getRank()) {
								flag = true;
								new_queue.insert(new_queue.begin() + j, queue[i]);
								break;
							}
						}
						if(!flag) {
							new_queue.push_back(queue[i]);
						}
					}
					queue = new_queue;
					printQueue(queue);
				}
			}
		}
		std::cout << "YOU WIN!!! Original Board:" << endl << primary_board.toString() << endl;
		queue.clear();
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


vector<Board> prioritizeBoards(vector<Board> vect) {
	bool flag = false;
	for(int i = 1; i < vect.size(); i++) {
		for(int j = 0; j < vect.size(); j++) {
			// cout << endl << vect[i].getRank() << endl;
			if(vect[i].getRank() < vect[j].getRank()) {
				vect.insert(vect.begin() + j, vect[i]);
			}
		}
	}

	printBreak();
	cout << endl << "prioritized: " << endl;
	for(int i = 0; i < vect.size(); i++) {
		cout << vect[i].getRank() << endl;
	}
	cout << endl << endl;
	return vect;
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


void printQueue(vector<Board> queue) {
	printBreak();
	cout << "Queue Ranks and Boards: " << endl;
	for(int i = 0; i < queue.size(); i++) {
		cout << queue[i].getRank() << endl
				 << queue[i].history() << endl
				 << queue[i].toString() << endl << endl;
	}
	printBreak();
}

void printQueueRanks(vector<Board> queue) {
	printBreak();
	cout << endl << "Queue Ranks: " << endl;
	for(int i = 0; i < queue.size(); i++) {
		cout << queue[i].getRank() << ", ";
	}
	printBreak();
}

void printBreak() {
  cout << "\n--------------------------------\n";
}
