#include "datastruct.h"
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <time.h>



using namespace std;
void fileInput(int argc, char **argv);
void cell_Parse();
void net_Parse();
void initial_cut();
void print_Cell();
void print_Net();
void print_Set();
void print_Bucket();
Cell_node* select_cell_swap_A();
Cell_node* select_cell_swap_B();
void cut_size();
bool balance(Cell_node *node);
bool all_not_locked();
bool net_without_two_cells(Cell_node* node);
bool no_gain_case(vector<Cell_node*> &list);
bool move_A_valid();
bool move_B_valid();
int balance_condition();
void gain();
void update_gain(vector<Cell_node*> &net, Cell_node *base_cell);
void move_cell();
void insert_gain_to_Bucket(vector<Cell_node*> &list);
void build_Bucket();
void end_FM();

int _total_cell_area = 0;	//總面積
int _A_cell_area = 0;		//A 面積
int _B_cell_area = 0;		//B 面積
int max_area = 0;	
int max_pin = 0;
int max_gain;
int current_cut_size;
int min_cut_size;
int initial_cut_size;
float ratio;
int counter_v;
unsigned int pass = 0;
unsigned int best_pass;
fstream File[3];
vector<Cell_node*> cell_list;	//Cell_list
vector<vector<Cell_node*> > net_list;	//Net_list
map<string, Cell_node*> cell_map;	//Cell_map
map<int, map<string, Cell_node*, CmpByPinArea> > new_Bucket;
set<int> A_oset;
set<int> B_oset;
set<int> Best_A_oset;
set<int> Best_B_oset;
vector<string> swap_list;
Cell_node *Only_cell_node_T, *Only_cell_node_F, *Only_cell_node;
clock_t start, ending, update_gain_start, update_gain_end, store_set_start, store_set_end, stroe_set_sum = 0;

void fileInput(int argc, char **argv){
	string inpath  ("..\\testcases\\");
	string outpath   ("..\\output\\");
	string filename;
	for(int index = 1; index < argc-1; index++){
		filename = inpath + filename.assign(argv[index]);
		File[index-1].open(filename, ios::in);
		if(!File[index-1]){
			cerr << "Can't open " << argv[index] << " !" << endl;
			exit(1);
		}
	}
	filename = outpath + filename.assign(argv[3]);
	File[2].open(filename, ios::out);
	if(!File[2]){
		cerr << "Can't open " << argv[3] << " !" << endl;
		exit(1);
	}
	cout << "-------------------testcases: " << argv[1] << " ------------------------" << endl;
	cell_Parse();
	net_Parse();
}

void cell_Parse(){
	string str;
	int area;
	while(File[1] >> str >> area){
		Cell_node *node = new Cell_node;
		node->id = str;
		node->area = area;
		node->pin = 0;
		node->gain = 0;
		node->set_pos = "B";
		node->fixed = false;
		B_oset.insert(atoi(str.substr(1).c_str()));
		Best_B_oset.insert(atoi(str.substr(1).c_str()));
		_total_cell_area += area;
		_B_cell_area += area;
		cell_list.push_back(node);
		cell_map.insert(pair<string, Cell_node*>(str, node));
		max_area = max_area < node->area ? node->area:max_area;
	}
	counter_v = cell_list.size() < 10000 ? 2 : (cell_list.size() < 150000 ? 3 : (cell_list.size() < 200000 ? 3 : 5));
}

void print_Cell(){
	cout << "Print cell..." << endl;
	cout << "cell" << "\t" << "area" << "\t"  << "pin" << "\t" << "gain" << "\t" << "cut_set" << endl;
	vector<int>::iterator ite;
	for(unsigned int i = 0; i < cell_list.size(); i++){
		cout << cell_list[i]->id << "\t" << cell_list[i]->area  << "\t" <<cell_list[i]->pin << "\t";
 		cout << cell_list[i]->gain << "\t" << cell_list[i]->set_pos;
 		cout << endl;
	}
	cout << "---------------------------------------------------------------" << endl;
}

