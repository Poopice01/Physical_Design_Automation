#include <stdlib.h>
#include <iostream>

int main()
{
	using namespace std;
	unsigned seed = 588;
	cout << "seed: " << seed << endl;
	srand(seed);
	for(int i = 0; i < 10; ++i){
		cout << rand()%10 << endl;
	}
	return 0;
}