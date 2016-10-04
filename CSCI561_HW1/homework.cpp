//
//  main.cpp
//  561hw1
//
//  Created by wu xiao yue on 9/12/16.
//  Copyright © 2016 eve. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
using namespace std;

string method, start_state, goal_state;

struct Node{
	string state;					// state name
	int number;
	int parent_number;		// parent number
	double g;							// distance from start state to current
	double h;							// estimated distance from current to goal state
	double f;							// estimated distance from start to goal state
	// operator overload for sort
	bool operator <(const Node &node) const {
		if (method == "UCS") {
			return node.g < g;
		} else if (method == "A*") {
			return node.f < f;
		} else {
			return 0;
		}
	}
	// operator overload for find_if
	bool operator() (const Node &node) {
		return node.state == state;
	}
};

struct Out{
	string state;
	double time;
};

//unordered_map<string, unordered_map<string, double> > live_traffic;
//unordered_map<string, double> sunday_traffic;

vector<pair<string, vector<pair<string,double> > > > live_traffic;
vector<string,double> sunday_traffic;
//vector<Node> searchTree;
deque<Node> explored;				// store the explored nodes
deque<Node> frontier;				// store the frontier nodes
deque<Node> record_node;		// store the node generated sequence

void input();
void output(Node goal);
Node general_search();
Node make_node(string state, int parent, double g);
void expand(Node current_node);


int node_number = 0;

int main(int argc, const char * argv[]) {
	Node goal;
	input();
	goal = general_search();
	output(goal);
	return 0;
}

Node general_search() {
	Node current_node;
	node_number = 0;
	explored.clear();
	frontier.clear();
	frontier.push_back(make_node(start_state, -1, 0));	// insert the start state node
	while (!frontier.empty()) {													// while there are nodes in frontier
		current_node = frontier.front();									// get the first node in the frontier
		frontier.pop_front();
		if (current_node.state == goal_state) {						// if it is the goal state node
			return current_node;														// return current node, which is the goal state node
		} else {																					// expand current node
			explored.push_back(current_node);
			expand(current_node);
		}
	}
	Node failure;
	failure.number = -1;
	return failure;
}

Node make_node(string state, int parent_number, double g) {
	Node new_node;
	new_node.state = state;
	new_node.parent_number = parent_number;
	new_node.g = g;
	new_node.number = node_number++;
	unordered_map<string, double>::iterator iter;
	iter = sunday_traffic.find(new_node.state);
	if (iter == sunday_traffic.end()) {
		cout << "state not exit in sundy traffic" << endl;
	} else {
		new_node.h = iter -> second;
	}
	new_node.f = new_node.g + new_node.h;
	record_node.push_back(new_node);
	return new_node;
}

