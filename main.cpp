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

void print(string message, vector<int> arr);
void print(string message, int x);
void printBreak();

int main(int argc, char **argv) {
  int rank, size;
  int data;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MCW, &rank);
  MPI_Comm_size(MCW, &size);

	if(!rank) {

		cout << rank << endl;
		//Variables:
		int primary_board_sum = 0;
		int num_of_levels = 0;
		int input = 0;
		int state1 = 0;
		int again = 0;
		std::string history_string = "";
		int board_int = 0;
		Board primary_board;
		Board perfect_board;
		Board temp1;
		Board temp2;
		Board temp3;
		Board temp4;
		Board start1;
		Board start2;
		Board start3;
		Board start4;
		Queue the_queue;

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
		case 1:  primary_board = start1; break;
		case 2:  primary_board = start2; break;
		case 3:  primary_board = start3; break;
		case 4:  primary_board = start4; break;
		}
		the_queue.addBoard(primary_board, state1);

		//Check to see if they started with a perfect board.
	// while(true){
		if (perfect_board.operator==(primary_board)) {
			std::cout << "You started with a perfect board." << endl;
		}
		//If not then solve for the starting puzzle.
		else {
			// while (true) {
				num_of_levels++;
				for (int i = 0; i < 3; i++) {
						
				//Create a copy of the head board, move North, and check if the user wins.
				state1++;
				temp1 = the_queue.getHeadBoardInfo();
				temp1.move(4 * i + 0);	//Rotate North.
				std::cout << "State " << state1 << " from state " << the_queue.getHeadBoardState() << " History " << temp1.history() << endl
									<< temp1.getRank() << endl
									<< temp1.toString() << endl;
				the_queue.addBoard(temp1, state1);
				if (temp1.operator==(perfect_board))
					goto ident;
				
				//Create a copy of the head board, move South, and check if the user wins.
				state1++;
				temp2 = the_queue.getHeadBoardInfo();
				temp2.move(4 * i + 1);  //Rotate South.
				std::cout << "State " << state1 << " from state " << the_queue.getHeadBoardState() << " History " << temp2.history() << endl 
									<< temp2.getRank() << endl 
									<< temp2.toString() << endl;
				the_queue.addBoard(temp2, state1);
				if (temp2.operator==(perfect_board))
					goto ident;

				//Create a copy of the head board, move East, and check if the user wins.
				state1++;
				temp3 = the_queue.getHeadBoardInfo();
				temp3.move(4 * i + 2);  //Rotate East.
				std::cout << "State " << state1 << " from state " << the_queue.getHeadBoardState() << " History " << temp3.history() << endl 
									<< temp3.getRank() << endl
									<< temp3.toString() << endl;
				the_queue.addBoard(temp3, state1);
				if (temp3.operator==(perfect_board))
					goto ident;

				//Create a copy of the head board, move West, and check if the user wins.
				state1++;
				temp4 = the_queue.getHeadBoardInfo();
				temp4.move(4 * i + 3);  //Rotate West
					std::cout << "State " << state1 << " from state " << the_queue.getHeadBoardState() << " History " << temp4.history() << endl 
										<< temp4.getRank() << endl
										<< temp4.toString() << endl;
					the_queue.addBoard(temp4, state1);
					if (temp4.operator==(perfect_board))
						goto ident;
	
				}
				the_queue.deleteFirst();

			
	
				//This program runs using integers. If the state counter is above 32,000 then it might cause overflow in many of the variables.
				//With this overflow, this program becomes useless. Because of this we must quit the program.
				if (state1 > 32000) {
					std::cout << "The board entered either impossible to solve, or took to many moves to be solved by this program. Goodbye." << endl;
					return 0;
				}
			// }
		}
		ident:
		std::cout << "YOU WIN!!! Original Board:" << endl << primary_board.toString() << endl;
		the_queue.clear();
		vector<int> temp = temp1.toVect();
		print("My temp vector", temp);
		cout << "vector size" << temp.size() << endl;
		int to_send = 0;
		for(int i = 0; i < 9; i++) {
			// cout << i <<  " : " << temp[i] << endl;
			// print("vector before send", temp);
			MPI_Send(&temp[i], 1, MPI_INT, 1, 0, MCW);
			// print("vector after send", temp);
		}
		



	}

	// ----------------------------------- Slave Processes -------------------------------------
	else {

		cout << "I AM IN A DIFFERENT PROCESS" << endl;
		vector<int> subVector;
		int data;
		int pos = 0;
		
		while(true){
			MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);		
			subVector.push_back(data);
			if(subVector.size() == 9 ){
				break;
			}
		}
		print("This is my received data", subVector);
		cout << rank << endl;

		Board newBoard;
		newBoard;
	}
	
	MPI_Finalize();
	return 0;
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
