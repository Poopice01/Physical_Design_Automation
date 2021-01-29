#include "route.h"
#include <iomanip>

using namespace std;

Route::Route(Grid g, vector<Net> n)
	:grid_(g), net_(n)
{
	index_.resize(net_.size());
	left_ = 0;
	right_ = grid_.numHGrid() - 1;
	bottom_ = 0;
	top_ = grid_.numVGrid() - 1;
}

void Route::status(){
	grid_.status();
	// for(auto net = net_.begin(); net != net_.end(); ++net){
	// 	(*net).status();
	// }
	cout << "Map: \n";
	for(auto row = map_.begin(); row != map_.end(); ++row){
		for(auto step = (*row).begin(); step != (*row).end(); ++step)
			cout << (*step) << " ";
	}
	cout << endl;
}

vector<int> Route::boundingStatus(Net::Bin& S, Net::Bin& T, double alpha){
	int upper = max(S.y(), T.y());
	int lower = upper == S.y() ? T.y() : S.y();
	int left = min(S.x(), T.x());
	int right = left == S.x() ? T.x() : S.x();
	//////////////////////////////////////////
	// alpha% area bound
	upper += static_cast<int>(abs(S.y() - T.y()) * alpha);
	lower -= static_cast<int>(abs(S.y() - T.y()) * alpha);
	left -= static_cast<int>(abs(S.x() - T.x()) * alpha);
	right += static_cast<int>(abs(S.x() - T.x()) * alpha);
	upper = upper > top_ ? top_ : upper;
	lower = lower < bottom_ ? bottom_ : lower;
	left = left < left_ ? left_ : left;
	right = right > right_ ? right_ : right;
	vector<int> bound = {upper, lower, left, right};
	//////////////////////////////////////////
	// test
	// cout << "upper: " << upper << endl;
	// cout << "lower: " << lower << endl;
	// cout << "left: " << left << endl;
	// cout << "right: " << right << endl;
	// int index = upper;
	// for(int row = upper; row >= lower; --row){
	// 	cout << "row[" << upper << "]\t";
	// 	--upper;
	// 	for(int col = left; col <= right; ++col){
	// 		cout << map_value(row, col) << "\t";
	// 	}
	// 	cout << endl;
	// }
	// cout << "---------------------------\n";
	//////////////////////////////////////////
	return bound;
}

vector<int> Route::boundingStatus(Net::Bin& S, Net::Bin& T, double alpha, int shift_unit){
	int upper = max(S.y(), T.y());
	int lower = upper == S.y() ? T.y() : S.y();
	int left = min(S.x(), T.x());
	int right = left == S.x() ? T.x() : S.x();
	//////////////////////////////////////////
	// alpha% area bound
	if(shift_unit < 99){
		upper += shift_unit;
		lower -= shift_unit;
		left -= shift_unit;
		right += shift_unit;
		upper = upper > top_ ? top_ : upper;
		lower = lower < bottom_ ? bottom_ : lower;
		left = left < left_ ? left_ : left;
		right = right > right_ ? right_ : right;	
	}
	else{
		upper = top_;
		lower = bottom_;
		left = left_;
		right = right_;
	}
	
	vector<int> bound = {upper, lower, left, right};
	//////////////////////////////////////////
	// test
	// cout << "upper: " << upper << endl;
	// cout << "lower: " << lower << endl;
	// cout << "left: " << left << endl;
	// cout << "right: " << right << endl;
	// int index = upper;
	// for(int row = upper; row >= lower; --row){
	// 	cout << "row[" << upper << "]\t";
	// 	--upper;
	// 	for(int col = left; col <= right; ++col){
	// 		cout << map_value(row, col) << "\t";
	// 	}
	// 	cout << endl;
	// }
	// cout << "---------------------------\n";
	//////////////////////////////////////////
	return bound;
}


void Route::map_initial(){
	map_.clear();
	vector<int> tmp(grid_.numHGrid(), 0);
	map_.resize(grid_.numVGrid(), tmp);
	// cout << "initializing finished..\n";
}

void Route::m_access(int x, int y, int label, Net::Bin& source, Net::Bin& start){
	int origin = map_value(source.y(), source.x());
	if(source == start){
		map_.at(y).at(x) += label;	
	}
	else{
		map_.at(y).at(x) += (origin + label);
	}
	
}

void Route::m_access(Net::Bin& target, int label, Net::Bin& source, Net::Bin& start){
	int origin = map_value(source.y(), source.x());
	if(source == start){
		map_.at(target.y()).at(target.x()) += label;	
	}
	else{
		map_.at(target.y()).at(target.x()) += (origin + label);	
	}
	
}

