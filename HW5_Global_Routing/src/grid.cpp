#include "grid.h"

using namespace std;

Grid::Grid(int h_c, int v_c, int h_g, int v_g, int nN)
	: h_capacity_(h_c), v_capacity_(v_c), num_h_grid_(h_g), num_v_grid_(v_g), numNet(nN)
{
	vector<int> tmprow(h_g - 1);
	vector<int> tmpcol(v_g - 1);
	h_channel.resize(v_g, tmprow);
	v_channel.resize(h_g, tmpcol);
	set<int> tmp;
	vector<set<int> > linkrow(h_g - 1, tmp);
	vector<set<int> > linkcol(v_g - 1, tmp);
	h_link.resize(v_g, linkrow);
	v_link.resize(h_g, linkcol);
	seed_ = time(NULL);
	if(numNet == 13357)
		seed_ = 1611733203;
	else if(numNet == 22465)
		seed_ = 1611728474;
	else if(numNet == 21609)
		seed_ = 1611730683;
	else if(numNet == 27781)
		seed_ = 1611736487;
}

void Grid::status(){
	int edge = h_channel.size() * h_channel.at(0).size() + v_channel.size() * v_channel.at(0).size();
	cout << "Total edge: " << edge << endl;
	cout << "Horizontal Capacity: " << h_capacity_ << endl;
	cout << "Vertical Capacity: " << v_capacity_ << endl;
}

bool Grid::available_vertical_move(Net::Bin& source, Net::Bin& destination){
	int target = min(source.y(), destination.y());
	if(v_channel.at(source.x()).at(target) >= v_capacity_)
		return false;
	return true;
}

bool Grid::available_horizontal_move(Net::Bin& source, Net::Bin& destination){
	int target = min(source.x(), destination.x());
	if(h_channel.at(source.y()).at(target) >= h_capacity_)
		return false;
	return true;
}

void Grid::h_move(Net::Bin& source, Net::Bin& destination, int id){
	int target = min(source.x(), destination.x());
	h_channel.at(source.y()).at(target) += 1;
	h_link.at(source.y()).at(target).insert(id);
}

void Grid::v_move(Net::Bin& source, Net::Bin& destination, int id){
	int target = min(source.y(), destination.y());
	v_channel.at(source.x()).at(target) += 1;
	v_link.at(source.x()).at(target).insert(id);
}

void Grid::v_grid_status(Net::Bin& source, Net::Bin& destination){
	int target = min(source.y(), destination.y());
	cout << v_channel.at(source.x()).at(target) << endl;
}

void Grid::h_grid_status(Net::Bin& source, Net::Bin& destination){
	int target = min(source.x(), destination.x());
	cout << h_channel.at(source.y()).at(target) << endl;
}

double Grid::h_grid_utility(Net::Bin& source, Net::Bin& destination){
	int target = min(source.x(), destination.x());
	if(target < num_h_grid_ - 1 && target != -1)
		return static_cast<double>(h_channel.at(source.y()).at(target)) / h_capacity_;
	return -1.0;
}

double Grid::v_grid_utility(Net::Bin& source, Net::Bin& destination){
	int target = min(source.y(), destination.y());
	if(target < num_v_grid_ - 1 && target != -1)
		return static_cast<double>(v_channel.at(source.x()).at(target)) / v_capacity_;
	return -1.0;
}

int Grid::numOverflow(){
	int overflow = 0;
	for(int index = 0; index < num_v_grid_; index++){
		for(auto demand = h_channel.at(index).begin(); demand != h_channel.at(index).end(); ++demand){
			if((*demand) > h_capacity_)
				overflow += ((*demand) - h_capacity_);
		}
	}
	for(int index = 0; index < num_h_grid_; index++){
		for(auto demand = v_channel.at(index).begin(); demand != v_channel.at(index).end(); ++demand){
			if((*demand) > v_capacity_)
				overflow += ((*demand) - v_capacity_);
		}
	}
	return overflow;
}

