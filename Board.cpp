/* Implaments the Board class.
*
* Authors: Dr. Vicki Allan, and Stuart Hopkins (A02080107)
* Last updated: 1/24/2018
*/

#include "Board.h"
#include <vector>

using namespace std;

//Copy constructor
Board::Board(const Board & b) {
	//std::cout << "Just Copied Board\n" << b.toString() << std::endl;
	assert(SIZE > 0);
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			board[i][j] = b.board[i][j];
	move_history = b.move_history;
	last_move_row = b.last_move_row;
	last_move_direction = b.last_move_direction;
	updateRank();
}

// Does not verify the incoming array is the correct size
Board::Board(int arr[], string history) {
	move_history = history;
	last_move_row = -1;
	last_move_direction = -1;

	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			board[i][j] = arr[j + i*SIZE];
		}
	}

	updateRank();
}

Board::Board(int arr[]) {
	move_history = "BLANK";
	state = -1;
	last_move_row = -1;
	last_move_direction = -1;

	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			board[i][j] = arr[j + i*SIZE];
		}
	}

	updateRank();
}

// Make one move.  m indicates which move is wanted. Return string describing the move
std::string Board::move(int m) {
	std::stringstream ss;
	ss << ":";
	int sub = m / 4;
	switch (m % 4)
	{
	case 0:  rotateNorth(sub); ss << "^" << sub; break;
	case 1:  rotateSouth(sub); ss << "v" << sub; break;
	case 2:  rotateEast(sub); ss << ">" << sub; break;
	case 3:  rotateWest(sub); ss << "<" << sub; break;
	}
	cout << "From Move: [" << last_move_row << ", " << last_move_direction << "]" << endl;
	addHistory(ss.str());
	updateRank();
	last_move_row = sub;
	last_move_direction = m % 4;
	return ss.str();
}

std::string Board::history() {
	return move_history;
}

// Create a printable representation of the Board class
std::string Board::toString() const {
	std::stringstream ss;
	// TODO: REMOVE THIS
	ss << "last move: [" << last_move_row << ", " << last_move_direction << "]" << std::endl;
	for (int i=0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
			ss << board[i][j] << " ";
		ss << std::endl;
	}
	return ss.str();
};

int Board::getState() {
	return state;
}

// return true if board is identical to b
bool Board::operator==(Board &b) {
	
	for (int i=0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (board[i][j] != b.board[i][j]) return false;
	return true;
}

/* Create a board by performing legal moves on a board
 * jumbleCt indicates the number of moves that may be required
 * if jumbleCt ==0, return the winning board                */
void Board::makeBoard(int jumbleCt) {
	int val=1;
	for (int i=0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			board[i][j] = val++;
	jumble(jumbleCt);
	updateRank();
	last_move_row = -1;
	last_move_direction = -1;
}

// Randomly apply ct moves to the board
void Board::jumble(int ct) {
	for (int i = 0; i < ct; i++)
		move(rand() % (SIZE * 4));
	move_history = "";
}

//Rotate East using row specified
void Board::rotateEast(int row){
	if (row <0|| row >=SIZE) return;
	int wrap = board[row][SIZE-1];
	for (int i = SIZE-2; i >=0; i--)
		board[row][i+1] = board[row][i];
	board[row][0] = wrap;
}

//Rotate West using row specified
void Board::rotateWest(int row){
	if (row <0 || row >=SIZE) return;
	int wrap = board[row][0];
	for (int i = 0; i <SIZE-1; i++)
		board[row][i] = board[row][i+1];
	board[row][SIZE-1] = wrap;
}

//Rotate South using column specified
void Board::rotateSouth(int col){
	if (col <0||col >=SIZE) return;
	int wrap = board[SIZE-1][col];
	for (int i = SIZE-2; i >=0; i--)
		board[i+1][col] = board[i][col];
	board[0][col] = wrap;
}

//Rotate North using column specified
void Board::rotateNorth(int col){
	if (col <0||col >=SIZE) return;
	int wrap = board[0][col];
	for (int i = 0; i <SIZE-1; i++)
		board[i][col] = board[i+1][col];
	board[SIZE-1][col] = wrap;
}

void Board::addHistory(std::string new_history) {
	std::stringstream ss;
	ss << move_history << new_history;
	move_history = ss.str();
}

void Board::changeState(int new_state) {
	state = new_state;
}

// Low score is better
void Board::updateRank() {
	int score = 0;

	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			int curr = board[i][j];
			int perfJ = (curr - 1) % SIZE;
			int perfI = (curr - perfJ - 1) / SIZE;

			int scoreI = abs(i - perfI);
			int scoreJ = abs(j - perfJ);

			// Item can rotate either way so the score should be adjusted accordingly
			scoreI = scoreI <= SIZE / 2 ? scoreI : SIZE - scoreI;  
			scoreJ = scoreJ <= SIZE / 2 ? scoreJ : SIZE - scoreJ;  

			score += scoreI + scoreJ;
		}
	}

	rank = score;
}

vector<int> Board::toVect(){
	vector<int> temp;
	for(int i = 0; i < SIZE; i++){
		for(int j = 0; j < SIZE; j++){
			temp.push_back(board[i][j]);
		}
	}
	return temp;
}

bool Board::isReversal(int arr[]) {
	cout << "LOOK: [" << last_move_row << ", " << last_move_direction << "]" << " = [" << arr[0] << ", " << arr[1] << "]" << std::endl;
	if(last_move_row == -1) {
		// cout << "ERROR THE LAST MOVE ARRAY IS NOT SET CORRECTLY" << endl;
	}
	if(arr[0] != last_move_row) 
		return false;
	
	switch (arr[1]) {
		case 0:  return (last_move_direction == 1);
		case 1:  return (last_move_direction == 0);
		case 2:  return (last_move_direction == 3);
		case 3:  return (last_move_direction == 2);
	}
	return false;
}