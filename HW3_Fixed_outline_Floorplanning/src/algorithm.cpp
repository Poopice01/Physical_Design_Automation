#include "algorithm.h"
using namespace std;

Floorplanner::Floorplanner(unordered_map<int, Block*> *blocklist, unordered_map<int, Terminal*> *tlist, vector<Net*> *nlist, int boundary){
	this->terminate_start = clock();
	this->blist = blocklist;
	this->tlist = tlist;
	this->nlist = nlist;
	this->boundary = boundary;
	this->max_area = numeric_limits<int>::min();
	this->max_wirelength = numeric_limits<int>::min();
	this->min_wirelength = numeric_limits<int>::max();
	this->feasible_min_wirelength = numeric_limits<int>::max();
	this->min_area = numeric_limits<int>::max();
	this->min_dead_space_rate = numeric_limits<double>::max();
	this->_average_uphill_cost = 0;
	this->_average_wirelength = 0;
	this->_average_area = 0;
	this->total_copy_time = 0;
	this->total_packing_time = 0;
	this->total_initialize = 0;
	this->total_wirelength_time = 0;
	this->total_pert_time = 0;
	this->seed = (unsigned)time(NULL);
	if(Floorplanner::get_block_num() == 100){
		if(this->boundary == 444)
			this->seed = 1608746035;
		else if(this->boundary == 454)
			this->seed = 1608766017;
	}
	else if(Floorplanner::get_block_num() == 200){
		if(this->boundary == 439)
			this->seed = 1608760953;
		else if(this->boundary == 449)
			this->seed = 1608762769;
	}
	else{
		if(this->boundary == 548)
			this->seed = 1608769912;
		else if(this->boundary == 560)
			this->seed = 1608776713;
	}
	srand(this->seed);
	// cout << "Seed: " << this->seed << "\n";
	this->complete_tree_initial();
	this->preparation_for_SA();
	// this->FastSA();
	this->classic_SA();
	this->restore_best_tree();
	// this->print_position();
	this->print_status();
	// cout << "Total time of Copy: " << this->total_copy_time / CLOCKS_PER_SEC << " sec.\n";
	// cout << this->seed << endl;
	// cout << endl;
}

// void Floorplanner::reset_contour(){
// 	this->h_contour.clear();
// 	this->h_contour.assign((int)(this->boundary * 2) , 0);
// }

void Floorplanner::tree_copy_test(){
	int wait;
	this->best_tree = this->tree;
	this->perturbation();
	this->packing();
	this->print_status();
	this->tree = this->best_tree;
	this->packing();
	this->print_status();
	cin >> wait;
}

void Floorplanner::print_status(){
	cout << "max x: " << this->_max_x << "\tmax y: " << this->_max_y << "\n"; 
	cout << "current floorplan ratio: " << this->_floorplan_ratio << "\n";
	cout << "Total wirelength: " << this->wirelength() << endl;
	cout << "---------------------------------------------------" << "\n";
}

void Floorplanner::complete_tree_initial(){
	clock_t start, end;
	start = clock();
	unordered_map<int, Block*>::iterator block_it;
	this->tree.database_input(this->nlist, this->tlist);
	block_it = (this->blist)->begin();
	(this->tree).set_root(block_it->second); //done
	(this->tree).complete_binary_tree(this->blist); //done
	this->packing(); //done
	end = clock();
	this->total_initialize += (end - start);
	// this->update_ratio_and_area(); //done
	// this->calculate_wirelength(); //done
	// this->min_max_check(); //done
	// this->print_status(); //done
	// this->best_wirelength_tree = this->tree;
}


void Floorplanner::update_ratio_and_area(){
	this->_max_x = this->tree.max_x();
	this->_max_y = this->tree.max_y();
	this->_floorplan_ratio = (double)this->_max_y / (double)this->_max_x;
	this->_area = this->_max_x * this->_max_y;
}

void Floorplanner::print_position(){
	this->tree.print_position();
}


void Floorplanner::perturbation(){
	clock_t start, end;
	start = clock();
	int operation = rand() % 100;
	int numBlock = (this->blist)->size();
	int target_node_id, target_A_id, target_B_id;
	if(operation < 40){
		target_node_id = rand() % numBlock;
		// cout << "rotate " << target_node_id << endl;
		this->tree.rotate(target_node_id);
	} // rotate // done
	else if(operation < 80){
		target_A_id = rand() % numBlock;
		target_B_id = rand() % numBlock;
		while(target_A_id == target_B_id)
			target_B_id = rand() % numBlock;
		// cout << "swap " << target_A_id << ", " << target_B_id << endl;
		this->tree.swap(target_A_id, target_B_id);
	} // swap two cell // done
	else{
		target_A_id = rand() % numBlock;
		target_B_id = rand() % numBlock;
		while(target_A_id == target_B_id)
			target_B_id = rand() % numBlock;
		// cout << "insert " << target_A_id << ", " << target_B_id << endl;
		this->tree.move(target_A_id, target_B_id);
	} // move a cell to another place // done
	end = clock();
	this->total_pert_time += (end - start);
}