void Route::m_access(Net::Bin& target, int label, Net::Bin& source, Net::Bin& start, Net::Bin& end){
	int origin = map_value(source.y(), source.x());
	double utility = source - target ? grid_.h_grid_utility(source, target) : grid_.v_grid_utility(source, target);
	// if(target == Net::Bin(40, 63)){
	// 	source.position();
	// 	cout << " -> ";
	// 	cout << "(40, 63) utility:" << utility << endl;
	// }
	int penalty = utility >= 1 ? (top_ - bottom_) * 100 + ( right_ - left_) * 100 : (utility) * 100;
	// if(target.x() == 50 && target.y() == 62){
	// 	cout << "origin: " << origin << endl;
	// 	cout << "utility: " << utility << endl;
	// 	cout << "penalty: " << penalty << endl;
	// }
	if(source == start){
		// if(source - target == 0){
		// 	map_.at(target.y()).at(target.x()) += (label + (target >> end) + (grid_.v_grid_utility(source, target) * grid_.v_capacity_));
		// 	// if(grid_.v_grid_utility(source, target) >= 1)
		// 	// 	map_.at(target.y()).at(target.x()) = 99;
		// }
		// else{
		// 	map_.at(target.y()).at(target.x()) += (label + (target >> end) + (grid_.h_grid_utility(source, target) * grid_.h_capacity_));
		// 	// if(grid_.h_grid_utility(source, target) >= 1)
		// 	// 	map_.at(target.y()).at(target.x()) = 99;
		// }
		map_.at(target.y()).at(target.x()) += (label + penalty);
	}
	else{
		// if(source - target == 0){
		// 	map_.at(target.y()).at(target.x()) += (origin + label + (target >> end) + (grid_.v_grid_utility(source, target) * grid_.v_capacity_));
		// 	// if(grid_.v_grid_utility(source, target) >= 1)
		// 	// 	map_.at(target.y()).at(target.x()) = 99;
		// }
		// else{
		// 	map_.at(target.y()).at(target.x()) += (origin + label + (target >> end) + (grid_.h_grid_utility(source, target) * grid_.h_capacity_));
		// 	// if(grid_.h_grid_utility(source, target) >= 1)
		// 	// 	map_.at(target.y()).at(target.x()) = 99;
		// }
		map_.at(target.y()).at(target.x()) += (origin + label + penalty);
	}
	target.add_cost(map_.at(target.y()).at(target.x()));
}

void Route::a_m_access(Net::Bin& target, int label, Net::Bin& source, Net::Bin& start, Net::Bin& end){
	int origin = map_value(source.y(), source.x()) - (source >> end);
	// double utility = source - target ? grid_.h_grid_utility(source, target) : grid_.v_grid_utility(source, target);
	if(source == start){
		map_.at(target.y()).at(target.x()) += (label + (target >> end));
	}
	else{
		map_.at(target.y()).at(target.x()) += (origin + label + (target >> end));
	}
	target.add_cost(map_.at(target.y()).at(target.x()));
}

void Route::random_shuffle(){
	// unsigned seed = time(NULL);
	// unsigned seed = 1611219738;
	cout << "seed: " << grid_.seed() << endl;
	auto rng = default_random_engine(grid_.seed());
	shuffle(begin(net_), end(net_), rng);
	for(int index = 0; index < net_.size(); ++index){
		index_.at(net_.at(index).id()) = index;
	}
}

void Route::pre_label(Net::Bin& S, Net::Bin& T){
	m_access(S.x(), S.y(), -2, S, S); // start label -2
	m_access(T.x(), T.y(), -3, T, T); // terminal label -3
}

void Route::filling(vector<Net::Bin>& bins, Net::Bin& source, Net::Bin& start){
	for(auto bin = bins.begin(); bin != bins.end(); ++bin){
		m_access((*bin), 1, source, start);
	}
}

void Route::filling(vector<Net::Bin>& bins, Net::Bin& source, Net::Bin& start, Net::Bin& end){
	for(auto bin = bins.begin(); bin != bins.end(); ++bin){
		m_access((*bin), 1, source, start, end);
	}
}

void Route::filling(vector<Net::Bin>& bins, Net::Bin& source, Net::Bin& start, Net::Bin& end, int mode){
	for(auto bin = bins.begin(); bin != bins.end(); ++bin){
		a_m_access((*bin), 1, source, start, end);
	}
}

void Route::lee_algorithm(Net& current, double alpha){
	queue<Net::Bin> unvisited;
	map_initial();
	pre_label(current.start(), current.terminal());
	vector<int> boundary = boundingStatus(current.start(), current.terminal(), alpha);
	unvisited.push(current.start());
	bool arrive = false;
	while(!unvisited.empty() && !arrive){
		Net::Bin current_bin = unvisited.front();
		unvisited.pop();
		vector<Net::Bin> candidate = search(current_bin, boundary, current.start());
		for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
			if((*bin).arrive(current.terminal())){
				arrive = true;
			}
		}
		if(!arrive){
			filling(candidate, current_bin, current.start());
			for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
				unvisited.push((*bin));
			}	
		}
		arrive = false;
	}
	current.path = retrace(current, current.terminal(), boundary, alpha);
}

