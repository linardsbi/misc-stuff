#include <iostream>

using namespace std;

class Node {
	public:
	Node* next = nullptr;
	int value = 0;
};

class LinkedList {
	public:
	
	int length() {
		return this->size;
	}
	
	void push(int value) {
		if (size == 0) {
			firstNode = new Node();
			firstNode->value = value;
			head = firstNode;
		} else {
			Node* newNode = new Node();
			
			if (size == 1) {
				firstNode->next = newNode;
			}
			
			newNode->value = value;
			
			head->next = newNode;
			head = newNode;
		}
		
		size++;
	}
	
	int pop(const bool front = false) {
		Node* temp = this->firstNode;
		int returnVal = 0;
		
		if (this->size < 1 || this->firstNode == nullptr) return 0;
		
		if (this->firstNode == this->head) {
			returnVal = temp->value;
			this->firstNode = nullptr;
			this->head = nullptr;
			
			this->size--;
			return returnVal;
		}
		
		if (front) {
			this->firstNode = this->firstNode->next;
			returnVal = this->firstNode->value;
			
			delete temp;
		} else {
			
			while (true) {
				if (temp->next == this->head) {
					this->head = temp;
					
					delete this->head->next;
					
					this->head->next = nullptr;
					break;
				}
				
				temp = temp->next;
			}
			
			returnVal = this->head->value;
		}
		
		this->size--;
		
		return returnVal;
	}
	
	Node* begin() {
		return this->firstNode;
	}
	Node* end() {
		return this->head;
	}
	
	void sort() {
		*this = this->mergeSort(*this);
	}
	
	LinkedList& operator+(LinkedList& list) {
		if (this->firstNode == nullptr) {
			this->firstNode = list.begin();
			this->head = list.end();
		} else {
			this->head->next = list.begin();
			this->head = list.end();
		}
	
		this->size += list.length();
	
		return *this;
	}
	
	static Node* firstIntersection(LinkedList left, LinkedList right) {
		LinkedList newList = left + right;
		newList.sort();
		
		for (Node* n = newList.begin(); n != nullptr; n = n->next) {
			if (n->next != nullptr && n->value == n->next->value) {
				return n;
			}
		}
		
		return nullptr;
	}
	
	private:
	Node* firstNode = nullptr;
	Node* head = nullptr;
	unsigned int size = 0;
	
	LinkedList mergeSort(LinkedList list) {
	
		if (list.length() <= 1) return list;
		
		LinkedList left;
		LinkedList right;
		
		unsigned i = 0;
		
		for (Node* n = list.begin(); n != nullptr; n = n->next) {
			if (i < (list.length() / 2)) {
				left.push(n->value);
			} else {
				right.push(n->value);
			}
			
			i++;
		}
		
		left = this->mergeSort(left);
		right = this->mergeSort(right);
		
		return this->merge(left, right);
	}
	
	LinkedList merge(LinkedList left, LinkedList right) {
		LinkedList result;
		
		while (left.length() > 0 && right.length() > 0) {
			if (left.begin()->value <= right.begin()->value) {
				result.push(left.begin()->value);
				left.pop(true);
				
			} else {
				result.push(right.begin()->value);
				right.pop(true);
			}
		}

		while (left.length() > 0) {
			result.push(left.begin()->value);
			left.pop(true);
		}
		
		while (right.length() > 0) {
			result.push(right.begin()->value);
			right.pop(true);
		}
		
		return result;
	}
};

int main() {
	LinkedList list1;
	LinkedList list2;
	
	list1.push(2);
	list1.push(5);
	list1.push(7);
	list1.push(1);
	list1.push(7);
	list1.push(7);
	
	list2.push(7);
	list2.push(8);
	list2.push(9);
	list2.push(-1);
	list2.push(0);
	
	Node* intersection = LinkedList::firstIntersection(list1, list2);
	
	if (intersection != nullptr) {
		cout << intersection->value;
	}
}