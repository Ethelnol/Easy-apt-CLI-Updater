#include <csignal>  //for sig_int capture
#include <cstdint>  //uint8_t
#include <iostream>
#include <sys/ioctl.h>  //for cols

#include "functions.h"

uint8_t currentStep = 0;

void sig_handler(int signal);

//used to update/install packages
bool update(const vector<vector<string>>& packages, const string& args, const bool flags[], size_t cols);

//remove packages that aren't needed but non disruptive
bool remove(const vector<vector<string>>& packages, const string& args, const bool flags[], size_t cols);

int main(int argc, char* argv[]){
	using std::cout, std::endl;

	if (signal(SIGINT, sig_handler) == SIG_ERR){
		std::cerr << "Error catching SIGINT" << endl;
		exit(1);
	}

	size_t cols;
	{
		struct winsize size{};
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

		cols = size.ws_col;
	}
	bool flags[3] = {false, false, false};	//quiet, yes, sim
	string args;
	vector<vector<string>> packages; // = {UPGRADE, INSTALL, REMOVE, NOW_REMOVE, WITHHELD}
	packages.resize(5);

	if (!getOpts(argc, argv, args, flags, cols)) exit(1);

	cout << "Getting list of updated packages..." << std::flush;
	if (system("sudo apt-get update >/dev/null 2>&1")) sig_handler(SIGINT);
	currentStep++;
	if (!flags[quiet]) cout << " Done";
	cout << endl;

	cout << "Sorting package information..." << std::flush;
	getPackages(packages);
	currentStep++;
	if (!flags[quiet]) cout << " Done";
	cout << endl;

	bool changes = false;
	bool acted = false;
	if (!packages.at(UPGRADE).empty() ||
	    !packages.at(INSTALL).empty() ||
	    !packages.at(NOW_REMOVE).empty()){
		changes = true;
		if (update(packages, args, flags, cols)) acted = true;
	}
	if (!packages.at(REMOVE).empty()){
		changes = true;
		if (remove(packages, args, flags, cols)) acted = true;
	}

	cout << endl;
	cout << "Update Complete";
	if (!changes) cout << ": Nothing to do";
	else if (!acted) cout << ": No action taken";
	cout << endl;

	return 0;
}

bool update(const vector<vector<string>>& packages, const string& args, const bool flags[], const size_t cols){
	using std::cout, std::endl;

	bool needComma = false;

	cout << endl;
	if (!packages.at(UPGRADE).empty()){
		size_t size = packages.at(UPGRADE).size();

		cout << size << " package"
		     << ((size > 1) ? "s are" : " is")
		     << " upgradable";

		needComma = true;
	}
	if (!packages.at(INSTALL).empty()){
		size_t size = packages.at(INSTALL).size();

		if (needComma) cout << ", ";

		cout << size << " package"
		     << ((size > 1) ? "s" : "")
		     << " would be newly installed";

		needComma = true;
	}
	if (!packages.at(NOW_REMOVE).empty()){
		size_t size = packages.at(NOW_REMOVE).size();

		if (needComma) cout << ", ";
		cout << size << " package"
		     << ((size > 1) ? "s are" : " is")
		     << " would be removed NOW";
	}
	cout << endl;

	if (!packages.at(UPGRADE).empty()){
		cout << "Upgrading:" << endl;
		outputVector(packages.at(UPGRADE), cols);
	}
	if (!packages.at(INSTALL).empty()){
		cout << "Installing:" << endl;
		outputVector(packages.at(INSTALL), cols);
	}
	if (!packages.at(NOW_REMOVE).empty()){
		cout << "Removing NOW:" << endl;
		outputVector(packages.at(NOW_REMOVE), cols);
	}
	if (!packages.at(WITHHELD).empty()){
		cout << "Withholding:" << endl;
		outputVector(packages.at(WITHHELD), cols);
	}

	if (flags[sim]) return false;

	char input;
	if (flags[yes]) input = 'y';
	else{
		cout << endl;
		cout << "Would you like to update? (y/N) " << std::flush;
		std::cin >> input;
	}

	if (tolower(input) == 'y'){
		string command = "sudo apt dist-upgrade --yes" + args;
		system(command.c_str());
		return true;
	}
	cout << "Abort." << endl;
	return false;
}

bool remove(const vector<vector<string>>& packages, const string& args, const bool flags[], const size_t cols){
	using std::cout, std::endl;
	char input;

	cout << endl;
	{
		size_t size = packages.at(REMOVE).size();

		cout << size << " package"
		     << ((size > 1) ? "s are" : " is")
			 << " removable" << endl;
	}

	cout << "Removing:" << endl;
	outputVector(packages.at(REMOVE), cols);

	if (flags[sim]) return false;

	if (flags[yes]) input = 'y';
	else{
		cout << endl;
		cout << "Would you like to remove these packages? (y/N) " << std::flush;
		std::cin >> input;
	}

	if (tolower(input) == 'y'){
		string command = "sudo apt autoremove --yes" + args;
		system(command.c_str());
		return true;
	}
	cout << "Abort." << endl;
	return false;
}

void sig_handler(int signal){
	using std::cerr, std::endl;

	cerr << endl << "Stopping update: ";

	if (signal == SIGINT){
		switch (currentStep){
			case 0: cerr << "Package list retrieval";
				break;
			case 1: cerr << "Package sorting";
				break;
			case 2: cerr << "Updating/Removal";
				break;
			default: cerr << "Unknown process";
				break;
		}

		cerr << " interrupted" << endl;
	}

	exit(signal);
}
