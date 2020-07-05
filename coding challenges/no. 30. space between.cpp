#include <iostream>

using namespace std;

/**
i'm guessing that this is O(1) additional space, 
since O(1) total space is impossible because the input length is not fixed
so additional space is always constant - 4+4+4+4 bytes
*/
unsigned getFilledCells(const unsigned* heightMap, const size_t inputSize) {
	unsigned highest = 0;
	unsigned totalCells = 0;
	
	for (size_t i = 0; i < inputSize; i++) {
		if (i == 0) {
			highest = heightMap[0];
			continue;
		}
		if (heightMap[i] < highest) {
			totalCells += highest - heightMap[i];
		} else {
			highest = heightMap[i];
		}
	}
	
	return totalCells;
}

int main() {
	unsigned input1[] = {2,1,2};
	unsigned input2[] = {3,0,1,3,0,5};
	unsigned inputSize = sizeof(input2) / sizeof(input2[0]);
	
	unsigned output = getFilledCells(input2, inputSize);
	
	cout << output << endl;
}