void Route::first_route(Net& current, double alpha){
	priority_queue<Net::Bin, vector<Net::Bin>, greater<Net::Bin> > pq_unvisited;
	map_initial();
	pre_label(current.start(), current.terminal());
	vector<int> boundary = boundingStatus(current.start(), current.terminal(), alpha, 2);
	pq_unvisited.push(current.start());
	bool arrive = false;
	while(!pq_unvisited.empty() && !arrive){
		Net::Bin current_bin = pq_unvisited.top();
		// if(current.id() == 12685){
		// 	current_bin.position();
		// 	cout << " ";
		// 	cout << current_bin.cost() << endl;
		// }
		pq_unvisited.pop();
		vector<Net::Bin> candidate = search(current_bin, boundary, current.start());
		for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
			if((*bin).arrive(current.terminal())){
				arrive = true;
			}
		}
		if(!arrive){
			filling(candidate, current_bin, current.start(), current.terminal(), 1);
			for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
				pq_unvisited.push((*bin));
			}	
		}
		arrive = false;
	}
	// cout << "net" << current.id() << endl;
	// cout << "S: "; 
	// current.start().position();
	// cout << endl;
	// cout << "T: "; 
	// current.terminal().position();
	// cout << endl;
	// if(current.id() == 5192){
	// 	int shift = 5;
	// 	int upper = max(current.terminal().y(), current.start().y());
	// 	int lower = upper == current.terminal().y() ? current.start().y() : current.terminal().y();
	// 	int left = min(current.terminal().x(), current.start().x());
	// 	int right = left == current.terminal().x() ? current.start().x() : current.terminal().x();
	// 	upper = upper + shift > top_ ? top_ : upper + shift;
	// 	lower = lower - shift < 0 ? bottom_ : lower - shift;
	// 	left = left - shift < 0 ? left_ : left - shift;
	// 	right = right + shift > right_ ? right_ : right + shift;
	// 	// right = 53;
	// 	// int upper = top_;
	// 	// int lower = bottom_;
	// 	// int left = left_;
	// 	// int right = right_;
	// 	//////////////////////////////////////////
	// 	// alpha% area bound
	// 	// upper += static_cast<int>(abs(current.start().y() - current.terminal().y()) * alpha);
	// 	// lower -= static_cast<int>(abs(current.start().y() - current.terminal().y()) * alpha);
	// 	// left -= static_cast<int>(abs(current.start().x() - current.terminal().x()) * alpha);
	// 	// right += static_cast<int>(abs(current.start().x() - current.terminal().x()) * alpha);
	// 	upper = upper > top_ ? top_ : upper;
	// 	lower = lower < bottom_ ? bottom_ : lower;
	// 	left = left < left_ ? left_ : left;
	// 	right = right > right_ ? right_ : right;
	// 	int index = upper;
	// 	for(int row = upper; row >= lower; --row){
	// 		cout << "row[" << upper << "]\t";
	// 		--upper;
	// 		for(int col = left; col <= right; ++col){
	// 			cout << setw(4) << setfill('0') << map_value(row, col) << " ";
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << "---------------------------\n";
	// }
	// int x;
	current.path = retrace(current, current.terminal(), boundary, alpha, 3);
	// debug_trace(current.path);
	// cin >> x;
}

void Route::second_route(Net& current, double alpha){
	// queue<Net::Bin> unvisited;
	priority_queue<Net::Bin, vector<Net::Bin>, greater<Net::Bin> > pq_unvisited;
	map_initial();
	pre_label(current.start(), current.terminal());
	vector<int> boundary = boundingStatus(current.start(), current.terminal(), alpha, 99);
	pq_unvisited.push(current.start());
	bool arrive = false;
	while(!pq_unvisited.empty() && !arrive){
		Net::Bin current_bin = pq_unvisited.top();
		// if(current.id() == 12685){
		// 	current_bin.position();
		// 	cout << " ";
		// 	cout << current_bin.cost() << endl;
		// }
		pq_unvisited.pop();
		vector<Net::Bin> candidate = search(current_bin, boundary, current.start());
		for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
			if((*bin).arrive(current.terminal()) && !(current_bin == current.start()) ){
				arrive = true;
			}
		}
		if(!arrive){
			filling(candidate, current_bin, current.start(), current.terminal());
			for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
				pq_unvisited.push((*bin));
			}	
		}
		arrive = false;
	}
	// cout << "net" << current.id() << endl;
	// cout << "S: "; 
	// current.start().position();
	// cout << endl;
	// cout << "T: "; 
	// current.terminal().position();
	// cout << endl;
	// if(current.id() == 5192){
	// 	int shift = 5;
	// 	int upper = max(current.terminal().y(), current.start().y());
	// 	int lower = upper == current.terminal().y() ? current.start().y() : current.terminal().y();
	// 	int left = min(current.terminal().x(), current.start().x());
	// 	int right = left == current.terminal().x() ? current.start().x() : current.terminal().x();
	// 	upper = upper + shift > top_ ? top_ : upper + shift;
	// 	lower = lower - shift < 0 ? bottom_ : lower - shift;
	// 	left = left - shift < 0 ? left_ : left - shift;
	// 	right = right + shift > right_ ? right_ : right + shift;
	// 	// right = 53;
	// 	// int upper = top_;
	// 	// int lower = bottom_;
	// 	// int left = left_;
	// 	// int right = right_;
	// 	//////////////////////////////////////////
	// 	// alpha% area bound
	// 	// upper += static_cast<int>(abs(current.start().y() - current.terminal().y()) * alpha);
	// 	// lower -= static_cast<int>(abs(current.start().y() - current.terminal().y()) * alpha);
	// 	// left -= static_cast<int>(abs(current.start().x() - current.terminal().x()) * alpha);
	// 	// right += static_cast<int>(abs(current.start().x() - current.terminal().x()) * alpha);
	// 	upper = upper > top_ ? top_ : upper;
	// 	lower = lower < bottom_ ? bottom_ : lower;
	// 	left = left < left_ ? left_ : left;
	// 	right = right > right_ ? right_ : right;
	// 	int index = upper;
	// 	for(int row = upper; row >= lower; --row){
	// 		cout << "row[" << upper << "]\t";
	// 		--upper;
	// 		for(int col = left; col <= right; ++col){
	// 			cout << setw(4) << setfill('0') << map_value(row, col) << " ";
	// 		}
	// 		cout << endl;
	// 	}
	// 	cout << "---------------------------\n";
	// }
	// int x;
	current.path = retrace(current, current.terminal(), boundary, alpha, 3);
	// debug_trace(current.path);
	// cin >> x;
}

