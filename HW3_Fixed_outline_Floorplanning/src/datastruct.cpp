#include "datastruct.h"
using namespace std;

// Node::Node(Block* tmp){
// 	this->body = tmp;
// 	this->leftchild = NULL;
// 	this->rightchild = NULL;
// 	this->parent = NULL;
// }

Terminal::Terminal(int id, double x, double y){
	this->id = id;
	this->_x = x;
	this->_y = y;
}

Block::Block(int id, int width, int height){
	this->area = width * height;
	this->width = width;
	this->height = height;
	this->x = 0.0;
	this->y = 0.0;
	this->is_rotated = false;
	this->id = id;
}

void B_star_tree::root_pack(int id){
	this->tree_list.at(id).at(6) = 0;
	this->tree_list.at(id).at(7) = 0;
}

void B_star_tree::update_position_x(int current, int available_x){
	this->tree_list.at(current).at(6) = available_x;
}

void B_star_tree::update_position_y(int current){
	unsigned int start = this->x_of(current);
	unsigned int end = this->available_x_of(current);
	int max = 0;
	for(unsigned int index = start; index < end; index++){
		if(index < this->contour.size()){
			if(this->contour.at(index) >= max)
				max = this->contour.at(index);
		}
		else{
			this->contour.push_back(0);
		}
	}
	this->tree_list.at(current).at(7) = max;
}

vector<vector<int> >* B_star_tree::tree_relation_output(){
	return &(this->tree_list);
}

void B_star_tree::tree_space_release(){
	vector<vector<int> >().swap(this->tree_list);
}

void B_star_tree::set_relation(vector<vector<int> >* list, int root){
	{ 
    	vector<vector<int> > tmp = this->tree_list;   
    	this->tree_list.swap(tmp); 
    } 
	this->tree_list = *list;
	this->_root_id = root; 
}

int B_star_tree::wirelength(){
	int total_wirelength = 0;
	for(unsigned int index = 0; index < this->net_list_blocks.size(); index++){
		int max_x = numeric_limits<int>::min();
		int max_y = numeric_limits<int>::min();
		int min_x = numeric_limits<int>::max();
		int min_y = numeric_limits<int>::max();
		vector<int>::iterator it;
		for(it = this->net_list_blocks.at(index).begin(); it != this->net_list_blocks.at(index).end(); ++it){
			int pin_x = this->get_pin_x(*it);
			int pin_y = this->get_pin_y(*it);
			max_x = pin_x > max_x ? pin_x : max_x;
			max_y = pin_y  > max_y ? pin_y : max_y;
			min_x = pin_x  < min_x ? pin_x  : min_x;
			min_y = pin_y < min_y ? pin_y : min_y;
		}
		for(it = this->net_list_terminals.at(index).begin(); it != this->net_list_terminals.at(index).end(); ++it){
			int t_x = this->get_terminal_x((*it)-1);
			int t_y = this->get_terminal_y((*it)-1);
			max_x = t_x > max_x ? t_x  : max_x;
			max_y = t_y > max_y ? t_y : max_y;
			min_x = t_x < min_x ? t_x : min_x;
			min_y = t_y < min_y ? t_y : min_y;
		}	
		total_wirelength += ((max_x - min_x) + (max_y - min_y));
	}
	return total_wirelength;
}

