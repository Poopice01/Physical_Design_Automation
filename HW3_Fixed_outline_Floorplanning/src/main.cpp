#include "algorithm.h"
#include <fstream>
#include <map>
using namespace std;

int numBlock, numTerminal, numNet, numPin;
int Floorplanner::total_block_area = 0;
int Floorplanner::numBlock = 0;
int B_star_tree::numBlock = 0;
clock_t B_star_tree::copy_time = 0;
int bound;
float dead_space_ratio;
unordered_map<int, Block*> blocks;
unordered_map<int, Terminal*> terminals;
vector<Net*> nets;

void block_parse(FILE* input){
	fscanf(input, "NumHardRectilinearBlocks : %d\n", &numBlock);
	fscanf(input, "NumTerminals : %d\n\n", &numTerminal);
	int count = numBlock;
	// cout << "NumHardRectilinearBlocks : " << numBlock << "\n";
	while(count--){
		int a, width, height, id;
		double x, y;
		fscanf(input, " sb%d hardrectilinear 4 (%lf, %lf) (%d, %d) (%d, %d) (%d, %d)\n", &id, &x, &y, &a, &a, &width, &height, &a, &a);
		Block* tmp = new Block(id, width, height);
		Floorplanner::add_block();
		B_star_tree::add_block();
		Floorplanner::add_area(tmp->get_area());
		blocks.insert(pair<int, Block*>(id, tmp));
	}
	bound = (int)sqrt(Floorplanner::get_total_area() * (1 + dead_space_ratio));
	fclose(input);
}

void terminal_parse(FILE* input){
	int id;
	double x, y;
	int cnt = numTerminal;
	while(cnt--){
		Terminal* tmp;
		fscanf(input, " p%d %lf %lf\n", &id, &x, &y);
		tmp = new Terminal(id, x, y);
		terminals.insert(pair<int, Terminal*>(id, tmp));
	}
	fclose(input);
}

void net_parse(FILE* input){
	fscanf(input, " NumNets : %d\n", &numNet);
	fscanf(input, " NumPins : %d\n", &numPin);
	int cnt = numNet;
	Block* Btmp;
	Terminal* Ttmp;
	while(cnt--){
		Net* tmp;
		int degree;
		string str;
		char c[100];
		fscanf(input, " NetDegree : %d\n", &degree);
		tmp = new Net(degree);
		for(int i=0; i< tmp->degree(); i++){
			fscanf(input, " %s\n", c);
			str = c;
			if(str[0] == 'p'){
				str.erase(0,1); // erase "p"
				Ttmp = terminals.find(stoi(str))->second;
				tmp->push_terminal(Ttmp);
			}else{
				str.erase(0,2); // erase "sb"
				Btmp = blocks.find(stoi(str))->second;
				tmp->push_block(Btmp);
			}
		}
		nets.push_back(tmp);
	}
	fclose(input);
}

void is_open(FILE *input, char* filename){
	if( NULL == input ){
        cerr << "can't open " << filename << " !" << "\n"; 
        exit(1);
    }
}


int main(int argc, char** argv){
	if(argc != 6){
		cerr << "Usage: ./exe [blocks] [nets] [pin] [opt] [dead_space_ratio]" << "\n";
		return 1;
	}
	vector<vector<int> > *temp;
	clock_t IO_start, IO_end, Start, Algo_start, Algo_end;
	IO_start = clock();
	dead_space_ratio = atof(argv[5]);
	FILE* input_block = fopen(argv[1], "r");
	FILE* input_net = fopen(argv[2], "r");
	FILE* input_pl = fopen(argv[3], "r");
	// cout << "File I/O finish.\n";
	// cout << "---------------------------\n";
	block_parse(input_block);
	terminal_parse(input_pl);
	net_parse(input_net);
	cout << "total block area:\t" << Floorplanner::get_total_area() << "\t";
	cout << "boundary:\t" << bound << "\n";
	Algo_start = clock();
	Floorplanner fpr(&blocks, &terminals, &nets, bound);
	Algo_end = clock();
	ofstream output(argv[4]);
	output << "Wirelength "<< fpr.wirelength() << endl;
	// output << "Boundary " << bound << endl;
	output << "Blocks\n";
	temp = fpr.output_database();
	// for(int index = 0; index < Floorplanner::get_block_num() ; index++ ){
	// 	output<< "sb" << blocks[index]->bid() << " " << blocks[index]->px() << " " << blocks[index]->py() << " " <<blocks[index]->rot() << "\n";
	// }
	for(int index = 0; index < temp->size() ; index++ ){
		output<< "sb" << index << " " << temp->at(index).at(6) << " " << temp->at(index).at(7)  << " " << temp->at(index).at(8) << "\n";
	}  
	output.close();
	IO_end = clock();
	// cout << "Total runtime: " << (double)(IO_end - IO_start)/CLOCKS_PER_SEC << " sec.\n";
	// cout << "Total time of I/O: " << (double)(IO_end - IO_start - (Algo_end - Algo_start))/CLOCKS_PER_SEC << " sec.\n";
	// cout << "Total time of Algorithm: " << (double)(Algo_end - Algo_start)/CLOCKS_PER_SEC << " sec.\n";
	// cout << "Total time of initialization: " << fpr.ini_time() << " sec.\n";
	// cout << "Total time of Perturbation: " << fpr.perturb_time() << " sec.\n";
	// cout << "Total time of packing: " << fpr.packing_time() << " sec.\n";
	// cout << "Total time of calculating wirelength: " << fpr.w_time() << " sec.\n";
	// cout << "Min dead_space_ratio: " << fpr.dsr() << endl;
	// cout << "-------------------------------------------------------------------\n";
	// cout << "Total time of copying tree: " << B_star_tree::c_time() << " sec.\n";
	// cout << "seed: " << fpr.return_seed() << " wirelength: " << fpr.wirelength() << " ";
	// cout << " wirelength: " << fpr.wirelength() << " ";
	
  	return 0;
}