void Route::detour_route(Net& current, double alpha, int shift_unit){
	// cout << "origin path: \n";
	// debug_trace(current.path);
	Net::Bin dommyT = Net::Bin(-1, -1);
	Net::Bin dommyS = Net::Bin(-1, -1);
	// for(auto pt = current.path.begin(); pt != current.path.end(); ++pt){
	// 	(*pt).position();
	// 	cout << endl;
	// }
	auto next_pt = current.path.begin();
	++next_pt;
	bool originDirection = (*next_pt) - current.terminal();
	// cout << "originDirection: " << originDirection << endl;
	vector<Net::Bin> Ds = dummyCreate(current, originDirection, shift_unit);
	current.path.clear();
	// cout << "net" << current.id() << endl;
	dommyT = Ds[1];
	dommyS = Ds[0];
	// cout << "dT: "; 
	// dommyT.position();
	// cout << endl;
	// cout << "dS: "; 
	// dommyS.position();
	// cout << endl;
	int dummyDirection = dommyS - current.start();
	// cout << "direction:" << dummyDirection << endl;
	queue<Net::Bin> unvisited;
	map_initial();
	pre_label(dommyS, dommyT);
	vector<int> bound = boundingStatus(dommyS, dommyT, 0);
	unvisited.push(dommyS);
	bool arrive = false;
	while(!unvisited.empty() && !arrive){
		Net::Bin current_bin = unvisited.front();
		// current_bin.position();
		// cout << endl;
		unvisited.pop();
		vector<Net::Bin> candidate = search(current_bin, bound, dommyS);
		for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
			// // cout << "candidate: ";
			// (*bin).position();
			// cout << endl;
			if((*bin).arrive(dommyT)){
				// cout << "arrive!\n";
				arrive = true;
			}
		}
		if(!arrive){
			// cout << "not arrive!" << endl;
			filling(candidate, current_bin, dommyS);
			for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
				unvisited.push((*bin));
			}	
		}
		arrive = false;

	}
	// cout << "see" << endl;
	// int upper = max(dommyT.y(), dommyS.y());
	// int lower = upper == dommyT.y() ? dommyS.y() : dommyT.y();
	// int left = min(dommyT.x(), dommyS.x());
	// int right = left == dommyT.x() ? dommyS.x() : dommyT.x();
	// //////////////////////////////////////////
	// // alpha% area bound
	// // upper += static_cast<int>(abs(current.start().y() - current.terminal().y()) * alpha);
	// // lower -= static_cast<int>(abs(current.start().y() - current.terminal().y()) * alpha);
	// // left -= static_cast<int>(abs(current.start().x() - current.terminal().x()) * alpha);
	// // right += static_cast<int>(abs(current.start().x() - current.terminal().x()) * alpha);
	// upper = upper > top_ ? top_ : upper;
	// lower = lower < bottom_ ? bottom_ : lower;
	// left = left < left_ ? left_ : left;
	// right = right > right_ ? right_ : right;
	// int index = upper;
	// for(int row = upper; row >= lower; --row){
	// 	cout << "row[" << upper << "]\t";
	// 	--upper;
	// 	for(int col = left; col <= right; ++col){
	// 		cout << map_value(row, col) << " ";
	// 	}
	// 	cout << endl;
	// }
	// cout << "---------------------------\n";
	current.path = retrace(current, dommyT, bound, alpha, dummyDirection);
	auto dummySPath = dommy_Access(current, current.start(), dommyS, alpha);
	auto dummyTPath = dommy_Access(current, dommyT, current.terminal(), alpha);
	dummyTPath.pop_back();
	reverse(dummySPath.begin(), dummySPath.end());
	dummySPath.pop_back();
	reverse(dummySPath.begin(), dummySPath.end());
	current.path.insert(current.path.end(), dummySPath.begin(), dummySPath.end());
	current.path.insert(current.path.begin(), dummyTPath.begin(), dummyTPath.end());
	// for(auto pt = current.path.begin(); pt != current.path.end(); ++pt){
	// 	(*pt).position();
	// 	cout << endl;
	// }
	// cout << "new path: \n";
	// debug_trace(current.path);
}

