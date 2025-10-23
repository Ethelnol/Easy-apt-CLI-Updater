/**
  * Created by Caitlyn Briggs on 04/10/2025
  **/

#include <iostream>
#include <sstream>

#include "functions.h"
#include "out.h"

extern bool flags[];

/**
  * Gets output of stdout and stderr and returns output as string
  * @param cmd command to be passed to system
  * @return string of stdout and stderr from cmd
  * @author Jeremy Morgan
  * @source www.jeremymorgan.com/tutorials/c-programming/how-to-capture-the-output-of-a-linux-command-in-c
  **/
string GetStdoutFromCommand(const string& cmd){
	string data;
	FILE* stream;
	const int max_buffer = 256;
	char buffer[max_buffer];

	stream = popen((cmd + " 2>&1").c_str(), "r");

	if (stream){
		while (!feof(stream)){
			if (fgets(buffer, max_buffer, stream)){data.append(buffer);}
		}
		pclose(stream);
	}

	return data;
}

void getPackages(array<vector<string>, 5>& packages){
	std::stringstream ss(GetStdoutFromCommand("sudo apt-get full-upgrade --assume-no"));
	string line;        //entire line
	string overflow;    //overflow string

	pkgType type;

	while (!ss.eof() && getline(ss, line)){
		line.assign(overflow + line);
		if (line.empty() || line.front() != 'T'){continue;}

		/* apt-get lines that denote package types where
		 * the nth character is different across all 6 messages
		 * {33,g,e,a,i,M,k}
		 * {36,d,n,c,l,E,t}
		 * {38,d,t,l,y,:,b}
		 */
		switch(line.at(33)){
			case 'g':{
				type = UPGRADE;
				break;
			}
			case 'e':{
				type = INSTALL;
				break;
			}
			case 'a': case 'i':{
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
				string errorBar(71, ' '); //\n and highlights offending char
				//         1 + (36 + 33) + 1
				errorBar[0] = '\n';
				errorBar[70] = '_';
				std::cerr << "\ngetPackages(), unknown line passed - " << line
				          << errorBar << std::endl;
				exit(line.at(33));
			}
		}

		string package;
		while (ss >> package && !package.empty() && std::islower(package.front())){
			packages.at(type).push_back(package);
		}

		overflow = package;
		packages.at(type).shrink_to_fit();
	}
}

bool getOpts(const int argc, char* argv[], string& args){
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
		if (i == "-h" || i == "--help"){return false;}
		else if (i == "-q" || i == "--quiet") flags[quiet] = true;
		else if (i == "-y" || i == "--yes" || i == "--assume-yes") flags[yes] = true;
		else if (i == "-s" || i == "--simulate" || i == "--just-print" ||
		         i == "--dry-run" || i == "--recon" || i == "--no-act" || i == "--assume-no")
			flags[sim] = true;

		args += " " + i;
	}

	return true;
}
