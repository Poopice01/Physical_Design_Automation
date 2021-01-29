#include "route.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <time.h>
using namespace std;

Route file_input(char*);
void output_format(char*, vector<Net>&);

int main(int argc, char** argv){
	// std::ios::sync_with_stdio(false);
	if(argc != 3){
		cout << "Usage: $ <exe> <*.modified.txt> <*.result>" << endl;
		return 0; 
	}
    clock_t start, end;
    start = clock();
	Route route = file_input(argv[1]);
	route.random_shuffle();
    int shift_unit = 1;
    int overflow = route.initial_route();
    overflow = route.re_route(0.05, shift_unit);
    while(overflow != 0){
        ++shift_unit;
        overflow = route.re_route(0.05, shift_unit);
        end = clock();
        if(static_cast<double>(end - start) / CLOCKS_PER_SEC > 60)
            break;
    }
    end = clock();
    // overflow = route.re_route(0.05, 1);
    cout << "overflow: " << overflow << endl;
    cout << "total time: " << static_cast<double>(end - start) / CLOCKS_PER_SEC << "'s" << endl;
    vector<Net> opt = route.output();
    output_format(argv[2], opt);
	return 0; 
}

Route file_input(char* filename){
	FILE *fp;
	char none[15];
	int sX, sY, eX, eY, pin, gridX, gridY, v_capacity, h_capacity, num_net, id;
	vector<Net> tmp;
	fp = fopen(filename, "r");
	if (!fp) {
        cout << "Error: No such file." << endl;
        exit(1);
    }
    fscanf(fp, "%s%d%d", &none, &gridX, &gridY);
    fscanf(fp, "%s%s%d", &none, &none, &v_capacity);
    fscanf(fp, "%s%s%d", &none, &none, &h_capacity);
    fscanf(fp, "%s%s%d", &none, &none, &num_net);
    Grid grid(h_capacity, v_capacity, gridX, gridY, num_net);
    for(int index = 0; index < num_net; ++index){
    	fscanf(fp, "%s%d%d", &none, &id, &pin);
    	fscanf(fp, "%d%d", &sX, &sY);
    	fscanf(fp, "%d%d", &eX, &eY);
    	Net net(id, pin, sX, sY, eX, eY);
    	tmp.push_back(net);
    }
    fclose(fp);
    Route route(grid, tmp);
    return route;
}

void output_format(char* filename, vector<Net>& nets){
	ofstream rpt(filename);
	for(auto net = nets.begin(); net != nets.end(); ++net){
		rpt << "net" << (*net).id() << " " << (*net).id() << endl;
		auto f_bin = (*net).path.rbegin();
		auto n_bin = (*net).path.rbegin();
		++n_bin;
		while(n_bin != (*net).path.rend()){
			rpt << "(" << (*f_bin).x() << ", " << (*f_bin).y() << ", 1)-";
			rpt << "(" << (*n_bin).x() << ", " << (*n_bin).y() << ", 1)\n";
			++n_bin;
			++f_bin; 	
		}
		rpt << "!\n";
	}
	rpt.close();
}