vector<Net::Bin> Route::dommy_Access(Net& net, Net::Bin& start, Net::Bin& end, double alpha){
	queue<Net::Bin> unvisited;
	map_initial();
	pre_label(start, end);
	vector<int> bound = boundingStatus(start, end, 0);
	unvisited.push(start);
	bool arrive = false;
	while(!unvisited.empty() && !arrive){
		Net::Bin current_bin = unvisited.front();
		unvisited.pop();
		vector<Net::Bin> candidate = search(current_bin, bound, start);
		for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
			if((*bin).arrive(end)){
				arrive = true;
			}
		}
		if(!arrive){
			filling(candidate, current_bin, start);
			for(auto bin = candidate.begin(); bin != candidate.end(); ++bin){
				unvisited.push((*bin));
			}	
		}
		arrive = false;
	}
	auto path = retrace(net, end, bound, alpha);
	return path;
}

vector<Net::Bin> Route::search(Net::Bin& source, vector<int>& boundary, Net::Bin& origin){
	Net::Bin up(source.x(), source.y() + 1);
	Net::Bin down(source.x(), source.y() - 1);
	Net::Bin left(source.x() - 1, source.y());
	Net::Bin right(source.x() + 1, source.y());
	vector<Net::Bin> tmp = {up, down, left, right};
	vector<Net::Bin> result;
	for(auto bin = tmp.begin(); bin != tmp.end(); ++bin){
		if(!(*bin).out_of_bound(boundary) && map_value((*bin)) == 0 && !((*bin) == origin)){
			result.push_back((*bin));
		}
	}
	return result;
}

vector<Net::Bin> Route::retrace(Net& net, Net::Bin& terminal, vector<int>& bound, double alpha){
	vector<Net::Bin> path;
	queue<Net::Bin> unvisited;
	// int ineratia = 0; // 1 -> horizontal; 0 -> vertical
	int ineratia = rand()%2;
	net.ineratia_ = ineratia;
	unvisited.push(terminal);
	while(!unvisited.empty()){
		Net::Bin current = unvisited.front();
		// if(net.id() == 9263){
		// 	current.position();
		// 	cout << endl;
		// }
		unvisited.pop();
		Net::Bin up(current.x(), current.y() + 1);
		Net::Bin down(current.x(), current.y() - 1);
		Net::Bin left(current.x() - 1, current.y());
		Net::Bin right(current.x() + 1, current.y());
		int value_up = !up.out_of_bound(bound) ? map_value(up) : 0;
		int value_down = !down.out_of_bound(bound) ? map_value(down) : 0;
		int value_left = !left.out_of_bound(bound) ? map_value(left) : 0;
		int value_right = !right.out_of_bound(bound) ? map_value(right) : 0;
		vector<Net::Bin> bins = {up, down, left, right};
		vector<int> value = {value_up, value_down, value_left, value_right}; // 0 -> up; 1 -> down; 2 -> left; 3 -> right
		int direction;
		direction = min_direction(net, value, ineratia, current, bins, bound);
		int d_value = value.at(direction);
		if(d_value != -2){
			if(ineratia != direction / 2){
				ineratia = ineratia == 1 ? 0 : 1;
			}
			path.push_back(current);
			unvisited.push(bins.at(direction));	
		}
		else{
			path.push_back(current);
			path.push_back(bins.at(direction));
		}
	}
	return path;
}

vector<Net::Bin> Route::retrace(Net& net, Net::Bin& terminal, vector<int>& bound, double alpha, int iner){
	set<int> visited;
	vector<Net::Bin> path;
	queue<Net::Bin> unvisited;
	int ineratia = iner < 2 ? iner : rand()%2; // 1 -> horizontal; 0 -> vertical
	unvisited.push(terminal);
	Net::Bin ancestor = unvisited.front();
	while(!unvisited.empty()){
		Net::Bin current = unvisited.front();
		visited.insert(current.x() + current.y() * grid_.numHGrid());
		// if(net.id() == 5192){
		// 	// int x;
		// 	current.position();
		// 	cout << " -> ";
		// 	// cin >> x;
		// }
		unvisited.pop();
		Net::Bin up(current.x(), current.y() + 1);
		Net::Bin down(current.x(), current.y() - 1);
		Net::Bin left(current.x() - 1, current.y());
		Net::Bin right(current.x() + 1, current.y());
		double value_up = !up.out_of_bound(bound) ? static_cast<double>(map_value(up)) : 0;
		double value_down = !down.out_of_bound(bound) ? static_cast<double>(map_value(down)) : 0;
		double value_left = !left.out_of_bound(bound) ? static_cast<double>(map_value(left)) : 0;
		double value_right = !right.out_of_bound(bound) ? static_cast<double>(map_value(right)) : 0;
		vector<Net::Bin> bins = {up, down, left, right};
		vector<double> value = {value_up, value_down, value_left, value_right}; // 0 -> up; 1 -> down; 2 -> left; 3 -> right
		int direction;
		direction = detour_direction(net, value, ineratia, current, bins, bound, ancestor, visited);
		int d_value = value.at(direction);
		if(d_value != -2){
			if(ineratia != direction / 2){
				ineratia = ineratia == 1 ? 0 : 1;
			}
			path.push_back(current);
			unvisited.push(bins.at(direction));
			ancestor = current;	
		}
		else{
			path.push_back(current);
			path.push_back(bins.at(direction));
		}
	}
	return path;
}

