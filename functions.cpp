/**
  * Created by Caitlyn Briggs on 04/10/2025
  **/

#include <cassert>
#include <cstdint>  //SIZE_MAX
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

		/*
		 * {33,g,e,a,i,M,k}
		 * {36,d,n,c,l,E,t}
		 * {38,d,t,l,y,:,b}
		 */
		switch(line.at(33)){
			case 'g':
				type = UPGRADE;
				break;
			case 'e':
				type = INSTALL;
				break;
			case 'a':
			case 'i':
				type = REMOVE;
				break;
			case 'M':
				type = NOW_REMOVE;
				break;
			case 'k':
				type = WITHHELD;
				break;
			default:
				std::cerr << "getPackages(), unknown line passed - " << line << std::endl;
				exit(line.at(33));
		}

		while (ss >> package && !package.empty() && std::islower(package.at(0)))
			packages.at(type).push_back(package);
		overflow = package;
		packages.at(type).shrink_to_fit();
	}
}

/**
  * getOpts helper function, outputs help information if "-h" or "--help" are passed as arguments
  **/
void outputHelp(const size_t cols){
	using std::cout, std::endl;

	const vector<vector<string>> page = {
			{"EasyUpdater"},
			{"Usage: EasyUpdater [options]"},
			{
				"EasyUpdater is a overhead program for apt and apt-get that makes the process of updating and removing packages faster with a single command.",
				"EasyUpdater cleanly displays data such as on the packages to be changes and displays the packages similar to apt v2.9.0's UI update.",
				"Any options passed to EasyUpdater will be passed when running updates, so be sure all arguments are vaild arguments for apt and apt-get.  See apt(8) for more information."
			}
	};

	for (size_t paragraph = 0; paragraph < page.size(); paragraph++){
		for (const string& line : page.at(paragraph)){
			std::stringstream ss(line);
			string word;
			size_t lineLength = 0;
			bool vomit = false, first = true;

			while (!ss.eof() && ss >> word){
				if (lineLength + word.size() + 1 > cols){
					if (first && lineLength == 0){
						vomit = true;
						break;
					}

					cout << endl;
					lineLength = 0;
					first = false;
				}

				cout << word << " " << std::flush;
				lineLength += word.size() + 1l;
			}

			if (vomit) cout << line;

			cout << endl;
		}

		if ((paragraph + 1) < page.size()) cout << endl;
	}
}

bool getOpts(const int argc, char* argv[], string& args, bool flags[], const size_t cols){
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
			outputHelp(cols);
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
  * outputVector helper function, outputs packages in vect cleanly
  * @param vect vector of packages to be output
  * @param largestPerCol vector of largest package length in each column
  * @post output sent to stdout
  * @return true if output
  */
bool vectorToStdout(const vector<string>& vect, const vector<size_t>& largestPerCol, const size_t maxRows){
	using std::cout, std::endl;

	const size_t maxCols = largestPerCol.size();

	if (maxCols < 2){
		for (const string& pac : vect)
			cout << "  " << pac << endl;
		return (maxCols == 1);
	}

	if (maxRows < 2){
		for (const string& pac : vect)
			cout << "  " << pac;
		cout << endl;
		return true;
	}

	for (size_t row = 0; row < maxRows; row++){
		for (size_t col = 0, pac = row; col < maxCols && pac < vect.size(); col++, pac += maxRows){
			cout << "  " << vect.at(pac);

			for (size_t k = vect.at(pac).size(); k < largestPerCol.at(col); k++)
				cout << ' ';
		}
		cout << endl;
	}

	return true;
}

/**
  * outputVector helper function, checks if packages of vect can fit into desired number of columns without exceeding terminal width
  * @param vect vector of packages to check layout validity
  * @param largestPerCol vector of largest package size in each column
  * @param cols width of terminal window
  * @return true if vect packages fit specified layout
  **/
bool isValidLayout(const vector<string>& vect, const vector<size_t>& largestPerCol, const size_t maxRows, const size_t cols){
	const size_t maxCols = largestPerCol.size();

	for (size_t row = 0; row < maxRows; row++){
		size_t lineLength = 0;	//current num of chars on line
		size_t pac = row;		//current package index, used to not exceed vect.size()

		for (size_t col = 0; col < maxCols && pac < vect.size(); col++, pac += maxRows){
			lineLength += 2 + largestPerCol.at(col);

			if (lineLength > cols) return false;
		}
	}

	return true;
}

/**
  * outputVector helper function, returns length of largest package in column
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

bool outputVector(const vector<string>& vect, const size_t cols){
	assert(!vect.empty());
	size_t maxCols = 0;

	for (size_t lineLength = 0, pac = 0; lineLength < cols && pac < vect.size(); maxCols++, pac++)
		lineLength += 2 + vect.at(pac).length();

	for (; maxCols > 1; maxCols--){
		size_t maxRows = ((vect.size() % maxCols) ? 1 : 0) + (vect.size() / maxCols);
		vector<size_t> largestPerCol(maxCols);

		for (size_t column = 0; column < maxCols; column++)
			largestPerCol.at(column) = getLargestInColumn(vect, column, maxRows);

		if (isValidLayout(vect, largestPerCol, maxRows, cols))
			return vectorToStdout(vect, largestPerCol, maxRows);
	}

	return vectorToStdout(vect, vector<size_t>(0), SIZE_MAX);
}
