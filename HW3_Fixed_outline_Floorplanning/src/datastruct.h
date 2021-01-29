#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <stdlib.h>
#include <math.h> 
#include <iostream>
#include <vector>
#include <queue>
#include <time.h>
using namespace std;

class Terminal {
private:
	int id;
	double _x, _y;
public:
	Terminal(int, double, double);
	int x(void){ return int(this->_x); }
	int y(void){ return int(this->_y); }
	int _id(void){ return int(this->id); }
	friend class B_star_tree;
};

class Block {
private:
	int id;
	int area, width, height;
	double x, y;
	bool is_rotated;
public:
	Block(int, int, int);
	int get_area(void) { return this->area; }
	void update_contour(vector<int> *contour);
	void rotate(void);
	void x_packing();
	double px() { return this->x; }
	double py() { return this->y; }
	int bid() {return this->id; }
	bool rot() { return this->is_rotated; }
	double available_x() { return this->x + (double)this->width; }
	bool pre_test_bound(Block* , int);
	bool test_x_bound(double, int);
	bool test_y_bound(double, int);
	double ratio(void) { return (double)this->height / (double)this->width;}
	void print_block();
	int get_pin_x(void) { return int(this->x + (this->width/2)); }
	int get_pin_y(void) { return int(this->y + (this->height/2)); }
	friend class B_star_tree;
	friend class Floorplanner;
	friend class Node;
	friend class Net;
};

class Net {
private:
	int _degree;
	vector<Block*> blocklist;
	vector<Terminal*> terminallist;

public:
	Net(int degree) { this->_degree = degree; }
	int degree(void) { return this->_degree; }
	void push_terminal(Terminal* tmp) { this->terminallist.push_back(tmp); }
	void push_block(Block* tmp) { this->blocklist.push_back(tmp); }
	friend class B_star_tree;
};

class B_star_tree {
private:
	int _root_id;
	int _max_x;
	int _max_y;
	vector<vector<int> > tree_list;
	vector<int> contour;
	vector<vector<int> > net_list_blocks;
	vector<vector<int> > net_list_terminals;
	vector<vector<int> > terminal_list;
	static int numBlock;
	static clock_t copy_time;

public:
	B_star_tree();
	void ini_insert_leftchild(Block*, vector<int>*);
	void ini_insert_rightchild(Block*, vector<int>*);
	void x_pack(int current);
	void y_pack(int current);
	void swap(int, int);
	void move(int, int);
	void database_input(vector<Net*>*, unordered_map<int, Terminal*>*);
	static double c_time() { return double(copy_time)/CLOCKS_PER_SEC; }
	static void add_block(void) { numBlock += 1; }
	static int get_block_num(void) { return numBlock; }

	//renew
	void root_pack(int id);
	void set_root(Block*);
	void set_status(Block*);
	void set_parent(int, int);
	void set_leftchild(int, int);
	void set_rightchild(int, int);
	void set_rotate(int, int);
	void rotate_block(int);
	void print_relation(int);
	void rotate(int id) { (this->tree_list.at(id)).at(3) = !(this->tree_list.at(id)).at(3); }
	void reset_contour();
	void update_contour(int);
	void update_position_x(int, int);
	void update_position_y(int);
	void contour_max_x_y();
	void print_all_relation();
	int root(void) { return this->_root_id; }
	int parent_of(int id);
	int leftchild_of(int id);
	int rightchild_of(int id);
	int rotate_status(int id);
	int real_rotate_status(int id) { return this->tree_list.at(id).at(8); }
	int max_x() { return this->_max_x; }
	int max_y() { return this->_max_y; }
	int available_x_of(int id) { return this->tree_list.at(id).at(6) + this->tree_list.at(id).at(4); }
	int x_of(int id) { return this->tree_list.at(id).at(6); }
	int y_of(int id) { return this->tree_list.at(id).at(7); }
	int get_pin_x(int id) { return int(this->x_of(id) + ((this->tree_list.at(id).at(4))/2)); }
	int get_pin_y(int id) { return int(this->y_of(id) + ((this->tree_list.at(id).at(5))/2)); }
	int get_terminal_x(int id) { return this->terminal_list.at(id).at(0); }
	int get_terminal_y(int id) { return this->terminal_list.at(id).at(1); }
	bool detection();
	bool is_leftchild_of(int parent, int child);
	bool is_leaf(int id) { return (this->tree_list.at(id)).at(1) == -1 && (this->tree_list.at(id)).at(2) == -1; }
	bool have_children(int id) { return (this->tree_list.at(id)).at(1) >= 0 && (this->tree_list.at(id)).at(2) >= 0; }
	void complete_binary_tree(unordered_map<int, Block*> *blocklist);
	void print_position();
	void tree_space_release();
	void set_relation(vector<vector<int> >*, int);
	int wirelength();
	vector<vector<int> >* tree_relation_output();

	friend class Floorplanner;
	friend class Net;
};



#endif