int Route::detour_direction(Net& net, vector<double>& value, int& ineratia, Net::Bin& source, vector<Net::Bin>& bins, vector<int>& bound, Net::Bin& ancestor, set<int>& visited){
	double min = numeric_limits<double>::max();
	double utility;
	int target_index = 0;
	bool enter = false;
	int count = 0;
	// && !(bins.at(index) == ancestor)
	switch(ineratia){
		case 0: // vertical 
			for(int index = value.size() - 1; index >= 0 ; --index){
				if(value.at(index) <= min && value.at(index) != 0 && value.at(index) != -3 && !bins.at(index).out_of_bound(bound) && visited.find(bins.at(index).x() + bins.at(index).y() * grid_.numHGrid()) == visited.end()){
					if(value.at(index) > 0 || value.at(index) == -2 && !(source == net.terminal())){
						min = value.at(index);
						target_index = index;
						enter = true;	
					}
				}
			}
			break;

		case 1: // horizontal
			for(int index = 0; index < value.size(); ++index){
				if(value.at(index) <= min && value.at(index) != 0 && value.at(index) != -3 && !bins.at(index).out_of_bound(bound) && visited.find(bins.at(index).x() + bins.at(index).y() * grid_.numHGrid()) == visited.end()){
					if(value.at(index) > 0 || value.at(index) == -2 && !(source == net.terminal())){
						min = value.at(index);
						target_index = index;
						enter = true;	
					}
				}
			}
			break;
	}
	if(!enter){
		for(int index = value.size() - 1; index >= 0 ; --index){
			if(value.at(index) != 0 && value.at(index) != -3 && !bins.at(index).out_of_bound(bound)){
				target_index = index;
			}
		}
		// cout << "Hi" << endl;
		// int x;
		// cin >> x;
	}	
	if(target_index < 2)
		grid_.v_move(bins.at(target_index), source, net.id());
	else
		grid_.h_move(bins.at(target_index), source, net.id());
	// if(!(source == net.start()) && !(source == net.terminal()))
	// 	map_.at(source.y()).at(source.x()) = numeric_limits<int>::max();	
	return target_index;
}

bool Route::addCost(Net::Bin& source, Net::Bin& target){
	int direction = source - target;
	// cout << direction << endl;
	int bottom = source.y() < target.y() ? source.y() : target.y();
	int left = source.x() < target.x() ? source.x() : target.x();
	// source.position();
	// cout << endl;
	// target.position();
	// cout << endl;
	// cout << "bottom: " << bottom << endl;
	// cout << "left: " << left << endl;
	int cost = 0;
	switch(direction){
		case 0: //vertical
			if(grid_.v_channel.at(source.x()).at(bottom) > grid_.v_capacity_)
				cost += numeric_limits<int>::max();
			break;

		case 1:
			if(grid_.h_channel.at(source.y()).at(left) > grid_.h_capacity_)
				cost += numeric_limits<int>::max();
			break;
	}
	return cost > 0;
}

int Route::min_direction(Net& net, vector<int>& value, int& ineratia, Net::Bin& source, vector<Net::Bin>& bins, vector<int>& bound){
	int min = map_value(source) == -3 ? numeric_limits<int>::max() : map_value(source);
	int target_index = 0;
	int count = 0;
	double utility;
	switch(ineratia){
		case 0: // vertical 
			for(int index = value.size() - 1; index >= 0 ; --index){
				if(value.at(index) <= min && value.at(index) != 0 && value.at(index) != -3 && !bins.at(index).out_of_bound(bound)){
					min = value.at(index);
					target_index = index;
				}
			}
			break;

		case 1: // horizontal
			for(int index = 0; index < value.size(); ++index){
				if(value.at(index) <= min && value.at(index) != 0 && value.at(index) != -3 && !bins.at(index).out_of_bound(bound)){
					min = value.at(index);
					target_index = index;
				}
			}
			break;
	}	
	if(target_index < 2)
		grid_.v_move(bins.at(target_index), source, net.id());
	else
		grid_.h_move(bins.at(target_index), source, net.id());	
	return target_index;
}

