#include <stdio.h>
#include <cmath>
#include <vector>

/*
 *  for i in combinationCount
 *  	clear subset
 *  	for j in inputLength
 *  		if (i >> j) is odd
 *  			push j'th item unto subset
 *  	push subset unto powerSet
 *
 *  	how it works:
 *  		take input = [a,b]
 *  		take i == 0:
 *  			j == 0 -> 0 is even, so skip
 *  			j == 1 -> 0 is even, so skip
 *  			
 *  		push empty
 *  		take i == 1:
 *  			j == 0 -> 1 is odd, so add j'th element ("a")
 *  			j == 1 -> 0 is even, skip
 *
 *  		push ("a")
 *  		take i == 2:
 *  			j == 0 -> b10 is even, skip
 *  			j == 1 -> b01 is odd, add ("b")
 *  		push("b")
 *  		take i == 3: 
 *  			j == 0 -> b11 is odd, add ("a")
 *  			j == 1 -> b01 is odd, add ("b")
 *  		push("a","b")
 *
 *  	tl;dr - the binary value of the current iteration specifies which elements of the input
 *  		array to add to the power set
 *  			
 */

using namespace std;

vector<vector<int>> getPowerSet(int* input, size_t inputLength) {
	vector<vector<int>> powerSet;
	powerSet.resize(pow(2, inputLength)); 
	
	vector<int> subset;
	
	for (size_t i = 0, combinationCount = pow(2, inputLength); i < combinationCount; i++) {
		subset.clear();
		for (size_t j = 0; j < inputLength; j++) {
			if ((i >> j) % 2 == 1) {
				subset.push_back(input[j]);
			}
		}
		powerSet[i] = subset;
	}

	return powerSet;
}

int main() {
	int input[] = {1, 2, 3};
	size_t inputLength = sizeof(input) / sizeof(input[0]);
	vector<vector<int>> powerSet = getPowerSet(input, inputLength);
	
	printf("{");
	for (size_t i = 0; i < powerSet.size(); i++) {
		printf("{");
		for (size_t j = 0; j < powerSet[i].size(); j++) {
			printf((j == powerSet[i].size() - 1) ? "%d" : "%d, ", powerSet[i][j]);
		}
		printf("}");
	}
	printf("}");
	

}
