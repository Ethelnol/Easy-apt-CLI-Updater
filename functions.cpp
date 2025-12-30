/*
 * Created by Caitlyn Briggs on 04/10/2025
 */

#include <iostream>
#include <sstream>

#include "functions.h"
#include "out.h"

extern bool flags[];

/*
 * Gets output of stdout and stderr and returns output as string
 * @param cmd command to be passed to system
 * @return string of stdout and stderr from cmd
 * @author Jeremy Morgan
 * @source www.jeremymorgan.com/tutorials/c-programming/how-to-capture-the-output-of-a-linux-command-in-c
 */
string getStdoutFromCommand(const string& cmd){
	string        data;
	FILE*         stream     = popen((cmd + " 2>&1").c_str(), "r");
	constexpr int MAX_BUFFER = 256;
	char          buffer[MAX_BUFFER];

	if (stream){
		while (!feof(stream)){
			if (fgets(buffer, MAX_BUFFER, stream)){
				data.append(buffer);
			}
		}
		pclose(stream);
	}

	return data;
}

void sortPackage(std::stringstream& ss, array<vector<string>, 5>& packages, string& line, pkgType type){
	string tmpStr; //holds package
	while (getline(ss, tmpStr, '\n') && tmpStr[0] == ' '){
		std::stringstream ss2(tmpStr);
		while (!ss2.eof() && ss2 >> tmpStr){
			packages.at(type).push_back(tmpStr);
		}
	}

	line = tmpStr;
	packages[type].shrink_to_fit();
}

void getPackages(array<vector<string>, 5>& packages){
	std::stringstream ss(getStdoutFromCommand("sudo apt-get full-upgrade --assume-no"));
	string            line;        //entire line
	string            overflow;    //overflow string

	pkgType type;

	while (!ss.eof()){
		if (line.empty() || line.front() != 'T'){
			getline(ss, line);
			continue;
		}

		/* apt-get lines that denote package types where
		 * the nth character is different across all 6 messages
		 * {33,g,e,a,i,M,k}
		 * {36,d,n,c,l,E,t}
		 * {38,d,t,l,y,:,b}
		 */
		switch (line.at(33)){
			case 'g':{
				type = UPGRADE;
				break;
			}
			case 'e':{
				type = INSTALL;
				break;
			}
			case 'a':
			case 'i':{
				type = REMOVE;
				break;
			}
			case 'M':{
				type = NOW_REMOVE;
				break;
			}
			case 'k':{
				type = WITHHELD;
				break;
			}
			default:{
				std::cerr << "\ngetPackages(), unknown line passed - " << line
				<< "\n                                                                    _\n";
				exit(line.at(33));
			}
		}

		sortPackage(ss, packages, line, type);
	}
}

void checkForImageUpdate(array<vector<string>, 5>& packages){
	for (string& i : packages[INSTALL]){
		if (i.substr(0, 6) == "linux-"){
			getPackages(packages);
			return;
		}
	}
}

bool getOpts(const int argc, char* argv[], string& args){
	vector<string> argsVect;

	for (auto i = 1; i < argc; i++){
		string tmpStr(argv[i]);

		if (tmpStr.size() < 2 || (tmpStr.size() > 3 && tmpStr.at(2) == '-')){
			std::cerr << "Error: invalid parameter - " << tmpStr << std::endl;
			exit(1);
		}

		if (tmpStr.size() == 2 ||
		    (tmpStr[0] == '-' && tmpStr[1] == '-' && std::isalnum(tmpStr.at(2)))
		){
			argsVect.push_back(tmpStr);
		}
		else{
			for (auto j = 1; j < tmpStr.size(); j++){
				argsVect.emplace_back('-', tmpStr.at(j));
			}
		}
	}

	for (const string& i : argsVect){
		if (i == "-h" || i == "--help"){
			return false;
		}

		if (i == "-q" || i == "--quiet"){
			flags[QUIET] = true;
		}
		else if (i == "-y" || i == "--yes" || i == "--assume-yes"){
			flags[YES] = true;
		}
		else if (i == "-s" || i == "--simulate" ||
		         i == "--just-print" || i == "--dry-run" ||
		         i == "--recon" || i == "--no-act" || i == "--assume-no"){
			flags[SIM] = true;
		}

		args.append(' ' + i);
	}

	return true;
}