int Route::utility_direction(Net& net, vector<int>& value, Net::Bin& source, vector<Net::Bin>& bins, vector<int>& bound, Net::Bin& ancestor){
	int target_index = 0;
	int count = 0;
	bool select = false;
	double min_utility = numeric_limits<double>::max();
	int min = numeric_limits<int>::max();
	double utility;
	while(count < 2){
		for(int index = value.size() - 1; index >= 0 ; --index){
			if(index < 2)
				utility = grid_.v_grid_utility(bins.at(index), source);
			else
				utility = grid_.h_grid_utility(bins.at(index), source);
				// bins.at(index).position();
				// cout << "\nutility: " << utility << endl;
			if(value.at(index) <= min && value.at(index) != 0 && value.at(index) != -3 && utility != -1.0 && !bins.at(index).out_of_bound(bound)){
				min = value.at(index);
				target_index = index;
				select = true;
			}
			if(select == false){
				if(value.at(index) != 0 && value.at(index) != -3 && utility != -1.0 && !(bins.at(index).out_of_bound(bound)) && utility < min_utility && !(bins.at(index) == ancestor) ){
					min_utility = utility;
					target_index = index;
				}
			}
		}
		// if(select == false){
		// 	cout << "ancestor: ";
		// 	ancestor.position();
		// 	cout << endl;
		// 	cout << "source: ";
		// 	source.position();
		// 	cout << endl;
		// 	cout << "next: ";
		// 	bins.at(target_index).position();
		// 	cout << " " << value.at(target_index) << endl;
		// 	cout << bins.at(target_index).out_of_bound(bound) << endl;
		// 	cout << "up: ";
		// 	bins.at(0).position();
		// 	cout << " " << value.at(0) << endl;
		// 	cout << bins.at(0).out_of_bound(bound) << endl;
		// 	cout << "down: ";
		// 	bins.at(1).position();
		// 	cout << " " << value.at(1) << endl;
		// 	cout << bins.at(1).out_of_bound(bound) << endl;
		// 	cout << "left: ";
		// 	bins.at(2).position();
		// 	cout << " " << value.at(2) << endl;
		// 	cout << bins.at(2).out_of_bound(bound) << endl;
		// 	cout << "right: ";
		// 	bins.at(3).position();
		// 	cout << " " << value.at(3) << endl;
		// 	cout << bins.at(3).out_of_bound(bound) << endl;
		// 	cout << endl;
		// }
		if(target_index < 2){
			if(grid_.available_vertical_move(bins.at(target_index), source))
				break;
		}
		else{
			if(grid_.available_horizontal_move(bins.at(target_index), source))
				break;
		}
		count++;
	}
	if(count == 2){
		net.congestion();
	}
	if(target_index < 2)
		grid_.v_move(bins.at(target_index), source, net.id());
	else
		grid_.h_move(bins.at(target_index), source, net.id());	
	
	return target_index;
}

void Route::debug_trace(vector<Net::Bin>& path){
	auto n_bin = path.begin();
	cout << "------------------------------------\n";
	for(auto bin = path.begin(); bin != path.end(); ++bin){
		++n_bin;
		(*bin).position();
		cout << endl;
		if(n_bin != path.end()){
			switch((*bin) - (*n_bin)){
				case 0:
					cout << "v_channel: ";
					grid_.v_grid_status((*bin), (*n_bin));
					break;
				case 1:
					cout << "h_channel: ";
					grid_.h_grid_status((*bin), (*n_bin));
					break;
			}	
		}
		
	}
	cout << "------------------------------------\n";
}

int Route::initial_route(){
	int index = 0;
	int total_wirelength = 0;
	int overflow;
	for(auto net = net_.begin(); net != net_.end(); net++){
		lee_algorithm(*net, 0);
		// first_route(*net, 0);
	}
	overflow = grid_.numOverflow();
	cout << "overflow: " << overflow << endl;
	cout << "first route complete!" << endl;
	return overflow;
}



int Route::re_route(double alpha, int shift_unit){
	int overflow;
	queue<int> bin_queue;
	vector<vector<int> > bottleneck = grid_.bottleneck_edge();
	// vector<int> rip_net = grid_.chosen_rip_net(bottleneck, net_, index_);
	vector<vector<int> > new_rip_net = grid_.new_chosen_rip_net(bottleneck);
	// overflow_status(rip_net);
	auto current_edge = new_rip_net.begin();
	while(current_edge != new_rip_net.end()){
		vector<Net> rip_bin;
		rip_bin.clear();
		for(auto id = (*current_edge).begin(); id != (*current_edge).end(); ++id){
			rip_bin.push_back(net_.at(index_.at(*id)));
		}
		sort(rip_bin.begin(), rip_bin.end(), &Route::cmp);
		for(auto id = rip_bin.begin(); id != rip_bin.end(); ++id){
			bin_queue.push((*id).id());
		}
		++current_edge;	
	}
	// cout << "#net with overflow:" << bin_queue.size() << endl;
	// cout << "overflow: " << grid_.numOverflow() << endl;
	// for(auto id = rip_bin.begin(); id != rip_bin.end(); ++id){
	// 	cout << (*id).HPWL() << endl;;
	// }
	// auto id = rip_bin.begin();
	// // debug_trace(net_.at(index_.at(*id)).path);
	// rip(net_.at(index_.at((*id).id())));
	// second_route(net_.at(index_.at((*id).id())), 0);
	// // overflow = grid_.numOverflow();
	// ++id;
	// while(id != rip_bin.end()){
	// 	// if(*id == 21649)
	// 	// 	debug_trace(net_.at(index_.at(*id)).path);
	// 	rip(net_.at(index_.at((*id).id())));
	// 	second_route(net_.at(index_.at((*id).id())), 0);
	// 	// overflow = grid_.numOverflow();
	// 	++id;
	// }
	overflow = grid_.numOverflow();
	while(!bin_queue.empty() && overflow > 0){
		int net_id = bin_queue.front();
		bin_queue.pop();
		if(overflow_path(net_.at(index_.at(net_id)))){
			rip(net_.at(index_.at(net_id)));
			second_route(net_.at(index_.at(net_id)), 0);
		}
		overflow = grid_.numOverflow();
	}
	// overflow = grid_.numOverflow();
	// cout << "overflow: " << overflow << "\r";
	return overflow;
}