void print_specific_cell(vector<Cell_node*> &list){
	cout << "Print cell..." << endl;
	cout << "cell" << "\t" << "area" << "\t"  << "pin" << "\t" << "gain" << "\t" << "cut_set\t" << "fixed" << endl;
	for(unsigned int i = 0; i < list.size(); i++){
		cout << list[i]->id << "\t" << list[i]->area  << "\t" <<list[i]->pin << "\t";
 		cout << list[i]->gain << "\t" << list[i]->set_pos << "\t" << list[i]->fixed;
 		cout << endl;
	}
	cout << "---------------------------------------------------------------" << endl;
}

void print_Net(){
	vector<Cell_node*>::iterator node;
	cout << "Net" << "\t" << "cell" << endl;
	for(unsigned int i = 0; i < net_list.size(); i++){
		cout << "n" << i+1 << "\t";
		for(node = net_list[i].begin(); node != net_list[i].end(); ++node){
			cout << (*node)->id << "(" << (*node)->area << ")" << " ";
		}
		cout << endl;
	}
}

void print_Set(){
	set<int>::iterator it;
	cout << "set A" << endl;
	for(it = Best_A_oset.begin(); it != Best_A_oset.end(); ++it){
		cout << *it << "\t";
	}
	cout << endl << "-----------------------------" << endl;
	cout << "set B" << endl;
	for(it = Best_B_oset.begin(); it != Best_B_oset.end(); ++it){
		cout << *it << "\t";
	}
}

void print_Bucket(){
	cout << "Print bucket list..." << endl;
	map<int, map<string, Cell_node*, CmpByPinArea> >::reverse_iterator map_it;
	map<string, Cell_node*, CmpByPinArea>::iterator cell_it;
	for ( map_it = new_Bucket.rbegin() ; map_it != new_Bucket.rend(); ++map_it){
		cout << "[" << map_it->first << "]" << "\t";
		cell_it = (map_it->second).begin();
		while(cell_it != ((map_it->second).end())){
			cout << cell_it->first << "[" << (cell_it->second)->set_pos << "]" << "\t";
			++cell_it;
		}
		cout << endl;
	}
	cout << "---------------------------------------------------------------" << endl;
}

void net_Parse(){
	string str, trash;
	vector<Cell_node*> node_vector;
	map<string, Cell_node*>::iterator node;
	int net = 1;
	while(File[0] >> trash){ //NET
		File[0] >> trash; // n?
		File[0] >> trash; // "{"
		while(File[0] >> str && str[0] != '}'){
			node = cell_map.find(str);
			node_vector.push_back(node->second);
			(node->second)->connect_net.push_back(net);
			(node->second)->pin += 1;
			max_pin = (node->second)->pin  > max_pin ? (node->second)->pin : max_pin;
		}
		net_list.push_back(node_vector);
		node_vector.erase(node_vector.begin(), node_vector.end());
		net++;
	}
}

void initial_cut(){
	cout << "Doing the initial cut..." << endl;
	vector<Cell_node*>::iterator it;
	it = cell_list.begin();
	int delta_area = (_A_cell_area) - (_B_cell_area);
	while(it != cell_list.end() && delta_area < 0 && delta_area < int(_total_cell_area / 10)+1){
		if(!net_without_two_cells(*it)){
			++it;
			continue;
		}
		(*it)->set_pos = "A";
		B_oset.erase(atoi((*it)->id.substr(1).c_str()));
		Best_B_oset.erase(atoi((*it)->id.substr(1).c_str()));
		// B_set.erase((*it)->id);
		A_oset.insert(atoi((*it)->id.substr(1).c_str()));
		Best_A_oset.insert(atoi((*it)->id.substr(1).c_str()));
		// A_set.insert((*it)->id);
		_A_cell_area += (*it)->area;
		_B_cell_area -= (*it)->area;
		delta_area = (_A_cell_area) - (_B_cell_area);
		it++;
	}
	// cout <<"|A| = " << _A_cell_area << "\t" << "|B| = "<< _B_cell_area << "\t";
	cut_size();
	min_cut_size = current_cut_size;
	ratio = min_cut_size < 10000 ? 1 : (min_cut_size < 50000 ? 0.1 : (min_cut_size < 100000 ? 0.001 : 0.01));
}

