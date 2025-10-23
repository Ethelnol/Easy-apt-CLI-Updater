/** 
  * Created by Caitlyn Briggs on 03/09/2025
  **/

#include <iostream>
#include <sstream>

#include "out.h"

/**
  * Outputs line to std::cout without wrapping
  * @param bounds max number of characters to output
  * @param largestWord size of largest word in line
  * @post std::cout buffer will not be flushed
  **/
void textInBounds(const string& line, const uint32_t bounds, const string::size_type largestWord = 0){
	if (bounds < largestWord || line.length() < bounds){
		std::cout << line << std::flush;
		return;
	}

	std::stringstream ss(line);

	string buffer; //string to hold text to reduce std::cout calls
	string::size_type buffer_len = 0; //length of buffer
	buffer.reserve(bounds + 1);

	string str; //next str in line
	if (largestWord){str.reserve(largestWord);}

	while (!ss.eof() && ss >> str){
		buffer_len = buffer_len + str.length();

		//text exceeds terminal width
		if (buffer_len > bounds){
			buffer.push_back('\n');
			std::cout << buffer;

			buffer_len = str.size();
			buffer.clear();
		}

		//append str with space
		if (buffer_len != bounds){
			str.push_back(' ');
			++buffer_len;
		}

		buffer.append(str);
	}

	buffer.push_back('\n');
	std::cout << buffer;
}

void outputHelp(const uint32_t cols){
	const vector<vector<string>> page = {
			{"EasyUpdater"},
			{"Usage: EasyUpdater [options]"},
			{"",
					"EasyUpdater is a overhead program for apt and apt-get that makes the process of updating and removing packages faster with a single command.",
					"EasyUpdater cleanly displays data such as on the packages to be changes and displays the packages similar to apt v2.9.0's UI update.",
					"Any options passed to EasyUpdater will be passed when running updates, so be sure all arguments are vaild arguments for apt and apt-get. See apt(8) for more information."}
	};

	auto paragraph = page.begin();
	auto line = paragraph->begin();

	while (paragraph != page.end()){
		textInBounds(*line, cols, 12);

		//increment line or increment paragraph
		if (++line == paragraph->end() && ++paragraph != page.end()){line = paragraph->begin();}
	}
	std::cout << std::flush;
}

bool outputVector(const vector<string>& vect, const uint16_t cols){
	uint32_t maxCols = 1; //while loop decrements by 1

	for (uint32_t lineLength = 0, pac = 0; lineLength < cols && pac < vect.size(); ++pac){
		++maxCols;
		lineLength += 2 + vect.at(pac).length();
	}

	while (--maxCols){
		uint32_t maxRows = vect.size() / maxCols;

		if (vect.size() % maxCols){++maxRows;}

		vector<string::size_type> largestPerCol = getLargestInColumn(vect, maxRows);

		if (isValidLayout(largestPerCol, cols)){
			return vectorToStdout(vect, largestPerCol, maxRows);
		}
	}

	return vectorToStdout(vect, {}, UINT16_MAX);
}

vector<string::size_type> getLargestInColumn(const vector<string>& vect, const uint16_t maxRows){
	uint16_t col = vect.size() / maxRows;
	if (vect.size() % maxRows){++col;}

	vector<string::size_type> ret(col, 0);
	uint16_t row = col = 0;

	for (const auto& i : vect){
		if (ret.at(col) < i.length()){ret.at(col) = i.length();}
		if (++row == maxRows){
			row = 0;
			++col;
		}
	}

	return ret;
}

bool isValidLayout(const vector<string::size_type>& largestPerCol, const uint16_t cols){
	uint16_t len = 0;

	for (auto i : largestPerCol){
		len = 2 + len + i;
		if (len > cols){return false;}
	}

	return true;
}

bool vectorToStdout(const vector<string>& vect, const vector<string::size_type>& largestPerCol, uint16_t maxRows){
	auto itr = vect.begin();
	const auto end = vect.end();

	if (largestPerCol.size() < 2){
		while (itr != end){std::cout << "  " << *itr++ << '\n';}

		return (vect.begin() + 1 != end);
	}

	if (maxRows < 2){
		while (itr != end){std::cout << "  " << *itr++;}
		std::cout << '\n';

		return true;
	}

	uint16_t row = 0;
	auto lpc_itr = largestPerCol.begin();
	while (row < maxRows){
		std::cout << "  " << *itr << string(*lpc_itr++ - itr->size(), ' ');
		itr = itr + maxRows;

		if (itr >= end){
			itr = vect.begin() + ++row;
			lpc_itr = largestPerCol.begin();

			std::cout << '\n';

			continue;
		}
	}

	return true;
}