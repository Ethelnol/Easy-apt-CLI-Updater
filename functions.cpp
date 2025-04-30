/** 
  * Created by Caitlyn Briggs on 10 04 2025
  **/

#include "functions.h"

#include <iostream>

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

void outputVector(const vector<string>& vect, const size_t cols){
	size_t maxPerLine = 0;  //max number of packages per line
	size_t lineLength = 0;  //length of line

	for (const string& i : vect){
		lineLength += i.length() + 1;
		if (lineLength >= cols) break;

		maxPerLine++;
	}

	if (maxPerLine < 2){
		for (const string& j : vect)
			std::cout << "  " << j << std::endl;
		return;
	}

	size_t i;               //index of current package
	size_t largestLength = 0;     //size of package with longest name
	size_t numOnLine;

	for (const string& package : vect){
		if (package.length() > largestLength)
			largestLength = package.length();
	}

	while (i != vect.size()){
		numOnLine = 0;   //number of packages on current line
		lineLength = 0;
		for (i = 0; i < vect.size(); i++){
			lineLength += 2 + largestLength;
			numOnLine++;

			if (lineLength >= cols){
				maxPerLine--;
				break;
			}

			if (numOnLine == maxPerLine){
				numOnLine = 0;
				lineLength = 0;
			}
		}

		if (maxPerLine < 2){
			for (const string& j : vect)
				std::cout << "  " << j << std::endl;
			return;
		}
	}

	size_t j;
	size_t vectSize = vect.size();
	size_t sortSize = (vectSize / maxPerLine);
	if (vectSize % maxPerLine) sortSize++;
	for (i = 0; i < sortSize; i++){
		for (numOnLine = 0; numOnLine < maxPerLine; numOnLine++){
			if (i + (numOnLine * sortSize) >= vectSize) break;

			std::cout << "  " << vect.at(i + (numOnLine * sortSize));
			for (j = 0; j < (largestLength - vect.at(i + (numOnLine * sortSize)).length()); j++) std::cout << " ";
		}
		std::cout << std::endl;
	}
}

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