void end_FM(){
	File[0].close();
	File[1].close();
	File[2].close();
}

void gain(){
	// cout << "Computing gain of all cells..." << endl;
	vector<vector<Cell_node*> >::iterator net_it;
	vector<Cell_node*>::iterator v_it;
	int A_num, B_num;
	Cell_node *A_only_Cell, *B_only_Cell;
	for(net_it = net_list.begin(); net_it != net_list.end(); ++net_it){
		A_num = 0;
		B_num = 0;
		for(v_it = (*net_it).begin(); v_it != (*net_it).end(); ++v_it){
			if((*v_it)->set_pos == "A"){
				A_num += 1;
				A_only_Cell = *v_it;
			}
			else{
				B_num += 1;
				B_only_Cell = *v_it;
			}
			if(A_num > 1 && B_num > 1){
				break;
			}
		}
		if(A_num == 0 || B_num == 0){
			for(v_it = (*net_it).begin(); v_it != (*net_it).end(); ++v_it){
				(*v_it)->gain -= 1;
				max_gain = (*v_it)->gain > max_gain ? (*v_it)->gain : max_gain;
			}
		}
		if(A_num == 1){
			A_only_Cell->gain += 1;
			max_gain = A_only_Cell->gain > max_gain ? A_only_Cell->gain : max_gain;
		}
		if(B_num == 1){
			B_only_Cell->gain += 1;
			max_gain = B_only_Cell->gain > max_gain ? B_only_Cell->gain : max_gain;
		}
	}
	insert_gain_to_Bucket(cell_list);
}

void insert_gain_to_Bucket(vector<Cell_node*> &list){
	vector<Cell_node*>::iterator cell_it;
	string key6, set_key;
	for(cell_it = list.begin(); cell_it != list.end(); ++cell_it){
		if((*cell_it)->fixed == true)
			continue;
		set_key = (*cell_it)->set_pos == "A" ? "1" : "0";
		key6 = set_key + "," + to_string((*cell_it)->pin) + "," + to_string((*cell_it)->area) + "," + (*cell_it)->id;
		((new_Bucket.find((*cell_it)->gain))->second).insert(pair<string, Cell_node*>(key6,(*cell_it)));
	}
}

void delete_bucket_gain(vector<Cell_node*> &list){
	vector<Cell_node*>::iterator cell_it;
	string key6, set_key;
	for(cell_it = list.begin(); cell_it != list.end(); ++cell_it){
		if((*cell_it)->fixed == true)
			continue;
		set_key = (*cell_it)->set_pos == "A" ? "1" : "0";
		key6 = set_key + "," + to_string((*cell_it)->pin) + "," + to_string((*cell_it)->area) + "," + (*cell_it)->id;
		((new_Bucket.find((*cell_it)->gain))->second).erase(key6);
	}
}