void expand(Node current_node) {
	// get adjacent states
	unordered_map<string, unordered_map<string,double> >::iterator iter1;
	iter1 = live_traffic.find(current_node.state);
	if (iter1 != live_traffic.end()) {
		unordered_map<string,double> temp = iter1 -> second;
		unordered_map<string,double>::iterator iter2;
		deque<Node> store;
		// go through every adjacent nodes
		for (iter2 = temp.begin(); iter2 != temp.end(); iter2++) {
			string child_state = iter2 -> first;
			double distance = iter2 -> second;
			double child_g = current_node.g + distance;
			Node child_node = make_node(child_state, current_node.number,child_g);
			// check whether this node is already in explored or frontier queue
			deque<Node>::iterator position_explored, position_frontier;
			position_explored = find_if(explored.begin(), explored.end(), Node(child_node));
			position_frontier = find_if(frontier.begin(), frontier.end(), Node(child_node));
			// choose different insert method
			if (method == "BFS") {
				// this child node is neither in explored queue nor in frontier queue
				if (position_explored == explored.end() && position_frontier == frontier.end()) {
					// put in frontier queue's back
					frontier.push_back(child_node);
				}
			} else if (method == "DFS") {
				// this child node is neither in explored queue nor in frontier queue
				if (position_explored == explored.end() && position_frontier == frontier.end()) {
					// put in a temp queue for store all siblings
					store.push_front(child_node);
				}
				
			} else if (method == "UCS") {
				int insert = 0;
				// this child node is neither in explored queue nor in frontier queue
				if (position_explored == explored.end() && position_frontier == frontier.end()) {
					insert = 1;																// put in frontier queue
				} else if (position_explored != explored.end()) {
					Node old = *position_explored;
					// if new_node's g smaller than old one
					if (child_node.g < old.g) {
						explored.erase(position_explored);			// delete from explored queue
						insert = 1;
					}
				} else if (position_frontier != frontier.end()) {
					Node old = *position_frontier;
					if (child_node.g < old.g) {
						frontier.erase(position_frontier);			// delete from frontier queue
						insert = 1;
					}
				} else {
					cout << "in both queue" << endl;
				}
				// insert the child_node and sort
				if (insert == 1) {
					frontier.push_back(child_node);					// insert
					sort(frontier.begin(), frontier.end());	// sort
				}
			} else if (method == "A*") {
				int insert = 0;
				// this child node is neither in explored queue nor in frontier queue
				if (position_explored == explored.end() && position_frontier == frontier.end()) {
					insert = 1;																// put in frontier queue
				} else if (position_explored != explored.end()) {
					Node old = *position_explored;
					// if new_node's g smaller than old one
					if (child_node.f < old.f) {
						explored.erase(position_explored);			// delete from explored queue
						insert = 1;
					}
				} else if (position_frontier != frontier.end()) {
					Node old = *position_frontier;
					if (child_node.f < old.f) {
						frontier.erase(position_frontier);			// delete from frontier queue
						insert = 1;
					}
				} else {
					cout << "in both queue" << endl;
				}
				// insert the child_node and sort
				if (insert == 1) {
					frontier.push_back(child_node);					// insert
					sort(frontier.begin(), frontier.end());	// sort
				}
			} else {
				cout<<"not know method"<<endl;
			}
		}
		// put nodes into frontier queue from temp store queue
		if (method == "DFS") {
			while (!store.empty()) {
				frontier.push_front(store.front());
				store.pop_front();
			}
		}
	}
}

void input() {
	ifstream infile("input.txt");
	if (!infile) {
		cout << "error opening infile" << endl;
		exit(1);
	}
	while (!infile.eof()) {
		int number_live, number_sunday, count;
		infile >> method;
		infile >> start_state >> goal_state;
		infile >> number_live;
		for (count = 0; count < number_live; count++) {
			string state1, state2;
			double time;
			unordered_map<string, unordered_map<string,double> >::iterator iter;
			infile >> state1 >> state2 >> time;
			iter = live_traffic.find(state1);
			if (iter == live_traffic.end()) {
				unordered_map<string, double> temp;
				temp.insert(pair<string, double>(state2, time));
				live_traffic.insert(pair<string, unordered_map<string, double> >(state1, temp));
			} else {
				live_traffic[state1].insert(pair<string, double>(state2, time));
			}
		}
		infile>>number_sunday;
		for (count = 0; count < number_sunday; count++) {
			string state;
			double time;
			unordered_map<string, double>::iterator iter;
			infile >> state >> time;
			iter = sunday_traffic.find(state);
			if (iter == sunday_traffic.end()) {
				sunday_traffic.insert(pair<string, double>(state, time));
			} else {
				cout << "duplicate sunday traffic on one same entry" << endl;
			}
		}
	}
}

void output(Node goal) {
	deque<Out> result;
	Out out;
	int count = 0;
	ofstream outfile("output.txt");
	if (! outfile.is_open()) {
		cout << "error opening outfile" << endl;
		exit(1);
	}
	// there is a solution
	if (goal.number != -1) {
		Node current = goal;
		while (current.number != 0) {
			// put in result sequence
			out.state = current.state;
			out.time = current.g;
			result.push_front(out);
			// get parent node
			current = record_node[current.parent_number];
		}
		// put the start state into sequence
		out.state = current.state;
		out.time = current.g;
		result.push_front(out);
		// write result into output.txt
		while (!result.empty()) {
			out = result.front();
			result.pop_front();
			if (method == "BFS" || method == "DFS") {
				out.time = count++;
			}
			outfile << out.state << " " << out.time << endl;
		}
	}
}













