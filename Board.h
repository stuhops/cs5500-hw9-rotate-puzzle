/* Creates a Board class that enables the user to create and modify 3x3 boards with
 * values of 1-9 with each number only occuring once.
*
* Authors: Dr. Vicki Allan, and Stuart Hopkins (A02080107)
* Last updated: 1/24/2018
*/

#ifndef BOARD_H
#define BOARD_H
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include <vector>


class Board {
private:
	static const int SIZE = 3;
	int board[SIZE][SIZE];  // Values of board
	std::string move_history;
	int state;
	int rank;

	void updateRank();

public:
	Board() { makeBoard(); };
	Board(int arr[], std::string history);
	Board(const Board & b);  //Create board from another board
	std::vector<int> toArray(Board b); //Takes Board and returns contents into array of integers
										//in order to send contents through MPI
	std::string move(int m);  //m is which move.  Returns string describing move
	std::string history(); //Returns the history of the board
	std::string toString() const;  //return a string which represents the contents of the board
	bool operator==(Board &b);   //Return true if two boards are equal
	void makeBoard(int jumbleCt = 0);  //Starting from a perfect board, do jumbleCt moves to alter it
	void jumble(int ct);  //Do jumble moves to alter board

	void rotateEast(int row);
	void rotateWest(int row);
	void rotateNorth(int col);
	void rotateSouth(int col);

	void addHistory(std::string new_history); //Adds to the history string.

	int getState();
	int getRank() { return rank; };

	void changeState(int new_state);
};
#endif