void update_gain(vector<Cell_node*> &net, Cell_node *base_cell){
	vector<Cell_node*>::iterator v_it;
	int T_num, F_num;
	Cell_node *T_only_Cell, *F_only_Cell;
	string before_set, current_set;
	current_set = base_cell->set_pos;
	before_set = current_set == "A" ? "B" : "A";
	T_num = 0;
	F_num = 0;
	// cout << "status: " << T_num << "\t" << F_num << endl;
	for(v_it = net.begin(); v_it != net.end(); ++v_it){
		// cout << (*v_it)->id << endl;
		if(*v_it == base_cell){
			continue;
		}
			
		if((*v_it)->set_pos == current_set){
			T_num += 1;
			T_only_Cell = *v_it;
		}
		else{
			F_num += 1;
			F_only_Cell = *v_it;
		}
		if(T_num > 1 && F_num > 1){
			break;
		}
	}
	// cout << T_only_Cell->id << endl;
	// cout << "status: " << T_num << "\t" << F_num << endl;
	if(T_num == 0){
		for(v_it = net.begin(); v_it != net.end(); ++v_it){
			if(*v_it == base_cell || (*v_it)->fixed == true){
				// cout << "avoid" << endl;
				continue;
			}
			(*v_it)->gain += 1;
			// cout << (*v_it)->id << "gain + 1" << endl; 
			max_gain = (*v_it)->gain > max_gain ? (*v_it)->gain : max_gain;
		}
	}
	else if(T_num == 1){
		if(T_only_Cell != base_cell && T_only_Cell->fixed == false){
			T_only_Cell->gain -= 1;
			// cout << T_only_Cell->id << "gain - 1" << T_only_Cell->gain << endl; 
			max_gain = T_only_Cell->gain > max_gain ? T_only_Cell->gain : max_gain;	
		}
	}
	if(F_num == 0){
		for(v_it = net.begin(); v_it != net.end(); v_it++){
			if(*v_it == base_cell || (*v_it)->fixed == true){
				// cout << "avoid" << endl;
				continue;
			}
			(*v_it)->gain -= 1;
			// cout << (*v_it)->id << "gain - 1" << endl; 
			max_gain = (*v_it)->gain > max_gain ? (*v_it)->gain : max_gain;
		}
	}
	else if(F_num == 1){
		//F_only_Cell != base_cell && F_only_Cell->fixed == false
		if(F_only_Cell != base_cell && F_only_Cell->fixed == false){
			F_only_Cell->gain += 1;
			// cout << F_only_Cell->id << "gain + 1: " << F_only_Cell->gain << endl; 
			max_gain = F_only_Cell->gain > max_gain ? F_only_Cell->gain : max_gain;
		}
	}
}


int Cell_case(vector<Cell_node*> &net, Cell_node *base_cell){
	vector<Cell_node*>::iterator v_it;
	Cell_node *T_only_Cell, *F_only_Cell;
	string current_set;
	current_set = base_cell->set_pos;
	int T_num, F_num;
	T_num = 0;
	F_num = 0;
	// cout << "status: " << T_num << "\t" << F_num << endl;
	for(v_it = net.begin(); v_it != net.end(); ++v_it){
		// cout << (*v_it)->id << endl;
		if(*v_it == base_cell){
			continue;
		}
			
		if((*v_it)->set_pos == current_set){
			T_num += 1;
			T_only_Cell = *v_it;
		}
		else{
			F_num += 1;
			F_only_Cell = *v_it;
		}
		if(T_num > 1 && F_num > 1){
			return 0;
		}
	}
	if(T_num == 1 && F_num == 1){
		if(T_only_Cell->fixed == false &&  F_only_Cell->fixed == false){
			Only_cell_node_T = T_only_Cell;
			Only_cell_node_F = F_only_Cell;
			return 1;
		}
		else if(T_only_Cell->fixed != false &&  F_only_Cell->fixed != false)
			return 0;
		else{
			Only_cell_node = T_only_Cell->fixed == true ? F_only_Cell : T_only_Cell;
			if(T_only_Cell->fixed == true)
				return 3;
			else
				return 2;
		}
	}
	else if(T_num == 1 && F_num != 1 && F_num != 0){
		if(T_only_Cell->fixed == false){
			Only_cell_node = T_only_Cell;
			return 2;
		}
		else
			return 0;
	}
	else if(T_num != 0 && T_num != 1 && F_num == 1){
		if(F_only_Cell->fixed == false){
			Only_cell_node = F_only_Cell;
			return 3;
		}
		else
			return 0;
	}
	return 4;
}

