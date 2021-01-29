#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H

#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>
using namespace std;


typedef struct{
	string id;
	int area;
	int pin;
	int gain;
	bool fixed;
	string set_pos;
	vector<int> connect_net;
} Cell_node;

typedef struct{
	string cell_id;
	Cell_node* itself;
} Net_node;

struct CmpByPinArea {
	bool operator()(const string& k1, const string& k2) {
  		int current = 0;
  		int pos;
  		int k1_num[4], k2_num[4];
  		for(int index = 0; index < 3; index++){
  			pos = k1.find_first_of(",", current);
  			k1_num[index] = atoi(k1.substr(current, pos - current).c_str());
  			current = pos + 1;
  		}
  		current++;
  		k1_num[3] = atoi(k1.substr(current).c_str());
  		current = 0;
  		for(int index = 0; index < 3; index++){
  			pos = k2.find_first_of(",", current);
  			k2_num[index] = atoi(k2.substr(current, pos - current).c_str());
  			current = pos + 1;
  		}
  		current++;
  		k2_num[3] = atoi(k2.substr(current).c_str());
  		// return (k1_num[1] > k2_num[1] || k1_num[1] < k2_num[1])?k1_num[1] > k2_num[1]:((k1_num[2] > k2_num[2] || k1_num[2] < k2_num[2])?k1_num[2] < k2_num[2]:k1_num[3] > k2_num[3]);
  		if(k1_num[0] > k2_num[0]){
  			return true;
  		}
  		else if(k1_num[0] == k2_num[0] && k1_num[0] == 1){
  			return (k1_num[1] > k2_num[1] || k1_num[1] < k2_num[1])?\
  		 	k1_num[1] > k2_num[1] : ((k1_num[2] > k2_num[2] || k1_num[2] < k2_num[2])? k1_num[2] < k2_num[2] : k1_num[3] < k2_num[3]);
  		}
  		else if(k1_num[0] == k2_num[0] && k1_num[0] == 0){
  			return (k1_num[1] > k2_num[1] || k1_num[1] < k2_num[1])?\
  		 	k1_num[1] < k2_num[1] : ((k1_num[2] > k2_num[2] || k1_num[2] < k2_num[2])? k1_num[2] > k2_num[2] : k1_num[3] > k2_num[3]);
  		}
  		else{
  			return false;
  		}
  	}
};


// class FM {
// private:
// 	int _total_cell_area;	//總面積
// 	int _A_cell_area;		//A 面積
// 	int _B_cell_area;		//B 面積
// 	int max_area;
// 	int max_pin;
// 	int max_gain;
// 	int current_cut_size;
// 	int min_cut_size;
// 	fstream File[3];
// 	vector<Cell_node*> cell_list;	//Cell_list
// 	vector<vector<Cell_node*> > net_list;	//Net_list
// 	map<string, Cell_node*> cell_map;	//Cell_map
// 	map<int, map<string, Cell_node*> > new_Bucket;
// 	set<string> A_set;	//A set
// 	set<string> B_set;	//B set


// public:
// 	FM(int argc, char **argv);
// 	void fileInput(int argc, char **argv);
// 	void cell_Parse();
// 	void net_Parse();
// 	void initial_cut();
// 	void print_Cell();
// 	void print_Net();
// 	void print_Set();
// 	void print_Bucket();
// 	Cell_node* select_cell_swap();
// 	void cut_size();
// 	bool balance(Cell_node *node);
// 	bool all_not_locked();
// 	void build_Bucket();
// 	void end_FM();
// };

#endif


