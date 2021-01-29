#ifndef GRID_H
#define GRID_H
#include "net.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#include <time.h>
#include <random>
using namespace std;

class Grid {
public:
	Grid(int, int, int, int, int);
	int numHGrid() { return num_h_grid_; }
	int numVGrid() { return num_v_grid_; }
	void status();
	bool available_vertical_move(Net::Bin&, Net::Bin&);
	bool available_horizontal_move(Net::Bin&, Net::Bin&);
	void h_move(Net::Bin&, Net::Bin&, int);
	void v_move(Net::Bin&, Net::Bin&, int);
	void h_grid_status(Net::Bin&, Net::Bin&);
	void v_grid_status(Net::Bin&, Net::Bin&);
	void rip(Net::Bin&, Net::Bin&, int);
	void h_rip(Net::Bin&, Net::Bin&, int);
	void v_rip(Net::Bin&, Net::Bin&, int);
	double h_grid_utility(Net::Bin&, Net::Bin&);
	double v_grid_utility(Net::Bin&, Net::Bin&);
	vector<vector<int> > bottleneck_edge();
	vector<int> chosen_rip_net(vector<vector<int> >&, vector<Net>&, vector<int>&);
	vector<vector<int> > new_chosen_rip_net(vector<vector<int> >&);
	int numOverflow();
	unsigned seed() { return seed_; }



private:
	int h_capacity_;
	int v_capacity_;
	int num_h_grid_;
	int num_v_grid_;
	int numNet;
	unsigned seed_;
	vector<vector<set<int> > > h_link;
	vector<vector<int> > h_channel;
	vector<vector<set<int> > > v_link;
	vector<vector<int> > v_channel;
	friend class Route;
};

#endif