void move_cell(){
	Cell_node *target_node;
	vector<int>::iterator net_it;
	string key6, set_key;
	// cout << "Moving cell and updating cell gain..." <<endl;
	int threshold = initial_cut_size;
	int case_gain, balance_case, direction = 1;
	pass = 0;
	best_pass = 0;
	while(all_not_locked()){
		while((new_Bucket.find(max_gain))->second.size() == 0){
			max_gain -= 1;
		}
		balance_case = balance_condition();
		switch(balance_case){
			case 0:
				if(direction == 0){
					target_node = select_cell_swap_A();
				}
				else{
					target_node = select_cell_swap_B();
				}
				break;
			case 1:
				target_node = select_cell_swap_B();
				direction = 1;
				break;

			case 2:
				target_node = select_cell_swap_A();
				direction = 0;
				break;
		}
		if(target_node == NULL){
			break;
		}
		set_key = target_node->set_pos == "A" ? "0" : "1";
		key6 = set_key + "," + to_string(target_node->pin) + "," + to_string(target_node->area) + "," + target_node->id;
		((new_Bucket.find(target_node->gain))->second).erase(key6);
		current_cut_size -= target_node->gain;
		if((current_cut_size > threshold && cell_list.size() > 10000)){
			pass++;
			break;
		}
		if(min_cut_size > current_cut_size){
			threshold = current_cut_size + current_cut_size*ratio;
			best_pass = pass;
		}
		min_cut_size = current_cut_size <= min_cut_size ? current_cut_size : min_cut_size;
		for(net_it = target_node->connect_net.begin(); net_it != target_node->connect_net.end(); ++net_it){
			case_gain = Cell_case(net_list[(*net_it)-1], target_node);
			switch(case_gain){
				case 0:
					break;
				case 1:
					set_key = Only_cell_node_T->set_pos == "A" ? "1" : "0";
					key6 = set_key + "," + to_string(Only_cell_node_T->pin) + "," + to_string(Only_cell_node_T->area) + "," + Only_cell_node_T->id;
					((new_Bucket.find(Only_cell_node_T->gain))->second).erase(key6);
					Only_cell_node_T->gain -= 1;
					max_gain = Only_cell_node_T->gain > max_gain ? Only_cell_node_T->gain : max_gain;
					((new_Bucket.find(Only_cell_node_T->gain))->second).insert(pair<string, Cell_node*>(key6, Only_cell_node_T));
					//-------------------------------------------------------------------------------------------------------------
					set_key = Only_cell_node_F->set_pos == "A" ? "1" : "0";
					key6 = set_key + "," + to_string(Only_cell_node_F->pin) + "," + to_string(Only_cell_node_F->area) + "," + Only_cell_node_F->id;
					((new_Bucket.find(Only_cell_node_F->gain))->second).erase(key6);
					Only_cell_node_F->gain += 1;
					max_gain = Only_cell_node_F->gain > max_gain ? Only_cell_node_F->gain : max_gain;
					((new_Bucket.find(Only_cell_node_F->gain))->second).insert(pair<string, Cell_node*>(key6, Only_cell_node_F));
					break;

				case 2:
					set_key = Only_cell_node->set_pos == "A" ? "1" : "0";
					key6 = set_key + "," + to_string(Only_cell_node->pin) + "," + to_string(Only_cell_node->area) + "," + Only_cell_node->id;
					((new_Bucket.find(Only_cell_node->gain))->second).erase(key6);
					Only_cell_node->gain -= 1;
					max_gain = Only_cell_node->gain > max_gain ? Only_cell_node->gain : max_gain;
					((new_Bucket.find(Only_cell_node->gain))->second).insert(pair<string, Cell_node*>(key6, Only_cell_node));
					break;

				case 3:
					set_key = Only_cell_node->set_pos == "A" ? "1" : "0";
					key6 = set_key + "," + to_string(Only_cell_node->pin) + "," + to_string(Only_cell_node->area) + "," + Only_cell_node->id;
					((new_Bucket.find(Only_cell_node->gain))->second).erase(key6);
					Only_cell_node->gain += 1;
					max_gain = Only_cell_node->gain > max_gain ? Only_cell_node->gain : max_gain;
					((new_Bucket.find(Only_cell_node->gain))->second).insert(pair<string, Cell_node*>(key6, Only_cell_node));
					break;

				case 4:
					delete_bucket_gain(net_list[(*net_it)-1]);
					update_gain(net_list[(*net_it)-1], target_node);
					insert_gain_to_Bucket(net_list[(*net_it)-1]);
					break;
			}
		}
		pass++;
	}
}