void Route::rip(Net& net){
	vector<Net::Bin>& bin_list = net.path;
	auto next_bin = bin_list.begin();
	// cout << bin_list.size() << endl;
	for(auto bin = bin_list.begin(); bin != bin_list.end(); ++bin){
		++next_bin;
		if(next_bin != bin_list.end()){
			grid_.rip((*bin), (*next_bin), net.id());	
		}
	}
}

vector<Net> Route::output(){
	sort(net_.begin(), net_.end(), &Net::cmp);
	return net_;
}

void Route::overflow_status(vector<int>& a){
	cout << "rip off " << a.size() << " net.\n";
}

vector<Net::Bin> Route::dummyCreate(Net& current, int ineratia, int shift_unit){
	bool t_above_s = current.terminal().y() > current.start().y();
	bool t_leftside_of_s = current.terminal().x() < current.start().x();
	// cout << "t_above_s: " << t_above_s << endl;
	// cout << "t_leftside_of_s: " << t_leftside_of_s << endl;
	// cout << "ineratia: " << ineratia << endl;
	Net::Bin dommyT = Net::Bin(-1, -1);
	Net::Bin dommyS = Net::Bin(-1, -1);
	vector<int> bound = boundingStatus(current.start(), current.terminal(), 0, shift_unit);
	switch(ineratia){
		case 0:
			if(t_above_s && t_leftside_of_s){
				dommyT = Net::Bin(current.terminal().x() - shift_unit, current.terminal().y());
				if(dommyT.out_of_bound(bound))
					dommyT = Net::Bin(current.terminal().x() + shift_unit, current.terminal().y());
				dommyS = Net::Bin(current.start().x(), current.start().y()  - shift_unit);
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x(), current.start().y() + shift_unit);
			}
			else if(!t_above_s && t_leftside_of_s){
				dommyT = Net::Bin(current.terminal().x() - shift_unit, current.terminal().y());
				if(dommyT.out_of_bound(bound))
					dommyT =Net::Bin(current.terminal().x() + shift_unit, current.terminal().y());
				dommyS = Net::Bin(current.start().x(), current.start().y() + shift_unit);
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x(), current.start().y() - shift_unit);
			}
			else if(t_above_s && !t_leftside_of_s){
				dommyT = Net::Bin(current.terminal().x() + shift_unit, current.terminal().y());
				if(dommyT.out_of_bound(bound))
					dommyT = Net::Bin(current.terminal().x() - shift_unit, current.terminal().y());
				dommyS = Net::Bin(current.start().x(), current.start().y()  - shift_unit);
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x(), current.start().y() + shift_unit);
			}
			else{
				dommyT = Net::Bin(current.terminal().x() + shift_unit, current.terminal().y());
				if(dommyT.out_of_bound(bound))
					dommyT = Net::Bin(current.terminal().x() - shift_unit, current.terminal().y());
				dommyS = Net::Bin(current.start().x(), current.start().y()  + shift_unit);
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x(), current.start().y() - shift_unit);
			}
			break;
		case 1:
			if(t_above_s && t_leftside_of_s){
				dommyT = Net::Bin(current.terminal().x(), current.terminal().y() + shift_unit);
				if(dommyT.out_of_bound(bound))
					dommyT = Net::Bin(current.terminal().x(), current.terminal().y() - shift_unit);
				dommyS = Net::Bin(current.start().x() + shift_unit, current.start().y());
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x() - shift_unit, current.start().y());
			}
			else if(!t_above_s && t_leftside_of_s){
				dommyT = Net::Bin(current.terminal().x(), current.terminal().y() - shift_unit);
				if(dommyT.out_of_bound(bound))
					dommyT =Net::Bin(current.terminal().x(), current.terminal().y() + shift_unit);
				dommyS = Net::Bin(current.start().x() + shift_unit, current.start().y());
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x() - shift_unit, current.start().y());
			}
			else if(t_above_s && !t_leftside_of_s){
				dommyT = Net::Bin(current.terminal().x(), current.terminal().y() + shift_unit);
				if(dommyT.out_of_bound(bound))
					dommyT = Net::Bin(current.terminal().x(), current.terminal().y() - shift_unit);
				dommyS = Net::Bin(current.start().x() + shift_unit, current.start().y());
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x() - shift_unit, current.start().y());
			}
			else{
				dommyT = Net::Bin(current.terminal().x(), current.terminal().y() - shift_unit);
				if(dommyT.out_of_bound(bound)){
					dommyT = Net::Bin(current.terminal().x(), current.terminal().y() + shift_unit);
				}
				dommyS = Net::Bin(current.start().x() - shift_unit, current.start().y());
				if(dommyS.out_of_bound(bound) || dommyS == dommyT)
					dommyS = Net::Bin(current.start().x()+ shift_unit, current.start().y());
			}
			break;
	}
	vector<Net::Bin> tmp = {dommyS, dommyT};
	return tmp;

}

bool Route::overflow_path(Net& current){
	auto bin = current.path.begin();
	auto next_bin = current.path.begin();
	++next_bin;
	while(next_bin != current.path.end()){
		if((*bin) - (*next_bin) == 0){
			if(grid_.v_grid_utility((*bin), (*next_bin)) > 1)
				return true;
		}
		else{
			if(grid_.h_grid_utility((*bin), (*next_bin)) > 1)
				return true;
		}
		++bin;
		++next_bin;
	}
	return false;
}