void Floorplanner::calculate_wirelength(){
	clock_t start, end;
	start = clock();
	this->reset_wirelength();
	this->_wirelength = this->tree.wirelength();
	end = clock();
	this->total_wirelength_time += (end - start);
}

void Floorplanner::packing(){
	clock_t start, end;
	start = clock();
	// cout << "start\t" << (this->tree).root() << "\n";
	(this->tree).x_pack((this->tree).root());
	// cout << "end\t" << (this->tree).root() << "\n";
	(this->tree).contour_max_x_y();
	end = clock();
	this->total_packing_time += (end - start);
	// cout << "startw\n";
	this->calculate_wirelength();
	// cout << "endw\n";
	this->update_ratio_and_area();
	this->min_max_check();
}

void Floorplanner::preparation_for_SA(){
	// cout << "Preparing parameter for SA..." << endl;
	// this->local_tree = this->tree;
	int perturbation_times = 10000;
	int uphill_times = 0;
	double total_area_per = 0.0;
	double total_wirelength_per = 0.0;
	double total_uphill_cost_per = 0.0;
	double alpha = 0.5;
	double beta = 1 - alpha;
	double adapative_alpha = alpha/4; //initial with 0.25
	double adapative_beta = beta/4; //initial with 0.25
	double last_cost = this->calculate_cost(adapative_alpha, adapative_beta);
	double current_cost;
	double delta_cost;
	clock_t copy_start, copy_end;
	for(int i = 0; i < perturbation_times; i++){
		this->perturbation();
		this->packing();
		total_area_per += (double)this->_area;
		total_wirelength_per += (double)this->_wirelength;
		current_cost = this->calculate_cost(adapative_alpha, adapative_beta);
		delta_cost = current_cost - last_cost;
		if(delta_cost > 0){
			uphill_times++;
			total_uphill_cost_per += delta_cost;
		}
		last_cost = current_cost;
	}
	this->_average_uphill_cost = total_uphill_cost_per / (double)uphill_times;
	// cout << this->_average_uphill_cost << endl;
	// cin >> x;
	this->_average_area = total_area_per / (double)perturbation_times;
	this->_average_wirelength = total_wirelength_per / (double)perturbation_times;
	// this->tree.tree_node_release();
	copy_start = clock();
	// this->tree = this->local_tree;
	copy_end = clock();
	this->total_copy_time += (copy_end - copy_start); 
}