int balance_condition(){
	int delta_area = (_A_cell_area) - (_B_cell_area);
	if( abs(delta_area) + 2 * max_area > int(_total_cell_area)/10 ){
		if(_A_cell_area < _B_cell_area){
			while(!move_B_valid())
				max_gain -= 1;
			return 1;
		}
		else{
			while(!move_A_valid())
				max_gain -= 1;
			return 2;
		}
	}
	else
		return 0;
}

bool move_B_valid(){
	map<int, map<string, Cell_node*, CmpByPinArea> >::iterator map_it;
	map<string, Cell_node*, CmpByPinArea>::reverse_iterator cell_it;
	int current_gain = max_gain;
	map_it = new_Bucket.find(current_gain);
	cell_it = (map_it->second).rbegin();
	while(cell_it != (map_it->second).rend()){
		if((cell_it->second)->set_pos == "B")
			return true;
		else
			return false;
	}
	return false;
}

bool move_A_valid(){
	map<int, map<string, Cell_node*, CmpByPinArea> >::iterator map_it;
	map<string, Cell_node*, CmpByPinArea>::iterator cell_it;
	int current_gain = max_gain;
	map_it = new_Bucket.find(current_gain);
	cell_it = (map_it->second).begin();
	while(cell_it != (map_it->second).end()){
		if((cell_it->second)->set_pos == "A")
			return true;
		else
			return false;
	}
	return false;
}

Cell_node* select_cell_swap_A(){
	map<int, map<string, Cell_node*, CmpByPinArea> >::iterator map_it;
	map<string, Cell_node*, CmpByPinArea>::iterator cell_it;
	int current_gain = max_gain;
	while(current_gain >= -max_pin){
		// cout << "current gain = " << current_gain << endl;
		map_it = new_Bucket.find(current_gain);
		cell_it = (map_it->second).begin();
		while(cell_it != (map_it->second).end()){
			// cout << "Moving " << cell_it->first << "..." <<endl;
			if(balance(cell_it->second)){
				// cout << "[[[[[select " << (cell_it->second)->id << "!]]]]]" << endl << endl << endl;
				(cell_it->second)->fixed = true;
				swap_list.push_back((cell_it->second)->id);
				if((cell_it->second)->set_pos == "A"){
					_A_cell_area -= (cell_it->second)->area;
					_B_cell_area += (cell_it->second)->area;
					A_oset.erase(atoi((cell_it->second)->id.substr(1).c_str()));
					// A_set.erase((cell_it->second)->id);
					B_oset.insert(atoi((cell_it->second)->id.substr(1).c_str()));
					// B_set.insert((cell_it->second)->id);
					(cell_it->second)->set_pos = "B";
				}
				else{
					_A_cell_area += (cell_it->second)->area;
					_B_cell_area -= (cell_it->second)->area;
					B_oset.erase(atoi((cell_it->second)->id.substr(1).c_str()));
					// B_set.erase((cell_it->second)->id);
					A_oset.insert(atoi((cell_it->second)->id.substr(1).c_str()));
					// A_set.insert((cell_it->second)->id);
					(cell_it->second)->set_pos = "A";
				}
				return cell_it->second;
			}
			// cout << "current gain: " << current_gain << " From A [[[[[Deny " << (cell_it->second)->id << "[" << (cell_it->second)->pin << "," << (cell_it->second)->area << "]!]]]]]"  << endl;
			++cell_it;
		}
		current_gain--;
	}
	return NULL;
}

