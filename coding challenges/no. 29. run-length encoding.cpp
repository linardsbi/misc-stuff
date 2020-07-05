#include <iostream>
#include <cassert>
#include <cstring>

using namespace std;

string encode(const char* input, const unsigned inputLength) {
	string encoded = "";
	unsigned currentCharCount = 0;
	
	for (size_t i = 0; i < inputLength; i++) {
		if (i > 0 && input[i] != input[i - 1]) {
			encoded += to_string(currentCharCount) + input[i - 1];
			currentCharCount = 1;
		} else {
			currentCharCount++;
		}
	}
	
	return encoded;
}

string decode(const char* input, const unsigned inputLength) {
	string decoded = "";
	unsigned currentCharCount = 0;
	
	for (size_t i = 0; i < inputLength - 1; i += 2) {
		decoded.resize(decoded.length() + (input[i] - '0'), input[i + 1]);
	}
	
	return decoded;
}

int main() {
	const char input[] = "AAAABBBCCDAA";
	unsigned inputLength = sizeof(input) / sizeof(input[0]);
	
	string encoded = encode(input, inputLength);
	string decoded = decode(encoded.c_str(), encoded.length());
	
	assert(strcmp(input, decoded.c_str()) == 0);
}