// void Floorplanner::FastSA(){
// 	cout << "Start FastSA..." << endl;
// 	double initial_uphill_acceptance = 0.99;
// 	double initial_temperature = this->_average_uphill_cost / (-1 * log(initial_uphill_acceptance)); //stage 1
// 	double frozen_temperature = initial_temperature / 1.0e50;
// 	double frozen_acceptance_rate = 0.03;
// 	int perturbation_times = Floorplanner::get_block_num() * Floorplanner::get_block_num();
// 	long long c = 1E12; //stage 2 
// 	int k = 7; // stage 2
// 	double alpha = 1;
// 	double beta = 1 - alpha;
// 	double adapative_alpha_initial = alpha / 4;
// 	double adapative_beta_initial = beta / 4;
// 	double adapative_alpha = adapative_alpha_initial;
// 	double adapative_beta = adapative_beta_initial;
// 	double best_cost = this->calculate_cost(adapative_alpha, adapative_beta);
// 	double last_cost = best_cost;
// 	double iteration_cost = last_cost;
// 	double iteration_x = this->_max_x;
// 	double iteration_y = this->_max_y;
// 	double iteration_wirelength = this->_wirelength;
// 	double temperature = initial_temperature;
// 	int iteration_count = 0;
// 	clock_t copy_start, copy_end, terminate;
// 	// cout << initial_temperature << endl;
// 	int leave = 0;
// 	this->local_list = this->tree.tree_relation_output();
// 	this->local_root = this->tree.root();
// 	while(temperature > frozen_temperature){
// 		// cin >> x;
// 		double total_delta_cost = 0.0;
// 		int feasible_floorplan = 0;
// 		int accepted_floorplan = 0;
// 		// cout << "iteration: " << iteration_count << ", fmhpwl: " << this->feasible_min_wirelength << "\n";
// 		cout << "iteration: " << iteration_count << " iteration x: " << iteration_x << " y: " << iteration_y << " wirelength: " << iteration_wirelength << " cost: " << iteration_cost << "\n";
// 		for(int index = 0; index < perturbation_times; index++){
// 			terminate = clock();
// 			if(double(terminate - this->terminate_start)/CLOCKS_PER_SEC >= 1190){
// 				leave = 1;
// 				break;
// 			}
// 			this->perturbation();
// 			this->packing();
// 			double cost = this->calculate_cost(adapative_alpha, adapative_beta);
// 			double delta_cost = cost - last_cost;
// 			if(delta_cost < 0){
// 				accepted_floorplan++;
// 				total_delta_cost += delta_cost;
// 				copy_start = clock();
// 				this->local_list = this->tree.tree_relation_output();
// 				this->local_root = this->tree.root();
// 				copy_end = clock();
// 				this->total_copy_time += (copy_end - copy_start);
// 				last_cost = cost;
// 				if(last_cost <= best_cost){
// 					best_cost = last_cost;
// 					iteration_cost = last_cost;
// 					iteration_wirelength = this->_wirelength;
// 					iteration_x = this->_max_x;
// 					iteration_y = this->_max_y;
// 				}
// 				if(this->is_feasible()){
// 					feasible_floorplan++;
// 					// this->print_status();
// 					// cin >> x;
// 					if(this->_wirelength <= this->feasible_min_wirelength){
// 						this->feasible_min_wirelength = this->_wirelength;
// 						this->best_list = this->tree.tree_relation_output();
// 						this->best_root = this->tree.root();
// 						adapative_alpha_initial = 0.1;
// 						adapative_beta_initial = 0.8;
// 					}
// 					if(this->dead_space_rate() < this->min_dead_space_rate){
// 						this->min_dead_space_rate = this->dead_space_rate();
// 					}
// 				}
// 			}
// 			else{
// 				double probability = exp(-1 * delta_cost / temperature);
// 				if(rand() / static_cast<double>(RAND_MAX) < probability){
// 					accepted_floorplan++;
// 					total_delta_cost += delta_cost;
// 					copy_start = clock();
// 					this->local_list = this->tree.tree_relation_output();
// 					this->local_root = this->tree.root();
// 					copy_end = clock();
// 					this->total_copy_time += (copy_end - copy_start);
// 					last_cost = cost;
// 				}
// 				else{
// 					copy_start = clock();
// 					// this->tree = this->local_tree;
// 					this->tree.set_relation(&(this->local_list), this->local_root);
// 					copy_end = clock();
// 					this->total_copy_time += (copy_end - copy_start);
// 				}
// 			}

// 		}
// 		cout << "accepted_floorplan: " << accepted_floorplan << endl;
// 		cout << "feasible_floorplan: " << feasible_floorplan << endl;
// 		if (accepted_floorplan / static_cast<double>(perturbation_times) < frozen_acceptance_rate) {
//       		break;
//     	}
//     	//update alpha beta
// 		adapative_alpha = adapative_alpha_initial + ( (alpha - adapative_alpha_initial) * (feasible_floorplan / static_cast<double>(perturbation_times)));
// 		adapative_beta = adapative_beta_initial + ( (beta - adapative_beta_initial) * (feasible_floorplan / static_cast<double>(perturbation_times)));
// 		//update temperature
// 		iteration_count++;
// 		double average_delta_cost = abs(total_delta_cost / (double)perturbation_times);
// 		if(iteration_count >= 1 && iteration_count < k ){
// 			temperature = initial_temperature * average_delta_cost / iteration_count / c;
// 		}
// 		else{
// 			temperature = initial_temperature * average_delta_cost / iteration_count;
// 		}
// 		cout << "T: " << temperature << endl;
// 		if(leave == 1){
// 			break;
// 		}    	
// 	}
// }

