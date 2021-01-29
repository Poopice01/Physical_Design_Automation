#include "net.h"
using namespace std;

Net::Net(int id, int pin, int s_x, int s_y, int e_x, int e_y)
 :id_(id), pin_(pin), start_(Bin(s_x, s_y)), end_(Bin(e_x, e_y))
{
	overflow_ = false;
	routed_ = false;
	ineratia_ = false;
	int width = abs(start_.x() - end_.x());
	int height = abs(start_.y() - end_.y());
	hpwl_ =  width + height; 
}

Net::Bin::Bin(int x, int y){
	x_ = x;
	y_ = y;
	cost_ = 0;
}

void Net::status(){
	start_.position();
	cout << "\t";
	end_.position();
	cout << endl;
}

void Net::Bin::position(void){
	cout << x_ << ", " << y_;
}

int Net::HPWL(void){
	int width = abs(start_.x() - end_.x());
	int height = abs(start_.y() - end_.y());
	return width + height; 
}

bool Net::Bin::operator==(const Bin& bin){
	if(x_ == bin.x_ && y_ == bin.y_)
		return true;
	return false;
}

int Net::Bin::operator-(const Bin& bin){
	if(x_ == bin.x_ && y_ != bin.y_)
		return 0; // vertical
	else if(x_ != bin.x_ && y_ == bin.y_)
		return 1; // horizontal
	else
		return -1;
}

int Net::Bin::operator>>(const Bin& bin){
	int x = bin.x_;
	int y = bin.y_;
	int width = abs(x - x_);
	int height = abs(y - y_);
	return max(width, height);
}

bool Net::Bin::operator<(const Bin& bin0){
	return cost_ > bin0.cost_;
}

