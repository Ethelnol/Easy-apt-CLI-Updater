/** 
  * Created by Caitlyn Briggs on 10 04 2025
  **/

#include "functions.h"

#include <iostream>

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

	size_t type;

	while (!ss.eof() && getline(ss, line)){
		type = SIZE_MAX;
		if (!overflow.empty()){
			overflow += line;
			line = overflow;
			overflow = "";
		}
		if (line.empty() || line.at(0) != 'T') continue;

		if (line == "The following packages will be upgraded:") type = UPGRADE;
		else if (line == "The following NEW packages will be installed:") type = INSTALL;
		else if (line == "The following packages were automatically installed and are no longer required:" ||
		         line == "The following package was automatically installed and is no longer required:") type = REMOVE;
		else if (line == "The following packages will be REMOVED:") type = NOW_REMOVE;
		else if (line == "The following packages have been kept back:") type = WITHHELD;

		if (type != SIZE_MAX){
			while (ss >> package && !package.empty() && std::islower(package.at(0)))
				packages.at(type).push_back(package);
			overflow = package;
		}
	}
}

bool getOpts(const int argc, char* argv[], string& args, bool& quiet, bool& assumeYes, bool& simulate){
	string tmpStr;
	vector<string> argsVect;

	for (int i = 1; i < argc; i++){
		tmpStr = argv[i];

		if (tmpStr.size() < 2 || tmpStr.at(2) == '-'){
			std::cerr << "getOpts(): invalid parameter - " << tmpStr << std::endl;
			exit(1);
		}

		if (tmpStr.size() == 2 || tmpStr.substr(0, 2) == "--")
			argsVect.push_back(tmpStr);
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
		else if (i == "-q" || i == "--quiet") quiet = true;
		else if (i == "-y" || i == "--yes" || i == "--assume-yes") assumeYes = true;
		else if (i == "-s" || i == "--simulate" || i == "--just-print" ||
		         i == "--dry-run" || i == "--recon" || i == "--no-act" || i == "--assume-no")
			simulate = true;

		args += " " + i;
	}

	return true;
}

/**
  * outputVector() helper function, returns length of largest package in vect
  * @param column column to check packages for
  * @param maxPerLine max number of packages on each line, max number of columns
  * @return length of largest package in column
  **/
size_t getLargestInColumn(const vector<string>& vect, const size_t column, const size_t maxPerLine){
	size_t returnNum = 0;
	for (size_t i = column; i < vect.size(); i += maxPerLine)
		if (vect.at(i).length() > returnNum) returnNum = vect.at(i).length();

	return returnNum;
}

/**
  * outputVector() helper function, outputs packages in vect cleanly
  * @param vect vector of packages to be output
  * @param largestPerCol vector of largest package length in each column, nullptr for single column output
  * @pre largestPerCol must be nullptr or not be empty
  * @post output sent to stdout
  */
void vectorToStdout(const vector<string>& vect, const vector<size_t>* largestPerCol){
	using std::cout, std::endl;

	const size_t numCols = (largestPerCol) ? largestPerCol->size() : 1;

	vector<vector<string>> layout;
	layout.resize(numCols);

	for (size_t p = 0; p < vect.size(); p++)
		layout.at((p % numCols)).push_back(vect.at(p));

	for (const vector<string>& r : layout){
		size_t index = 0;
		for (const string& p : r){
			cout << "  " << p;

			if (!largestPerCol){
				cout << endl;
				continue;
			}

			for (size_t i = 0; i < largestPerCol->at(index++) - p.length(); i++) cout << " ";
		}
		if (largestPerCol) cout << endl;
	}
}

void outputVector(const vector<string>& vect, const size_t cols){
	size_t maxPerLine = 0;			//max number of packages per line
	size_t lineLength = 0;			//number of chars currently on line
	size_t p = 0;					//index of current package
	vector<size_t> largestPerCol;	//vector of largest package length for each column

	for (const string& i : vect){
		lineLength += i.length() + 1;
		if (lineLength >= cols) break;

		maxPerLine++;
	}

	while (p != vect.size()){
		if (maxPerLine < 2) return vectorToStdout(vect, nullptr);

		lineLength = 0;
		largestPerCol.resize(maxPerLine);

		//must get largest package in column for all new columns
		for (p = 0; p < maxPerLine; p++)
			largestPerCol.at(p) = getLargestInColumn(vect, p, maxPerLine);

		for (p = 0; p < vect.size(); p++){
			//2 for mandatory spaces, then largest size from column
			lineLength += 2 + largestPerCol.at(p % maxPerLine);

			//if number of chars on line exceeds width of terminal
			if (lineLength >= cols){
				maxPerLine--;
				break;
			}

			if ((p % maxPerLine) == maxPerLine)
				lineLength = 0;
		}
	}

	vectorToStdout(vect, &largestPerCol);
}