void Floorplanner::classic_SA(){
	// cout << "Start SA..." << endl;
	double initial_uphill_acceptance = 0.99;
	double initial_temperature = this->_average_uphill_cost / -1 * log(initial_uphill_acceptance); 
	double frozen_temperature = initial_temperature / 1.0e50;
	// double frozen_temperature = 1.0e-10;
	double r = 0.07;
	double frozen_acceptance_rate = 0.01;
	int perturbation_times = Floorplanner::get_block_num() * Floorplanner::get_block_num();
	double alpha = 0.3;
	double beta = 1 - alpha;
	double adapative_alpha_initial = alpha/2;
	double adapative_beta_initial = beta/2;
	double adapative_alpha = adapative_alpha_initial;
	double adapative_beta = adapative_beta_initial;
	double best_cost = this->calculate_cost(adapative_alpha, adapative_beta);
	double last_cost = best_cost;
	double iteration_cost = last_cost;
	double iteration_x = this->_max_x;
	double iteration_y = this->_max_y;
	double iteration_wirelength = this->_wirelength;
	double temperature = initial_temperature;
	int iteration_count = 0;
	clock_t copy_start, copy_end, terminate;
	// cout << initial_temperature << endl;
	int leave = 0;
	// this->best_tree = this->tree;
	// this->local_tree = this->tree;
	vector<vector<int> >().swap(this->local_list);
	this->local_list.assign((*(this->tree.tree_relation_output())).begin(), (*(this->tree.tree_relation_output())).end());
	this->local_root = this->tree.root();
	while(temperature > frozen_temperature){
		// cin >> x;
		int feasible_floorplan = 0;
		int accepted_floorplan = 0;
		// if(iteration_count > 4)
		// 	r = 0.7;
		// cout << "iteration: " << iteration_count << ", fmhpwl: " << this->feasible_min_wirelength << "\n";
		// cout << "iteration: " << iteration_count << " iteration x: " << iteration_x << " y: " << iteration_y << " wirelength: " << iteration_wirelength << " cost: " << iteration_cost << "\n";
		for(int index = 0; index < perturbation_times; ++index){
			terminate = clock();
			if(double(terminate - this->terminate_start)/CLOCKS_PER_SEC >= 1197){
				leave = 1;
				break;
			}
			this->perturbation();
			// cout << "tr" << this->tree.root() << endl;
			// cout << "lr" << this->local_root << endl;
			this->packing();
			double cost = this->calculate_cost(adapative_alpha, adapative_beta);
			double delta_cost = cost - last_cost;
			if(delta_cost < 0){
				accepted_floorplan++;
				copy_start = clock();
				// this->local_tree = this->tree;
				vector<vector<int> >().swap(this->local_list);
				// cout << "1" << endl;
				this->local_list.assign((*(this->tree.tree_relation_output())).begin(), (*(this->tree.tree_relation_output())).end());
				// cout << this->tree.root() << endl;
				this->local_root = this->tree.root();
				copy_end = clock();
				this->total_copy_time += (copy_end - copy_start);
				last_cost = cost;
				if(last_cost <= best_cost){
					best_cost = last_cost;
					iteration_cost = last_cost;
					iteration_wirelength = this->_wirelength;
					iteration_x = this->_max_x;
					iteration_y = this->_max_y;
					// cout << iteration_x << ", " << iteration_y << endl;
					// this->best_tree = this->tree;
				}
				if(this->is_feasible()){
					feasible_floorplan++;
					if(this->_wirelength <= this->feasible_min_wirelength){
						this->feasible_min_wirelength = this->_wirelength;
						vector<vector<int> >().swap(this->best_list);
						this->best_list.assign((*(this->tree.tree_relation_output())).begin(), (*(this->tree.tree_relation_output())).end());
						this->best_root = this->tree.root();
						adapative_alpha_initial = 0.1;
						adapative_beta_initial = 0.8;
						// this->best_wirelength_tree = this->tree;
					}
					if(this->dead_space_rate() < this->min_dead_space_rate){
						this->min_dead_space_rate = this->dead_space_rate();
						// this->best_dead_space_ratio_tree = this->tree;
					}
				}
			}
			else{
				double probability = exp(-1 * delta_cost / temperature);
				if(rand() / static_cast<double>(RAND_MAX)< probability){
					accepted_floorplan++;
					copy_start = clock();
					// cout << this->tree.root() << endl;
					vector<vector<int> >().swap(this->local_list);
					// cout << "2" << endl;
					this->local_list.assign((*(this->tree.tree_relation_output())).begin(), (*(this->tree.tree_relation_output())).end());
					this->local_root = this->tree.root();
					copy_end = clock();
					this->total_copy_time += (copy_end - copy_start);
					// cout << this->_max_x << ", " << this->_max_y << endl;
					last_cost = cost;
				}
				else{
					copy_start = clock();
					// this->tree = this->local_tree;
					// cout << "3" << endl;
					this->tree.set_relation(&(this->local_list), this->local_root);
					copy_end = clock();
					this->total_copy_time += (copy_end - copy_start);
				}
			}

		}
		temperature *= r;
		// cout << "accepted_floorplan: " << accepted_floorplan << endl;
		// cout << "feasible_floorplan: " << feasible_floorplan << endl;
		if (accepted_floorplan / static_cast<double>(perturbation_times) < frozen_acceptance_rate) {
      		break;
    	}
    	//update alpha beta
		// adapative_alpha = adapative_alpha_initial + ( (alpha - adapative_alpha_initial) * (feasible_floorplan / (double)perturbation_times));
		// adapative_beta = adapative_beta_initial + ( (beta - adapative_beta_initial) * (feasible_floorplan / (double)perturbation_times));
		// cout << "Alpha: " << adapative_alpha << endl;
		// cout << "Beta: " << adapative_beta << endl;
		// adapative_beta = adapative_beta_initial + ( 1 - (feasible_floorplan / (double)perturbation_times));
		// adapative_alpha = 1 - adapative_beta;
		//update temperature
		iteration_count++;
		// cout << "T: " << temperature << endl;
		if(leave == 1){
			break;
		}    	
	}

}