Cell_node* select_cell_swap_B(){
	map<int, map<string, Cell_node*, CmpByPinArea> >::iterator map_it;
	map<string, Cell_node*, CmpByPinArea>::reverse_iterator cell_it;
	int current_gain = max_gain;
	while(current_gain >= -max_pin){
		// cout << "current gain = " << current_gain << endl;
		map_it = new_Bucket.find(current_gain);
		cell_it = (map_it->second).rbegin();
		while(cell_it != (map_it->second).rend()){
			// cout << "Moving " << cell_it->first << "..." <<endl;
			if(balance(cell_it->second)){
				// cout << "[[[[[select " << (cell_it->second)->id << "!]]]]]" << endl << endl << endl;
				(cell_it->second)->fixed = true;
				swap_list.push_back((cell_it->second)->id);
				if((cell_it->second)->set_pos == "A"){
					_A_cell_area -= (cell_it->second)->area;
					_B_cell_area += (cell_it->second)->area;
					A_oset.erase(atoi((cell_it->second)->id.substr(1).c_str()));
					// A_set.erase((cell_it->second)->id);
					B_oset.insert(atoi((cell_it->second)->id.substr(1).c_str()));
					// B_set.insert((cell_it->second)->id);
					(cell_it->second)->set_pos = "B";
				}
				else{
					_A_cell_area += (cell_it->second)->area;
					_B_cell_area -= (cell_it->second)->area;
					B_oset.erase(atoi((cell_it->second)->id.substr(1).c_str()));
					// B_set.erase((cell_it->second)->id);
					A_oset.insert(atoi((cell_it->second)->id.substr(1).c_str()));
					// A_set.insert((cell_it->second)->id);
					(cell_it->second)->set_pos = "A";
				}
				return cell_it->second;
			}
			// cout << "current gain: " << current_gain << " From B [[[[[Deny " << (cell_it->second)->id << "[" << (cell_it->second)->pin << "," << (cell_it->second)->area << "]!]]]]]"  << endl;
			++cell_it;
		}
		current_gain--;
	}
	return NULL;
}

bool balance(Cell_node *node){
	int delta_area = (_A_cell_area) - (_B_cell_area);
	int target_area = (node->area);
	int rate = 0;
	rate = node->set_pos == "A" ? -2 : 2;
	return abs(delta_area + (rate * target_area)) <= int(_total_cell_area / 10)+1;
}

void build_Bucket(){
	map<string, Cell_node*, CmpByPinArea> *temp;
	// cout << "Initialize bucket list..." << endl;
	for(int i = max_pin; i >= -(max_pin); --i){
		temp = new map<string, Cell_node*, CmpByPinArea>;
		new_Bucket.insert(pair<int, map<string, Cell_node*, CmpByPinArea> >(i, *temp));
	}
}


void cut_size(){
	current_cut_size = 0;
	vector<Cell_node*>::iterator node;
	string pivot;
	for(unsigned int i = 0; i < net_list.size(); ++i){
		// cout << "net " << i << endl;
		node = net_list[i].begin();
		pivot = (*node)->set_pos;
		// cout << pivot << endl;
		for(; node != net_list[i].end(); node++){
			// cout << (*node)->id << "\t" << (*node)->set_pos << endl;
			if((*node)->set_pos != pivot){
				current_cut_size += 1;
				break;
			}
		}
		// cout << "cutsize:\t" << this->current_cut_size << endl;
	}
	initial_cut_size = current_cut_size;
	cout << "cutsize:\t" << current_cut_size << endl;
}

bool all_not_locked(){
	vector<Cell_node*>::iterator v_it;
	for(v_it = cell_list.begin(); v_it != cell_list.end(); ++v_it){
		if((*v_it)->fixed == false)
			return true;
	}
	return false;
}

void fileOutput(){
	string str;
	set<int>::iterator set_it;
	File[2] << "cut_size " << min_cut_size << endl;
	File[2] << "A " << Best_A_oset.size() << endl;
	for(set_it = Best_A_oset.begin(); set_it != Best_A_oset.end(); ++set_it){
		str = "c" + to_string(*set_it);
		File[2] << str << endl;
	}
	File[2] << "B " << Best_B_oset.size() << endl;
	for(set_it = Best_B_oset.begin(); set_it != Best_B_oset.end(); ++set_it){
		str = "c" + to_string(*set_it);
		File[2] << str << endl;
	}
}

