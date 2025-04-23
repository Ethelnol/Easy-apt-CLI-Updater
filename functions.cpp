/** 
  * Created by Caitlyn Briggs on 10 04 2025
  **/

#include "functions.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/ioctl.h> //for rows and cols
#include <unistd.h> //for rows and cols

void getPackages(vector<vector<string>>& packages){
	std::stringstream ss(GetStdoutFromCommand("sudo apt-get dist-upgrade --assume-no"));
	string line;
	string package;

	while (!ss.eof()){
		getline(ss, line);
		if (line.empty() || line.at(0) != 'T') continue;

		if (line == "The following packages will be upgraded:"){
			ss >> package;
			while (!package.empty() && isalpha(package.at(0))){
				packages.at(UPGRADE).push_back(package);
				ss >> package;
			}
			//packages.at(UPGRADE) = sortVectorAlphabetically(packages.at(UPGRADE));
		}
		else if (line == "The following NEW packages will be installed:"){
			ss >> package;
			while (!package.empty()){
				packages.at(INSTALL).push_back(package);
				ss >> package;
			}
			//packages.at(INSTALL) = sortVectorAlphabetically(packages.at(INSTALL));
		}
		else if (line == "The following packages were automatically installed and are no longer required:" ||
		         line == "The following package was automatically installed and is no longer required:"){
			ss >> package;
			while (!package.empty() && std::islower(package.at(0))){
				packages.at(REMOVE).push_back(package);
				ss >> package;
			}
			//remove = sortVectorAlphabetically(remove);
		}
		else if (line == "The following packages will be REMOVED:"){
			ss >> package;
			while (!package.empty()){
				packages.at(NOW_REMOVE).push_back(package);
				ss >> package;
			}
			//packages.at(NOW_REMOVE) = sortVectorAlphabetically(packages.at(NOW_REMOVE));
		}
		else if (line == "The following packages have been kept back:"){
			ss >> package;
			while (!package.empty()){
				packages.at(WITHHELD).push_back(package);
				ss >> package;
			}
			//packages.at(WITHHELD) = sortVectorAlphabetically(packages.at(WITHHELD));
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
	size_t maxPerLine = 1;
	string line;

	for (size_t i = 0; i < line.size() && line.size() < cols; i++){
		line += "  " + vect.at(i);
		maxPerLine++;
	}

	maxPerLine--;

	vector<string> outputVect;

	if (maxPerLine < 2 || cols == 0){
		for (const string& j : vect)
			std::cout << "  " << j << std::endl;
		return;
	}

	line = "";
	size_t numOnLine = 0;
	size_t i = 0;

	while (i != vect.size() && maxPerLine > 2){
		for (i = 0; i < vect.size(); i++){
			if (numOnLine == maxPerLine){
				outputVect.push_back(line);
				numOnLine = 0;
				line = "";
			}

			line += "  " + vect.at(i);

			if (line.size() >= cols){
				outputVect.resize(0);
				numOnLine = 0;
				maxPerLine--;
				break;
			}

			numOnLine++;
		}

		if (maxPerLine == 1)
			for (const string& j : vect)
				outputVect.push_back("  " + j);
	}

	for (const string& j : outputVect)
		std::cout << j << std::endl;
}

char toLower(char c){
	if (!isalpha(c)) return c;
	return static_cast<char>(tolower(c));
}

/**
  * Checks if str1 is alphabetically smaller than str2
  * @param str1 to check
  * @param str2 to check str1 against
  * @return true if str1 is alphabetically smaller than str2
  * @return false if str1 is the same as str2 or is alphabetically larger than str2
  **/
/*
bool isSmallerAlphabetically(const string& str1, const string& str2){
	for (size_t i = 0; i < std::min(str1.size(), str2.size()); i++){
		if (toLower(str1.at(i)) < toLower(str2.at(i)))
			return true;
		if (toLower(str1.at(i)) > toLower(str2.at(i)))
			return false;
	}
	//if the sizes are the same, they must be the same string (with potentially different capitalisation)
	if (str1.size() == str2.size()) return false;
	//if str1 is bigger than str2, then str1 is alphabetically larger
	return (std::min(str1.size(),str2.size()) == str1.size());
}
 */

/*
vector<string> sortVectorAlphabetically(vector<string>& vect){
	if (vect.empty()) return vect;

	vector<string> sorted = vect;
	size_t i = 0;

	while (i != sorted.size() - 1)
		for (i = 0; i < sorted.size() - 1; i++)
			if (isSmallerAlphabetically(sorted.at(i + 1), sorted.at(i)) &&
			    sorted.at(i + 1) != sorted.at(i)){
				string tmpStr = sorted.at(i + 1);
				sorted.at(i + 1) = sorted.at(i);
				sorted.at(i) = tmpStr;
				break;
			}

	return sorted;
}
*/

/*
bool upgradePackage(vector<string> upgrade, const bool quiet){
	vector<string> hasDependencies;
	vector<string> upgraded;
	upgraded.resize(upgrade.size());

	while (!upgrade.empty()){


	}
}
 */

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