void B_star_tree::database_input(vector<Net*>* nlist, unordered_map<int, Terminal*>* tlist){
	vector<Net*>::iterator n_it;
	unordered_map<int, Terminal*>::iterator t_it;
	vector<Block*>::iterator b_it;
	vector<Terminal*>::iterator pin_it;
	vector<int> temp;
	temp.clear();
	this->terminal_list.assign(tlist->size(), temp);
	for(n_it = nlist->begin(); n_it != nlist->end(); ++n_it){
		vector<int> pins;
		vector<int> blocks;
		for(b_it = (*n_it)->blocklist.begin(); b_it != (*n_it)->blocklist.end(); ++b_it){
			blocks.push_back((*b_it)->id);
			// cout << (*b_it)->id << endl;
		}
		this->net_list_blocks.push_back(blocks);
		for(pin_it = (*n_it)->terminallist.begin(); pin_it != (*n_it)->terminallist.end(); ++pin_it){
			pins.push_back((*pin_it)->id);
			// cout << (*pin_it)->id << endl;
		}
		this->net_list_terminals.push_back(pins);
	}
	for(t_it = tlist->begin(); t_it != tlist->end(); ++t_it){
		vector<int> position;
		position.push_back(t_it->second->x());
		position.push_back(t_it->second->y());
		// cout << t_it->second->_id() << endl;
		// this->terminal_list.push_back(position);
		this->terminal_list.at(t_it->second->_id() - 1) = position;
	}
}

void B_star_tree::set_root(Block* tmp){
	this->tree_list.at(tmp->id).at(6) = 0;
	this->tree_list.at(tmp->id).at(7) = 0;
	this->tree_list.at(tmp->id).at(8) = tmp->is_rotated;
	this->tree_list.at(tmp->id).at(4) = tmp->width;
	this->tree_list.at(tmp->id).at(5) = tmp->height;
	this->tree_list.at(tmp->id).at(0) = -1;
	this->_root_id = tmp->id;
}

void B_star_tree::print_relation(int id){
	if(id != -1)
		cout <<  this->parent_of(id) << ", " << this->leftchild_of(id)<< ", " << this->rightchild_of(id) << ", " << this->rotate_status(id) << endl;
	if(id == this->root())
		cout << "root" << "\n";
}

void B_star_tree::print_position(){
	for(unsigned int index = 0; index < this->tree_list.size(); index++){
		cout << "Macro" << index << "(" << this->tree_list.at(index).at(6) << ", " << this->tree_list.at(index).at(7) << ")\n";
	}
}

void B_star_tree::contour_max_x_y(void){
	int l;
	vector<int>::iterator max;
	max = max_element(begin(this->contour), end(this->contour));
	this->_max_x = this->contour.size();
	this->_max_y = *max;
}

int B_star_tree::parent_of(int id){
	if(id != -1)
 		return (this->tree_list.at(id)).at(0);
 	else
 		return -1; 
}

int B_star_tree::leftchild_of(int id){
	if(id != -1)
 		return (this->tree_list.at(id)).at(1);
 	else
 		return -1; 
}

int B_star_tree::rightchild_of(int id){
	if(id != -1)
 		return (this->tree_list.at(id)).at(2);
 	else
 		return -1; 
}

int B_star_tree::rotate_status(int id){
	if(id != -1)
 		return (this->tree_list.at(id)).at(3);
 	else
 		return -1; 
}

bool B_star_tree::is_leftchild_of(int parent, int child){
	if(parent != -1)
		return (this->tree_list.at(parent)).at(1) == child;
	else
		return true;
}

void B_star_tree::set_parent(int itself, int parent){
	if(itself != -1)
		(this->tree_list.at(itself)).at(0) = parent;
}

void B_star_tree::set_leftchild(int itself, int leftchild){
	if(itself != -1)
		(this->tree_list.at(itself)).at(1) = leftchild;
}

void B_star_tree::set_rightchild(int itself, int rightchild){
	if(itself != -1)
		(this->tree_list.at(itself)).at(2) = rightchild;
}

void B_star_tree::set_rotate(int itself, int status){
	if(itself != -1)
		(this->tree_list.at(itself)).at(3) = status;
}

void B_star_tree::set_status(Block* tmp){
	this->tree_list.at(tmp->id).at(4) = tmp->width;
	this->tree_list.at(tmp->id).at(5) = tmp->height;
	this->tree_list.at(tmp->id).at(8) = tmp->is_rotated;
}