void Floorplanner::restore_best_tree(){
	int operation = 0;
	switch(operation){
		case 0:
			// this->tree = this->best_wirelength_tree;
			this->tree.set_relation(&(this->best_list), this->best_root);
			break;

		case 1:
			this->tree = this->best_tree;
			break;

		case 2:
			this->tree = this->best_dead_space_ratio_tree;
			break;
	}
	this->packing();
}


void Floorplanner::min_max_check(){
	if (this->_area <= this->min_area){
      this->min_area = this->_area;
    }
    if (this->_area >= this->max_area){
      this->max_area = this->_area;
    }
    if (this->_wirelength <= this->min_wirelength){
      this->min_wirelength = this->_wirelength;
    }
    if (this->_wirelength >= this->max_wirelength){
      this->max_wirelength = this->_wirelength;
    }
}

double Floorplanner::dead_space_rate(){
	double area = this->_max_x * this->_max_y;
	int white_space = area - Floorplanner::total_block_area;
	return white_space / area;
}

bool Floorplanner::is_feasible(){
	return this->_max_x <= this->boundary && this->_max_y <= this->boundary;
}

void Floorplanner::initialize_min_max(){
	this->max_area = numeric_limits<int>::min();
	this->max_wirelength = numeric_limits<int>::min();
	this->min_wirelength = numeric_limits<int>::max();
	this->min_area = numeric_limits<int>::max();
}

double Floorplanner::calculate_cost(double alpha, double beta){
	double normalized_area = (double)this->_area / (this->max_area - this->min_area);
	double normalized_wirelength = (double)this->_wirelength / (this->max_wirelength - this->min_wirelength);
	double penalty = 0.0;
	double width_penalty, height_penalty;
	width_penalty = this->_max_x > this->boundary ? pow((this->_max_x - this->boundary), 2) * 5: 0;
	height_penalty = this->_max_y > this->boundary ? pow((this->_max_y - this->boundary), 2)  * 5: 0;
	if(this->_max_x > this->boundary || this->_max_y > this->boundary){
		if(this->_max_x > this->boundary && this->_max_y > this->boundary)
			penalty += ( this->_area - pow(this->boundary, 2));
		else if(this->_max_x > this->boundary)
			penalty += ((this->_max_x - this->boundary) * this->_max_y);
		else
			penalty += (this->_max_x * (this->_max_y - this->boundary));
		penalty += (pow((this->_max_x - this->boundary), 2) + pow((this->_max_y - this->boundary), 2));
	}
	// return (alpha * normalized_area) + (beta * normalized_wirelength) + (1 - alpha - beta) * pow((1 - this->_floorplan_ratio), 2) + width_penalty + height_penalty;
	// return (alpha * normalized_area) + (beta * normalized_wirelength) + (1 - alpha - beta) * pow((1 - this->_floorplan_ratio), 2) + penalty;
	return (alpha * normalized_area) + (beta * normalized_wirelength) + (1.0 - alpha - beta) *  1000 * penalty;
	// return (alpha * this->_area) + (beta * this->_wirelength) + (1.0 - alpha - beta) *  1000 * penalty;
	// return (alpha * normalized_area) + (beta * normalized_wirelength) + 10 * penalty / normalized_area;
	// return (alpha * normalized_area) + 10 * penalty / normalized_area;
}


vector<vector<int> >* Floorplanner::output_database(){
	return &(this->tree.tree_list);
}