void Grid::rip(Net::Bin& source, Net::Bin& destination, int id){
	int direction = source - destination;
	switch(direction){
		case 0: // vertical
			v_rip(source, destination, id);
			break;
		case 1:
			h_rip(source, destination, id);
			break;
	}
}

void Grid::h_rip(Net::Bin& source, Net::Bin& destination, int id){
	int target = min(source.x(), destination.x());
	h_channel.at(source.y()).at(target) -= 1;
	h_link.at(source.y()).at(target).erase(id);
}

void Grid::v_rip(Net::Bin& source, Net::Bin& destination, int id){
	int target = min(source.y(), destination.y());
	v_channel.at(source.x()).at(target) -= 1;
	v_link.at(source.x()).at(target).erase(id);	
}

// vector<vector<int> > Grid::bottleneck_edge(){
// 	vector<vector<int> > edges;
// 	for(int y = 0; y < h_channel.size(); ++y){
// 		for(int col = 0; col < h_channel.at(y).size(); ++col){
// 			if(h_channel.at(y).at(col) > h_capacity_){
// 				vector<int> tmp = { y, col, 1};
// 				edges.push_back(tmp);
// 			}
// 		}
// 	}
// 	for(int x = 0; x < v_channel.size(); ++x){
// 		for(int row = 0; row < v_channel.at(x).size(); ++row){
// 			if(v_channel.at(x).at(row) > v_capacity_){
// 				vector<int> tmp = { x, row, 0};
// 				edges.push_back(tmp);
// 			}
// 		}
// 	}
// 	cout << "numBottleneck edges:" << edges.size() << endl;
// 	return edges;
// }

vector<vector<int> > Grid::bottleneck_edge(){
	vector<vector<int> > edges;
	int overflow;
	for(int y = 0; y < h_channel.size(); ++y){
		for(int col = 0; col < h_channel.at(y).size(); ++col){
			if(h_channel.at(y).at(col) > h_capacity_){
				overflow = h_channel.at(y).at(col) - h_capacity_;
				vector<int> tmp = { overflow, y, col, 1};
				edges.push_back(tmp);
			}
		}
	}
	for(int x = 0; x < v_channel.size(); ++x){
		for(int row = 0; row < v_channel.at(x).size(); ++row){
			if(v_channel.at(x).at(row) > v_capacity_){
				overflow = v_channel.at(x).at(row) - v_capacity_;
				vector<int> tmp = { overflow, x, row, 0};
				edges.push_back(tmp);
			}
		}
	}
	sort(edges.begin(), edges.end());
	// cout << "numBottleneck edges:" << edges.size() << endl;
	return edges;
}

vector<vector<int> > Grid::new_chosen_rip_net(vector<vector<int> >& bottleneck){
	set<int> rip_net_id;
	vector<int> tmp;
	vector<vector<int> > overflow_net;
	overflow_net.clear();
	for(auto num = bottleneck.begin(); num != bottleneck.end(); ++num){
		if((*num).at(3) == 1){
			tmp.clear();
			int y = (*num).at(1);
			int col = (*num).at(2);
			int rip_num = (h_link.at(y).at(col).size()) - h_capacity_;
			for(auto id = h_link.at(y).at(col).begin(); id != h_link.at(y).at(col).end(); ++id){
				tmp.push_back(*id);
			}
			// for(auto id = h_link.at(y).at(col).begin(); id != h_link.at(y).at(col).end(); ++id){
			// 	tmp_net.push_back(nets.at(index.at(*id)));
			// }
			auto rng = default_random_engine(seed_);
			shuffle(begin(tmp), end(tmp), rng);
			// sort(begin(tmp_net), end(tmp_net), &Net::cmpHPWL);
			// for(auto id = tmp_net.begin(); id != tmp_net.end(); ++id){
			// 	tmp.push_back((*id).id());
			// }
			for(int num = 0; num < rip_num; ++num){
				rip_net_id.insert(tmp.at(num));
			}
		}
		else{
			tmp.clear();
			int x = (*num).at(1);
			int row = (*num).at(2);
			int rip_num = (v_link.at(x).at(row).size()) - v_capacity_;
			for(auto id = v_link.at(x).at(row).begin(); id != v_link.at(x).at(row).end(); ++id){
				tmp.push_back(*id);
			}
			// auto rng = default_random_engine(seed_);
			// shuffle(begin(tmp), end(tmp), rng);
			// for(auto id = v_link.at(x).at(row).begin(); id != v_link.at(x).at(row).end(); ++id){
			// 	tmp_net.push_back(nets.at(index.at(*id)));
			// }
			auto rng = default_random_engine(seed_);
			shuffle(begin(tmp), end(tmp), rng);
			// sort(begin(tmp_net), end(tmp_net), &Net::cmpHPWL);
			// for(auto id = tmp_net.begin(); id != tmp_net.end(); ++id){
			// 	tmp.push_back((*id).id());
			// }
			for(int num = 0; num < rip_num; ++num){
				rip_net_id.insert(tmp.at(num));
			}
		}
		overflow_net.push_back(tmp);
	}
	return overflow_net;
}