void B_star_tree::complete_binary_tree(unordered_map<int, Block*> *blocklist){
	unordered_map<int, Block*>::iterator block_it;
	int current = this->root();
	block_it = blocklist->begin();
	++block_it;
	queue<int> queue_node;
	while(block_it != blocklist->end()){
		this->set_parent(block_it->second->id, current);
		this->set_leftchild(current, block_it->second->id);
		this->set_rotate(block_it->second->id, block_it->second->is_rotated);
		this->set_status(block_it->second);
		queue_node.push(block_it->second->id);
		++block_it;
		if(block_it == blocklist->end())
			break;
		this->set_parent(block_it->second->id, current);
		this->set_rightchild(current, block_it->second->id);
		this->set_status(block_it->second);
		queue_node.push(block_it->second->id);
		current = queue_node.front();
		queue_node.pop();
		++block_it;
	}
}

void B_star_tree::print_all_relation(){
	for(int index = 0; index < B_star_tree::numBlock; index++){
		cout << index << ": ";
		this->print_relation(index);
	}
}

bool B_star_tree::detection(){
	vector<int> show;
	show.assign(B_star_tree::get_block_num(), 0);
	for(int index = 0; index < B_star_tree::get_block_num(); index++){
		if(this->tree_list.at(index).at(0) != -1){
			// cout << this->tree_list.at(index).at(0) << endl;
			show.at(this->tree_list.at(index).at(0)) += 1;
		}
	}
	for(int index = 0; index < B_star_tree::get_block_num(); index++){
		if(show.at(index) > 2){
			cout << "P: " << index << endl;
			return false;
		}
	}
	show.assign(B_star_tree::get_block_num(), 0);
	for(int index = 0; index < B_star_tree::get_block_num(); index++){
		if(this->tree_list.at(index).at(1) != -1)
			show.at(this->tree_list.at(index).at(1)) += 1;
	}
	for(int index = 0; index < B_star_tree::get_block_num(); index++){
		if(show.at(index) > 1){
			cout << "L: " << index << endl;
			return false;
		}
	}
	show.assign(B_star_tree::get_block_num(), 0);
	for(int index = 0; index < B_star_tree::get_block_num(); index++){
		if(this->tree_list.at(index).at(2) != -1)
			show.at(this->tree_list.at(index).at(2)) += 1;
	}
	for(int index = 0; index < B_star_tree::get_block_num(); index++){
		if(show.at(index) > 1){
			cout << "R: " << index << endl;
			return false;
		}
	}

	return true;

}

