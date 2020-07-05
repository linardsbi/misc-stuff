#include <iostream>
#include <vector>
#include <cassert>
#include <math.h>

using namespace std;

/**
loop through input strings
	if current string is not going to make the line longer than max length
		if next string and next string is going to make line longer than max length
			add string to line
		else
			add string and space to line
		
		if current string is last
			add line to all lines
	else 
		calculate total padding
		if padding > 1 and padding % word count - 1 divides without remainder
			add padding % word count - 1 spaces to each word except the last one
		else if padding == 1
			add extra space to first word
		else if padding % word count - 1 divides with remainder
			calculate extra padding (padding - (padding / word count - 1))
			for word except last word in line
				add padding % word count - 1 spaces
				if extra padding is not 0
					add 1 space
					extra padding--
		
		add line to all lines
		reset line
		add string and space to line
*/

string padLine(vector<string> &line, const unsigned currentLineLength, const unsigned maxLineLength) {
	int totalPad = maxLineLength - currentLineLength;
	unsigned wordsToPad = (line.size() > 1) ? line.size() - 1 : line.size();
	int singlePad = totalPad / wordsToPad;
	string finishedLine = "";
	
	if (wordsToPad == 1) {
		line[0].resize(line[0].length() + totalPad, ' ');
	} else if (totalPad > 1 && (totalPad % wordsToPad) == 0) {
		for (size_t wordNum = 0; wordNum < line.size() - 1; wordNum++) {
			line[wordNum].resize(line[wordNum].length() + singlePad, ' ');
		}
	} else if (totalPad == 1) {
		line[0] += " ";
	} else if (totalPad > 1 && (totalPad % wordsToPad) == 1) {
		float extraPad = totalPad - ceil((float)totalPad / wordsToPad);
		
		for (size_t wordNum = 0; wordNum < line.size() - 1; wordNum++) {
			line[wordNum].resize(line[wordNum].length() + singlePad, ' ');
		
			if (extraPad != 0) {
				line[wordNum] += " ";
				extraPad--;
			}
		}
	} else if (totalPad == 0) {
		// no padding needed
	} else {
		cout << totalPad << endl;
		cout << wordsToPad;
		assert(totalPad > 0); // should never get here: current line is longer then max line length
	}
	
	for (size_t wordNum = 0; wordNum < line.size(); wordNum++) {
		finishedLine += line[wordNum];
	}
	
	return finishedLine;
}

vector<string> justify(string* input, const unsigned inputLen, const unsigned lineLength) {
	vector<string> lines;
	vector<string> line;
	unsigned currentLength = 0;
	bool wrap = false;
	
	for (size_t i = 0; i < inputLen; i++) {
		string word = input[i];
		
		if (currentLength + word.length() <= lineLength && !wrap) {
			line.push_back(word);
			currentLength += word.length();
		} else {
			lines.push_back(padLine(line, currentLength, lineLength));
			line.clear();
			
			line.push_back(word);
			currentLength = word.length();
			wrap = false;
		}
		
		if (i + 1 <= inputLen - 1 && (currentLength + input[i + 1].length()) <= lineLength) {
			// need a special case for length == lineLength to wrap and not put a space at line end
			if (currentLength + input[i + 1].length() == lineLength) {
				wrap = true;
			} else {
				line.back() += " ";
				currentLength++;
			}
		}
	}
	
	if (line.size() > 0) {
		lines.push_back(padLine(line, currentLength, lineLength));
	}
	
	return lines;
}

int main() {
	string input[] = {"the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"};
	unsigned inputLen = sizeof(input) / sizeof(input[0]);
	unsigned k = 16;
	
	vector<string> justified = justify(input, inputLen, k);
	
	for (auto &line: justified) {
		cout << line;
		cout << endl;
	}
}