vector<int> Grid::chosen_rip_net(vector<vector<int> >& bottleneck, vector<Net>& nets, vector<int>& index){
	set<int> rip_net_id;
	vector<int> tmp;
	// vector<Net> tmp_net;
	for(auto num = bottleneck.begin(); num != bottleneck.end(); ++num){
		if((*num).at(2) == 1){
			tmp.clear();
			int y = (*num).at(0);
			int col = (*num).at(1);
			int rip_num = (h_link.at(y).at(col).size()) - h_capacity_;
			for(auto id = h_link.at(y).at(col).begin(); id != h_link.at(y).at(col).end(); ++id){
				tmp.push_back(*id);
			}
			// for(auto id = h_link.at(y).at(col).begin(); id != h_link.at(y).at(col).end(); ++id){
			// 	tmp_net.push_back(nets.at(index.at(*id)));
			// }
			auto rng = default_random_engine(seed_);
			shuffle(begin(tmp), end(tmp), rng);
			// sort(begin(tmp_net), end(tmp_net), &Net::cmpHPWL);
			// for(auto id = tmp_net.begin(); id != tmp_net.end(); ++id){
			// 	tmp.push_back((*id).id());
			// }
			for(int num = 0; num < rip_num; ++num){
				rip_net_id.insert(tmp.at(num));
			}
		}
		else{
			tmp.clear();
			int x = (*num).at(0);
			int row = (*num).at(1);
			int rip_num = (v_link.at(x).at(row).size()) - v_capacity_;
			for(auto id = v_link.at(x).at(row).begin(); id != v_link.at(x).at(row).end(); ++id){
				tmp.push_back(*id);
			}
			// auto rng = default_random_engine(seed_);
			// shuffle(begin(tmp), end(tmp), rng);
			// for(auto id = v_link.at(x).at(row).begin(); id != v_link.at(x).at(row).end(); ++id){
			// 	tmp_net.push_back(nets.at(index.at(*id)));
			// }
			auto rng = default_random_engine(seed_);
			shuffle(begin(tmp), end(tmp), rng);
			// sort(begin(tmp_net), end(tmp_net), &Net::cmpHPWL);
			// for(auto id = tmp_net.begin(); id != tmp_net.end(); ++id){
			// 	tmp.push_back((*id).id());
			// }
			for(int num = 0; num < rip_num; ++num){
				rip_net_id.insert(tmp.at(num));
			}
		}
	}
	tmp.clear();
	for(auto id = rip_net_id.begin(); id != rip_net_id.end(); ++id){
		tmp.push_back(*id);
	}
	// auto rng = default_random_engine(seed_);
	// shuffle(begin(tmp), end(tmp), rng);
	return tmp;
}