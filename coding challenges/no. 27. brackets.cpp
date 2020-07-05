#include <iostream>
#include <vector>

using namespace std;

bool wellFormed(string str) {
	vector<char> tokens;
	
	for (unsigned i = 0; i < str.length(); i++) {
		if (str[i] == '[' || str[i] == '(' || str[i] == '{') {
			tokens.push_back(str[i]);
		}
		
		if (str[i] == ']' && tokens.back() == '[' || str[i] == ')' && tokens.back() == '(' || str[i] == '}' && tokens.back() == '{') {
			tokens.pop_back();
		}
	}
	
	return tokens.size() == 0;
}

int main() {
	string cases[] = {"([])[]({})", "([)]", "((()"};
	
	for (string &testCase: cases) {
		cout << "case: " << testCase << " returned " << ((wellFormed(testCase)) ? "true" : "false") << endl;
	}
}