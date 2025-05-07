/** 
  * Created by Caitlyn Briggs on 04/10/2025
  **/

#include <iostream>
#include <sstream>

#include "functions.h"

/**
  * Gets output of stdout and stderr and returns output as string
  * @param cmd command to be passed to system
  * @return string of stdout and stderr from cmd
  * @author Jeremy Morgan
  * @source www.jeremymorgan.com/tutorials/c-programming/how-to-capture-the-output-of-a-linux-command-in-c
  **/
string GetStdoutFromCommand(string cmd){
	string data;
	FILE* stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");

	stream = popen(cmd.c_str(), "r");

	if (stream){
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream)) data.append(buffer);
		pclose(stream);
	}
	return data;
}

void getPackages(vector<vector<string>>& packages){
	std::stringstream ss(GetStdoutFromCommand("sudo apt-get dist-upgrade --assume-no"));
	string line;
	string package;
	string overflow;

	int8_t type;

	while (!ss.eof() && getline(ss, line)){
		line.insert(0, overflow);
		overflow = "";
		if (line.empty() || line.at(0) != 'T') continue;

		switch(static_cast<uint8_t>(line.at(33))){
			case 103:
				type = UPGRADE;
				break;
			case 101:
				type = INSTALL;
				break;
			case 97:
			case 105:
				type = REMOVE;
				break;
			case 77:
				type = NOW_REMOVE;
				break;
			case 107:
				type = WITHHELD;
				break;
			default:
				type = -1;
				break;
		}

		if (type != -1){
			while (ss >> package && !package.empty() && std::islower(package.at(0)))
				packages.at(type).push_back(package);
			overflow = package;
			packages.at(type).shrink_to_fit();
		}
	}
}

bool getOpts(const int argc, char* argv[], string& args, bool flags[]){
	string tmpStr;
	vector<string> argsVect;

	for (int i = 1; i < argc; i++){
		tmpStr = argv[i];

		if (tmpStr.size() < 2 || tmpStr.at(2) == '-'){
			std::cerr << "getOpts(): invalid parameter - " << tmpStr << std::endl;
			exit(1);
		}

		if (tmpStr.size() == 2 ||
		    (tmpStr.substr(0, 2) == "--" &&
		     tmpStr.size() > 2 &&
		     std::isalnum(tmpStr.at(2)))
		   ) argsVect.push_back(tmpStr);
		else{
			string dash = "-";
			for (int j = 1; j < tmpStr.size(); j++)
				argsVect.push_back(dash + tmpStr.at(j));
		}
	}

	for (const string& i : argsVect){
		if (i == "-h" || i == "--help"){
			std::cout << "Help" << std::endl;
			return false;
		}
		else if (i == "-q" || i == "--quiet") flags[quiet] = true;
		else if (i == "-y" || i == "--yes" || i == "--assume-yes") flags[yes] = true;
		else if (i == "-s" || i == "--simulate" || i == "--just-print" ||
		         i == "--dry-run" || i == "--recon" || i == "--no-act" || i == "--assume-no")
			flags[sim] = true;

		args += " " + i;
	}

	return true;
}

/**
  * outputVector() helper function, returns length of largest package in column
  * @param column column to check
  * @param maxRows maximum number of packages in each column
  * @return length of largest package in column
  **/
size_t getLargestInColumn(const vector<string>& vect, const size_t col, const size_t maxRows){
	size_t returnNum = 0;

	for (size_t n = (col * maxRows); n < ((col + 1) * maxRows) && n < vect.size(); n++)
		if (vect.at(n).length() > returnNum) returnNum = vect.at(n).length();

	return returnNum;
}

/**
  * outputVector() helper function, outputs packages in vect cleanly
  * @param vect vector of packages to be output
  * @param largestPerCol vector of largest package length in each column, nullptr for single column output
  * @pre largestPerCol must be nullptr or not be empty
  * @post output sent to stdout
  */
void vectorToStdout(const vector<string>& vect, const vector<size_t>* largestPerCol, const size_t maxRows){
	using std::cout, std::endl;

	if (!largestPerCol){
		for (const string& pac : vect)
			cout << "  " << pac << endl;
		return;
	}

	if (maxRows < 2){
		for (const string& pac : vect)
			cout << "  " << pac;
		cout << endl;
		return;
	}

	const size_t maxCols = largestPerCol->size();

	for (size_t row = 0; row < maxRows; row++){
		for (size_t col = 0, pac = row; col < maxCols && pac < vect.size(); col++, pac += maxRows){
			cout << "  " << vect.at(pac);

			for (size_t k = vect.at(pac).size(); k < largestPerCol->at(col); k++) cout << ' ';
			cout << std::flush;
		}
		cout << endl;
	}
}

/**
  * outputVector() helper function, checks if packages of vect can fit into desired number of columns without exceeding terminal width
  * @param vect vector of packages to check layout validity
  * @param largestPerCol vector of largest package size in each column
  * @param cols width of terminal window
  * @pre largestPerCol has size of desired number of columns
  * @return true if vect packages fit specified layout
  **/
bool isValidLayout(const vector<string>& vect, const vector<size_t>& largestPerCol, const size_t maxRows, const size_t cols){
	const size_t maxCols = largestPerCol.size();
//	const size_t maxRows = (vect.size() % maxCols ? 1 : 0) + (vect.size() / maxCols);

	for (size_t row = 0; row < maxRows; row++){
		size_t lineLength = 0;
			for (size_t col = 0, pac = row; col < maxCols && pac < vect.size(); col++, pac += maxRows){

			lineLength += 2 + largestPerCol.at(col);

			if (lineLength > cols) return false;
		}
	}

	return true;
}

void outputVector(const vector<string>& vect, const size_t cols){
	size_t maxCols = 0;
	size_t maxRows;
	size_t i = 0;

	for (const string& j : vect){
		i += 2 + j.length();
		if (i >= cols) break;

		maxCols++;
	}

	for (i = maxCols; i > 1; i--){
		vector<size_t> largestPerCol(i);
		maxRows = ((vect.size() % i) ? 1 : 0) + (vect.size() / i);

		for (size_t column = 0; column < i; column++)
			largestPerCol.at(column) = getLargestInColumn(vect, column, maxRows);

		if (isValidLayout(vect, largestPerCol, maxRows, cols))
			return vectorToStdout(vect, &largestPerCol, maxRows);
	}

	vectorToStdout(vect, nullptr, SIZE_MAX);
}