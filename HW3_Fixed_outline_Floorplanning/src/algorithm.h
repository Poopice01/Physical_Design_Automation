#ifndef _ALGORITHM_H
#define _ALGORITHM_H
#include "datastruct.h"
#include <string>


class Floorplanner {
private:
	vector<int> h_contour;
	vector<vector<int> > local_list;
	vector<vector<int> > best_list;
	int local_root;
	int best_root;
	int boundary;
	int _max_x;
	int _max_y;
	int _area;
	int max_area;
	int min_area;
	int max_wirelength;
	int min_wirelength;
	int feasible_min_wirelength;
	double min_dead_space_rate;
	double _floorplan_ratio;
	double _average_uphill_cost;
	double _average_wirelength;
	double _average_area;
	int _wirelength;
	unsigned seed;
	clock_t total_copy_time, terminate_start, total_initialize, total_wirelength_time, total_pert_time, total_packing_time;
	vector<Block*> best_rotate;
	vector<Block*> best_wirelength_rotate;
	vector<Block*> best_cost_rotate;
	unordered_map<int, Block*> *blist;
	unordered_map<int, Terminal*> *tlist;
	vector<Net*> *nlist;
	vector<int> rotate_list;
	B_star_tree tree;
	B_star_tree best_wirelength_tree;
	B_star_tree best_dead_space_ratio_tree;
	B_star_tree best_tree;
	B_star_tree local_tree;
	static int total_block_area;
	static int numBlock;

public:
	Floorplanner(unordered_map<int, Block*>* blocklist, unordered_map<int, Terminal*>* tlist, vector<Net*>* nlist, int boundary);
	void initialize_in_x_bound(); // Initialize solution in x bound
	void initialize_min_max();
	void complete_tree_initial();
	static void add_area(int area) { total_block_area += area; }
	static int get_total_area(void) { return total_block_area; }
	static void add_block(void) { numBlock += 1; }
	static int get_block_num(void) { return numBlock; }
	// wirelength method
	void reset_wirelength(void) { this->_wirelength = 0; }
	void calculate_wirelength(void);
	int wirelength(void) { return this->_wirelength; }
	// contour method 
	void update_contour(Block*);
	void reset_contour();
	vector<int>* get_contour() { return &(this->h_contour); }
	// method for SA
	void preparation_for_SA(void);
	void FastSA();
	void classic_SA();
	void perturbation();
	void packing();
	void update_ratio_and_area(void);
	double calculate_cost(double, double);
	double dead_space_rate();
	void min_max_check(void);
	bool is_feasible();
	void record_best_w_rotate();
	void record_best_r_rotate();
	void record_best_cost_rotate();
	void restore_best_tree();
	void restore_rotate(vector<Block*>& rotate_block_list);
	// method for test and test
	vector<vector<int> >* output_database();
	double w_time(void) { return double(this->total_wirelength_time) / CLOCKS_PER_SEC; }
	double packing_time(void) { return double(this->total_packing_time) / CLOCKS_PER_SEC; }
	double perturb_time(void) { return double(this->total_pert_time) / CLOCKS_PER_SEC; }
	double ini_time(void) { return double(this->total_initialize) / CLOCKS_PER_SEC; }
	void print_position();
	void print_h_contour();
	void print_status(void);
	void tree_copy_test();
	unsigned return_seed() { return this->seed; }
	double dsr() { return this->min_dead_space_rate; }

	friend class B_star_tree;
};



#endif