void B_star_tree::swap(int A_id, int B_id){
	int tmpP, tmpL, tmpR, tmpRot;
	tmpP = this->parent_of(A_id);
	tmpL = this->leftchild_of(A_id);
	tmpR = this->rightchild_of(A_id);
	// tmpRot = this->rotate_status(A_id);
	// if(this->root() == A_id || this->root() == B_id){
	// 	this->_root_id = this->root() == A_id ? B_id : A_id;
	// }
	if(A_id != this->parent_of(B_id) && B_id != tmpP){
		// Parent's child set
		if(this->is_leftchild_of(tmpP, A_id))
			this->set_leftchild(tmpP, B_id);
		else
			this->set_rightchild(tmpP, B_id);
		if(this->is_leftchild_of(this->parent_of(B_id), B_id))
			this->set_leftchild(this->parent_of(B_id), A_id);
		else
			this->set_rightchild(this->parent_of(B_id), A_id);
		// children's parent set
		this->set_parent(tmpL, B_id);
		this->set_parent(tmpR, B_id);
		this->set_parent(this->leftchild_of(B_id), A_id);
		this->set_parent(this->rightchild_of(B_id), A_id);
		// A <-> B
		this->set_parent(A_id, this->parent_of(B_id));
		this->set_leftchild(A_id, this->leftchild_of(B_id));
		this->set_rightchild(A_id, this->rightchild_of(B_id));
		// this->set_rotate(A_id, this->rotate_status(B_id));
		this->set_parent(B_id, tmpP);
		this->set_leftchild(B_id, tmpL);
		this->set_rightchild(B_id, tmpR);
		// this->set_rotate(B_id, tmpRot);
	}
	else{
		int father, child;
		int status = 0;
		father = (A_id == this->parent_of(B_id)) ? A_id : B_id;
		child = (father == A_id) ? B_id : A_id;
		// cout << father << ", " << child << endl; //15, 84
		// cout << "parent of father: " << this->tree_list.at(father).at(0) << endl; //28
		// cout << "parent of child: " << this->tree_list.at(child).at(0) << endl; //15
		tmpP = this->parent_of(father); //28
		tmpL = this->leftchild_of(father); 
		tmpR = this->rightchild_of(father); 
		// tmpRot = this->rotate_status(father);
		// Parent's child set //done
		if(this->is_leftchild_of(tmpP, father)) // 28C = 84
			this->set_leftchild(tmpP, child);
		else
			this->set_rightchild(tmpP, child);
		// children's parnet set
		this->set_parent(this->leftchild_of(child), father); 
		this->set_parent(this->rightchild_of(child), father);
		if(this->is_leftchild_of(father, child)){
			this->set_parent(this->rightchild_of(father), child);
			status = 0;
			// this->tree.set_leftchild(child, father);
		}
		else{
			this->set_parent(this->leftchild_of(father), child);
			status = 1;
			// this->tree.set_rightchild(child, father);
		}
		this->set_parent(father, child);
		this->set_parent(child, tmpP);
		// Child <-> Son
		// this->set_rotate(father, child);
		// this->set_rotate(child, tmpRot);
		this->set_leftchild(father, this->leftchild_of(child));
		this->set_rightchild(father, this->rightchild_of(child));
		switch(status){
			case 0:
				this->set_rightchild(child, tmpR);
				this->set_leftchild(child, father);
				break;

			case 1:
				this->set_leftchild(child, tmpL);
				this->set_rightchild(child, father);
				break;
		}
		// cout << "new parent of father: " << this->tree_list.at(father).at(0) << endl;
		// cout << "new parent of child: " << this->tree_list.at(child).at(0) << endl;
	}
	if(this->root() == A_id || this->root() == B_id){
		this->_root_id = this->root() == A_id ? B_id : A_id;
	}
}

void B_star_tree::move(int A_id, int B_id){
	int parent, only_child, insert_child;
	int delete_op = 0;
	delete_op = this->is_leaf(A_id) ? 0 : ( this->have_children(A_id) ? 2 : 1);
	// cout << "delete_op: " << delete_op << endl;
	// if(A_id == this->parent_of(B_id) || B_id == this->parent_of(A_id)){
	// 	this->swap(A_id, B_id);
	// 	return;
	// }
	switch(delete_op){ // after deletion A_id will be a free node
		case 0:
			parent = this->parent_of(A_id);
			if(this->is_leftchild_of(parent, A_id))
				this->set_leftchild(parent, -1);
			else
				this->set_rightchild(parent, -1);
			this->set_parent(A_id, -1);
			break;

		case 1:
			parent = this->parent_of(A_id);
			only_child = this->leftchild_of(A_id) == -1 ? this->rightchild_of(A_id) : this->leftchild_of(A_id);
			if(this->is_leftchild_of(parent, A_id))
				this->set_leftchild(parent, only_child);
			else
				this->set_rightchild(parent, only_child);
			if(parent == -1)
				this->_root_id = only_child;
			this->set_parent(only_child, parent);
			this->set_parent(A_id, -1);
			this->set_leftchild(A_id, -1);
			this->set_rightchild(A_id, -1);
			break;

		case 2:
			int direction = 0;
			int chosen_child;
			while(!this->is_leaf(A_id)){
				direction =  rand()%2; // 0 = left, 1 = right
				chosen_child = direction == 0 ? this->leftchild_of(A_id) : this->rightchild_of(A_id);
				if(chosen_child == B_id){

				}
				if(chosen_child == -1)
					chosen_child = direction == 0 ? this->rightchild_of(A_id) : this->leftchild_of(A_id);
				this->swap(A_id, chosen_child);
			}
			parent = this->parent_of(A_id);
			if(this->is_leftchild_of(parent, A_id)){
				this->set_leftchild(parent, -1);
			}
			else
				this->set_rightchild(parent, -1);
			this->set_parent(A_id, -1);
			// this->print_relation(A_id);
			break;
	}
	int insert_direction = rand()%2; // 0 = left, 1 = right;
	// cout << "insert_direction: " << insert_direction << endl;
	insert_child = insert_direction == 0 ? this->leftchild_of(B_id) : this->rightchild_of(B_id);
	if(insert_child == -1){
		// cout << "leaf" << endl;
		this->set_parent(A_id, B_id);
		if(insert_direction == 0)
			this->set_leftchild(B_id, A_id);
		else
			this->set_rightchild(B_id, A_id);
		// cout << "leaf" << endl;
	}
	else{
		// cout << "non leaf" << endl;
		this->set_parent(A_id, B_id);
		this->set_parent(insert_child, A_id);
		if(insert_child == this->leftchild_of(B_id)){
			this->set_leftchild(B_id, A_id);
			this->set_leftchild(A_id, insert_child);
		}
		else{
			this->set_rightchild(B_id, A_id);
			this->set_rightchild(A_id, insert_child);
		}
		// cout << "non leaf" << endl;
	}
}

