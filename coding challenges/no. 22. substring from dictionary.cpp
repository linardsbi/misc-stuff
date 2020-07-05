#include <iostream>

using namespace std;

int main() {
	string str = "bedbathandbeyond";
	string dict[] = {"bedbath","bed","bath","and","beyond"};
	size_t dictLength = sizeof(dict) / sizeof(dict[0]);
	size_t pos = -1;
	
	string found[dictLength] = {""};
	size_t foundLength = -1;
	
	for (unsigned i = 0; i < dictLength; i++) {
		pos = str.find(dict[i]);
		
		if (pos != -1) {
			foundLength++;
			found[foundLength] = str.substr(pos, dict[i].length());
			str.erase(pos, dict[i].length());
			
			cout << found[foundLength] << " ";
		}
		
	}
	
}