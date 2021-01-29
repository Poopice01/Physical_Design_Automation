#ifndef NET_H
#define NET_H
#include <vector>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;


class Net {
private:
	class Bin {
	public:
		Bin(int, int);
		void position(void);
		int x() { return x_; }
		int y() { return y_; }
		int cost() { return cost_; }
		bool out_of_bound(vector<int>& b) { return x_ > b[3] || x_ < b[2] || y_ > b[0] || y_ < b[1]; }
		bool arrive(Bin& bin) { return bin.x() == x_ && bin.y() == y_; }
		void add_cost(int num) { cost_ = num; }
		bool operator==(const Bin&);
		int operator-(const Bin&);
		int operator>>(const Bin&);
		// bool operator> (const Bin&);
		bool operator<(const Bin&);
		friend bool operator>(const Bin& lhs, const Bin& rhs) { return lhs.cost_ > rhs.cost_; }
	private:
		int x_, y_;
		int cost_;
	};
	int id_;
	int pin_;
	Bin start_;
	Bin end_;
	bool overflow_;
	bool routed_;
	bool ineratia_;
	int hpwl_;

public:
	Net(int, int, int, int, int, int);
	void status(void);
	int HPWL(void);
	void congestion() { overflow_ = true; }
	bool ineratia() { return ineratia_; }
	void flip() { ineratia_ = !ineratia_; }
	void resolve() { overflow_ = false; }
	void route_finished() { routed_ = true; }
	void rip() { routed_ = false; }
	bool overflow() { return overflow_; }
	bool routed() { return routed_; }
	static bool cmp(const Net& a, const Net& b) { return a.id_ < b.id_; }
	static bool cmpHPWL(const Net& a, const Net& b) { return a.hpwl_ > b.hpwl_; }
	void turn();
	int id() { return id_;}
	Bin& start() { return start_; }
	Bin& terminal() { return end_; }
	vector<Bin> path;
	friend class Route;
	friend class Grid;


};

#endif