void B_star_tree::update_contour(int current){
	unsigned int start = this->tree_list.at(current).at(6);
	unsigned int end = this->tree_list.at(current).at(6) + this->tree_list.at(current).at(4);
	int max_y = 0;
	for(unsigned int index = start; index < end; index++){
		if( index < this->contour.size() )
			max_y = max_y > this->contour.at(index) ? max_y : this->contour.at(index);
		else
			this->contour.push_back(0);
	}
	for(unsigned int index = start; index < end; index++){
		this->contour.at(index) = max_y + this->tree_list.at(current).at(5);
	}
}

void B_star_tree::rotate_block(int current){
	int temp;
	this->tree_list.at(current).at(8) = !this->tree_list.at(current).at(8);
	temp = this->tree_list.at(current).at(4);
	this->tree_list.at(current).at(4) = this->tree_list.at(current).at(5);
	this->tree_list.at(current).at(5) = temp;
}

void B_star_tree::x_pack(int current){
	// cout << current << " ";
	if(current != -1){
		// cout << current << " ";
		if(this->rotate_status(current) != this->real_rotate_status(current))
			this->rotate_block(current);
		if(current == this->root()){
			this->reset_contour();
			this->root_pack(current);
			this->update_contour(current);
		}
		else{
			this->update_position_x(current, this->available_x_of(this->parent_of(current)));
			this->update_position_y(current);
			this->update_contour(current);
		}
		this->x_pack(this->leftchild_of(current));
		this->y_pack(this->rightchild_of(current));
	}
}

void B_star_tree::y_pack(int current){
	if(current != -1){
		if(this->rotate_status(current) != this->real_rotate_status(current))
			this->rotate_block(current);
		this->update_position_x(current, this->x_of(this->parent_of(current)));
		this->update_position_y(current);
		this->update_contour(current);
		this->x_pack(this->leftchild_of(current));
		this->y_pack(this->rightchild_of(current));
	}
}

void B_star_tree::reset_contour(){
	this->contour.clear();
	// this->contour.assign(B_star_tree::get_block_num(), 0);
}

B_star_tree::B_star_tree(){
	this->_root_id = -1;
	this->tree_list.clear();
	for(int index = 0; index < B_star_tree::get_block_num(); index++){
		vector<int> relationship;
		relationship.assign(3, -1);
		relationship.push_back(0);
		relationship.push_back(0);
		relationship.push_back(0);
		relationship.push_back(0);
		relationship.push_back(0);
		relationship.push_back(0);
		this->tree_list.push_back(relationship);
	}

	this->reset_contour();
}
