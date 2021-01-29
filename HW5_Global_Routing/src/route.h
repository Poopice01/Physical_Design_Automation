#ifndef ROUTE_H
#define ROUTE_H
#include "net.h"
#include "grid.h"
#include <algorithm>
#include <queue>
#include <random>
#include <time.h>
#include <limits>
#include <set>

using namespace std;

class Route {
public:
	Route(Grid, vector<Net>);
	/////////////////////////////////////////
	// test function
	void status();
	vector<int> boundingStatus(Net::Bin&, Net::Bin&);
	vector<int> boundingStatus(Net::Bin&, Net::Bin&, double);
	vector<int> boundingStatus(Net::Bin&, Net::Bin&, double, int);
	Net& net(int index) { return net_.at(index); }
	void debug_trace(vector<Net::Bin>&);
	vector<Net> output();
	void overflow_status(vector<int>&);
	/////////////////////////////////////////
	// algorithm
	int initial_route();
	int re_route(double, int);
	void random_shuffle();
	void lee_algorithm(Net&, double);
	void second_route(Net&, double);
	void first_route(Net&, double);
	void detour_route(Net&, double, int);
	void filling(vector<Net::Bin>&, Net::Bin&, Net::Bin&);
	void filling(vector<Net::Bin>&, Net::Bin&, Net::Bin&, Net::Bin&);
	void filling(vector<Net::Bin>&, Net::Bin&, Net::Bin&, Net::Bin&, int);
	void rip(Net&);
	vector<Net::Bin> search(Net::Bin&, vector<int>&, Net::Bin&);
	vector<Net::Bin> retrace(Net&, Net::Bin&, vector<int>&, double);
	vector<Net::Bin> retrace(Net&, Net::Bin&, vector<int>&, double, int);
	int min_direction(Net&, vector<int>&, int&, Net::Bin&, vector<Net::Bin>&, vector<int>&);
	int detour_direction(Net&, vector<double>&, int&, Net::Bin&, vector<Net::Bin>&, vector<int>&, Net::Bin&, set<int>&); 
	bool addCost(Net::Bin&, Net::Bin&); 
	int utility_direction(Net&, vector<int>&, Net::Bin&, vector<Net::Bin>&, vector<int>&, Net::Bin&);
	int rip_up_and_reroute(void);
	vector<Net::Bin> dommy_Access(Net& net, Net::Bin& start, Net::Bin& end, double alpha);
	vector<Net::Bin> dummyCreate(Net& current, int ineratia, int shift_unit);
	bool overflow_path(Net& current);
	static bool cmp(Net a, Net b) { return a.HPWL() > b.HPWL(); }
	/////////////////////////////////////////
	// map
	void map_initial();
	void pre_label(Net::Bin&, Net::Bin&);
	void m_access(int, int, int, Net::Bin&, Net::Bin&);
	void m_access(Net::Bin&, int, Net::Bin&, Net::Bin&);
	void m_access(Net::Bin&, int, Net::Bin&, Net::Bin&, Net::Bin&);
	void a_m_access(Net::Bin& target, int label, Net::Bin& source, Net::Bin& start, Net::Bin& end);
	int map_value(int row, int col) { return map_.at(row).at(col); }
	int map_value(Net::Bin& current) { return map_.at(current.y()).at(current.x()); }


private:
	Grid grid_;
	vector<Net> net_;
	vector<vector<int> > map_;
	vector<int> index_;
	int left_;
	int right_;
	int top_;
	int bottom_;
};

#endif
