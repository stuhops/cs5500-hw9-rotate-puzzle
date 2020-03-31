/* Implaments the Queue class. 
 *
 * Author: Stuart Hopkins (A02080107)
 * Last updated 1/24/2018
 */

#include "Queue.h"

//Constructor
Queue::Queue() {
	head = NULL;
	tail = NULL;	
}

//Returns the head board.
Board Queue::getHeadBoardInfo() {
	return head->my_board;
}

//Returns the history of the head board.
std::string Queue::getHeadBoardHistory() {
	return head->my_board.history();
}

//Returns the state of the head board.
int Queue::getHeadBoardState() {
	return head->state;
}

//Changes the state of the head board.
void Queue::changeHeadBoardState(int s) {
	head->my_board.changeState(s);
}

//Adds the new board onto the tail end of the list.
void Queue::addBoard(Board new_board, int the_state) {
	Node* n = NULL;
	n = new Node;
	n->my_board = new_board;
	n->state = the_state;
	n->next_node = NULL;
	if (head == NULL)
		head = n;
	else
		tail->next_node = n;
	tail = n;
}

//Deletes the first item in the queue.
void Queue::deleteFirst() {
	Node* tmp = new Node;
	tmp = head;
	if (head->next_node != NULL)
		head = head->next_node;
	delete tmp;
}

//Clears the Queue.
void Queue::clear() {
	Node* next = new Node;
	for (Node* c = head; c != NULL; c = next) {
		next = c->next_node;
		delete c;
	}
	head = NULL;
	tail = NULL;
}