void restore_best_set(){
	// cout << "restore best set to initial..." << endl;
	string str;
	int cell_num;
	if(best_pass >= 0){
		for(unsigned int index = 0; index <= best_pass; index++){
			cell_num = atoi((swap_list[index]).substr(1).c_str());
			if(Best_A_oset.find(cell_num) != Best_A_oset.end()){
				Best_A_oset.erase(cell_num);
				Best_B_oset.insert(cell_num);
			}
			else{
				Best_B_oset.erase(cell_num);
				Best_A_oset.insert(cell_num);
			}
		}
	}
}

void restore_iteration_set(){
	string str;
	int cell_num;
	Cell_node* temp;
	store_set_start = clock();
	for(unsigned int index = pass-1; index > best_pass; index--){
		temp = (cell_map.find(swap_list[index]))->second;
		cell_num = atoi((swap_list[index]).substr(1).c_str());
		if(A_oset.find(cell_num) != A_oset.end()){
			_A_cell_area -= temp->area;
			_B_cell_area += temp->area;
			A_oset.erase(cell_num);
			B_oset.insert(cell_num);
			temp->set_pos = "B";
		}
		else{
			_B_cell_area -= temp->area;
			_A_cell_area += temp->area;
			B_oset.erase(cell_num);
			A_oset.insert(cell_num);
			temp->set_pos = "A";
		}
	}
	store_set_end = clock();
	stroe_set_sum += (store_set_end - store_set_start);
}

bool net_without_two_cells(Cell_node* node){
	vector<int>::iterator n_it;
	int counter = 0;
	for(n_it = node->connect_net.begin(); n_it != node->connect_net.end(); n_it++){
		if(net_list[(*n_it)-1].size() == 2){
			counter++;
		}
		if(counter > counter_v){
			return false;
		}
	}
	return true;
}

void clean_bucket(){
	for(int i = max_pin; i >= -(max_pin); --i){
		(new_Bucket.find(i)->second).clear();
	}
}

void unlock_reset_cells(){
	// cout << "reset all cells..." << endl;
	vector<int>::iterator ite;
	for(unsigned int i = 0; i < cell_list.size(); i++){
		cell_list[i]->fixed = false;
		cell_list[i]->gain = 0;
	}
}

void iteration_FM(){
	int new_cut_size, last_cut_size, i;
	i = 1;
	while(1){
		// cout << "------Move " << i << "------" << endl;
		last_cut_size = min_cut_size;
		gain();
		move_cell();
		restore_best_set();
		restore_iteration_set();
		cut_size();
		swap_list.clear();
		unlock_reset_cells();
		clean_bucket();
		new_cut_size = min_cut_size;
		i++;
		if(last_cut_size <= new_cut_size){
			break;
		}
	}
}

int main(int argc, char **argv){
	ios_base::sync_with_stdio(false);
	start = clock();
	fileInput(argc, argv);
	update_gain_start = clock();
	initial_cut();
	build_Bucket();
	cout << "Doing FM algorithm..." << endl;
	iteration_FM();
	update_gain_end = clock();
	fileOutput();
	ending = clock();
	cout << "Done!" << endl;
	cut_size();
	int delta_area = abs(_A_cell_area - _B_cell_area);
	cout << "Area:\t"<< _A_cell_area << "\t" << _B_cell_area << "\t" << delta_area << "\t" << int(_total_cell_area/10) << endl;
	cout << "--------------------------------------------------------------------------------------------" << endl;
	cout << "Min cut size:\t"<< min_cut_size << endl;
	cout << "calculate I/O time: " << double(update_gain_start - start + ending - update_gain_end)/CLOCKS_PER_SEC << " sec.\n";
	cout << "calculate FM time: " << double(update_gain_end - update_gain_start)/CLOCKS_PER_SEC << " sec.\n" ;


	end_FM();
	return 0;
}