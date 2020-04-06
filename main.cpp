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
#include "Board.cpp"

using namespace std;

#define MCW MPI_COMM_WORLD


// ------------------------------- Functions ----------------------------------------
Board initialize();

vector<Board> prioritizeQueue(vector<Board> queue);

void print(string message, vector<int> arr);
void print(string message, int x);
void printQueue(vector<Board> queue);
void printQueueRanks(vector<Board> queue);
void printBreak();


// --------------------------------- Main -------------------------------------------
int main(int argc, char **argv) {
	const int ROWS = 5;
	const int DIRECTIONS = 4;  // N, E, S, W
	const int ITERS = 3;

  int rank, size;
  int data[ROWS * ROWS];
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);
	MPI_Request finishedRequest;
	MPI_Request workRequest;
	MPI_Status finishedStatus;
	MPI_Status workStatus;

	int workFlag = 0;
	int finished = 0;
	int primary_board_sum = 0;
	int num_of_levels = 0;
	int state1 = 0;
	int again = 0;
	std::string history_string = "";
	int board_int = 0;
	Board primary_board;
	Board curr_board;
	vector<Board> queue;

	if(!rank) {
		MPI_Irecv(&finished, 1, MPI_INT, MPI_ANY_SOURCE, 1, MCW, &finishedRequest);

		primary_board = initialize();
		queue.push_back(primary_board);

		if (!primary_board.getRank()) {
			std::cout << "You started with a perfect board." << endl;
		}
		else {
			while (true) {
				for(int a = 0; a < ITERS; a++) {
					num_of_levels++;
					std::cout << "Level " << num_of_levels << endl;

					int queue_length = queue.size();
					for(int b = 0; b < queue_length; b++) {

						for (int i = 0; i < ROWS; i++) { 
							for (int j = 0; j < DIRECTIONS; j++) {
								state1++;
								curr_board = queue[0];
								curr_board.move(DIRECTIONS * i + j);
								queue.push_back(curr_board);
								if (!curr_board.getRank()) {
									break;
								}
							}
							if (!curr_board.getRank()) break;
						}
						queue.erase(queue.begin());
						if (!curr_board.getRank()) {
							finished = 1;
							for(int j = 1; j < size; j++) {
								MPI_Send(&finished, 1, MPI_INT, j, 1, MCW);
							}
							break;
						}
					}
					if (!curr_board.getRank()) break;
				}
				if (!curr_board.getRank()) break;

				queue = prioritizeQueue(queue);
				if(queue.size() > ROWS * ROWS) 
					queue.resize(ROWS * ROWS);
				printQueue(queue);

				// ------------------------------- Send Work Out ------------------------------------
				if(!(num_of_levels % ITERS) || num_of_levels == 1) {
					if(num_of_levels != ITERS) {
						// ------------------ Receive --------------------
						for(int i = 1; i < size; i++) {
							// std::cout << "Pre-Wait" << endl;
							while(true) {
								MPI_Iprobe(i, 0, MCW, &workFlag, &workStatus);
								MPI_Test(&finishedRequest, &finished, &finishedStatus);

								if(finished) {
									for(int j = 1; j < size; j++) {
										MPI_Send(&finished, 1, MPI_INT, j, 1, MCW);
									}
									break;
								}
								else if(workFlag) {
									workFlag = 0;
									break;
								}
							}
							if(finished) break;
							// std::cout << "Post-Wait" << endl;

							for(int j = 0; j < size; j++) {
								MPI_Recv(&data, ROWS * ROWS, MPI_INT, i, 0, MCW, MPI_STATUS_IGNORE);
								queue.push_back(Board(data));
							}
						}
						queue = prioritizeQueue(queue);
						queue.resize(size);
						// printQueue(queue);
						if(num_of_levels / 5 > 5) {
							std::cout << "HIT LIMIT" << endl;
							while(true){};
						}
					}
					if(finished) break;

					// --------------------- Send ----------------------
					vector<int> curr;
					for(int i = 1; i < size; i++) {
						curr = queue[0].toVect();
						queue.erase(queue.begin());

						int to_send[curr.size()];
						for(int j = 0; j < curr.size(); j++) {
							to_send[j] = curr[j];
						}
						MPI_Send(&to_send, ROWS * ROWS, MPI_INT, i, 0, MCW);
					}
					curr_board = queue[0];
					queue.clear();
					queue.push_back(curr_board);
				}
				if(finished) break;
			}
		}
		std::cout << "YOU WIN!!! Original Board:" << endl << primary_board.toString() << endl;
		queue.clear();
		int to_send = 0;
	}

	// ----------------------------------- Slave Processes -------------------------------------
	else {
		MPI_Irecv(&finished, 1, MPI_INT, 0, 1, MCW, &finishedRequest);
		while(true) {
			MPI_Irecv(&data, ROWS * ROWS, MPI_INT, 0, 0, MCW, &workRequest);

			queue.clear();
			while(true) {
				MPI_Test(&finishedRequest, &finished, &finishedStatus);
				if(finished) {
					cout << rank << " Received a finish signal" << endl;
					break;
				}

				MPI_Test(&workRequest, &workFlag, &workStatus);
				if(workFlag) {
					workFlag = 0;
					// cout << rank << " Data Received" << endl;
					// if(rank == 1) {
					// 	std::cout << "Incoming array: " << endl;
					// 	for(int i = 0; i < ROWS; i++) {
					// 		for(int j = 0; j < ROWS; j++) {
					// 			std::cout << data[j + i*ROWS] << "  ";
					// 		}
					// 		std::cout << endl;
					// 	}
					// }
					queue.push_back(Board(data));
					break;
				}
			}

			// cout << rank << endl;
			// cout << queue[0].toString() << endl;

			for(int a = 0; a < ITERS; a++) {
				int queue_length = queue.size();
				// std::cout << rank << " starting " << a << "th iteration" << endl;
				for(int b = 0; b < queue_length; b++) {
					MPI_Test(&finishedRequest, &finished, &finishedStatus);
					if(finished) {
						std::cout << rank << " Received a finish signal" << endl;
						break;
					}

					for (int i = 0; i < ROWS; i++) { 
						for (int j = 0; j < DIRECTIONS; j++) {
							state1++;
							curr_board = queue[0];
							curr_board.move(DIRECTIONS * i + j);
							// std::cout << "State " << state1 << " from state " << queue[0].getState() << " History " << curr_board.history() << endl
							// 					<< curr_board.getRank() << endl
							// 					<< curr_board.toString() << endl;
							queue.push_back(curr_board);
							if (!curr_board.getRank()) {
								std::cout << rank << " Send Finished Signal" << endl;
								break;
							}
						}
						if (!curr_board.getRank()) break;
					}
					queue.erase(queue.begin());
					if (!curr_board.getRank()) {
						finished = 1;
						MPI_Send(&finished, 1, MPI_INT, 0, 1, MCW);
						break;
					}
					// queue.resize(size);
				}
				queue = prioritizeQueue(queue);
				if (!curr_board.getRank() || finished) break;
				// std::cout << rank << " ending " << a << "th iteration" << endl;
			}
			if (!curr_board.getRank() || finished) break;
			// std::cout << rank << " Out of loop " << endl;


			// std::cout << rank << " Ready to Send" << endl;
			vector<int> curr;
			for(int i = 0; i < size; i++) {
				curr = queue[0].toVect();
				queue.erase(queue.begin());

				int to_send[curr.size()];
				for(int j = 0; j < curr.size(); j++) {
					to_send[j] = curr[j];
				}
				MPI_Send(&to_send, ROWS * ROWS, MPI_INT, 0, 0, MCW);
			}

		}
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
	start4.makeBoard(8);
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


vector<Board> prioritizeQueue(vector<Board> queue) {
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
	return new_queue;
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
