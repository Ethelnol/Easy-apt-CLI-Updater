#include <csignal> //for sig_int capture
#include <iostream>
#include <sys/ioctl.h> //for cols

#include "functions.h"
#include "out.h"

bool flags[3] = {false, false, false}; //quiet, yes, sim

uint8_t currentStep = 0;

void sigHandler(const int signal){
	using std::cerr;

	cerr << "\nStopping update: ";

	switch (currentStep){
		case 0:{
			cerr << "Package list retrieval";
			break;
		}
		case 1:{
			cerr << "Package sorting";
			break;
		}
		case 2:{
			cerr << "Updating/Removal";
			break;
		}
		default:{
			cerr << "Unknown process";
			break;
		}
	}

	cerr << " interrupted\n";

	exit(signal);
}

//used to update/install packages
bool update(const array<vector<string>, 5>& packages, const string& args, uint32_t cols);

//remove packages that aren't needed but non disruptive
bool remove(const array<vector<string>, 5>& packages, const string& args, uint32_t cols);

int main(int argc, char* argv[]){
	using std::cout;

	if (signal(SIGINT, sigHandler) == SIG_ERR){
		std::cerr << "Error catching SIGINT" << std::endl;
		exit(1);
	}

	winsize size{};
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

	const uint16_t           cols = size.ws_col;
	string                   args;
	array<vector<string>, 5> packages; //= {UPGRADE, INSTALL, REMOVE, NOW_REMOVE, WITHHELD}

	if (!getOpts(argc, argv, args)){
		outputHelp(cols);
		exit(1);
	}

	cout << "Getting list of updated packages..." << std::flush;
	if (system("sudo apt-get update >/dev/null 2>&1") == 33280){
		sigHandler(SIGINT);
	}
	currentStep++;
	if (!flags[QUIET]){
		cout << " Done";
	}
	cout << '\n';

	cout << "Sorting package information..." << std::flush;
	getPackages(packages);
	checkForImageUpdate(packages);
	currentStep++;
	if (!flags[QUIET]){
		cout << " Done";
	}
	cout << '\n';

	bool changes = false;
	bool acted   = false;
	if (!packages.at(UPGRADE).empty() || !packages.at(INSTALL).empty() || !packages.at(NOW_REMOVE).empty()){
		changes = true;
		if (update(packages, args, cols)){
			acted = true;
		}
	}
	if (!packages.at(REMOVE).empty()){
		changes = true;
		if (remove(packages, args, cols)){
			acted = true;
		}
	}

	cout << "\nUpdate Complete";
	if (!changes){
		cout << ": Nothing to do";
	}
	else if (!acted){
		cout << ": No action taken";
	}
	cout << std::endl;

	return 0;
}

bool update(const array<vector<string>, 5>& packages, const string& args, const uint32_t cols){
	using std::cout;

	bool needComma = false;

	cout << '\n';
	if (!packages.at(UPGRADE).empty()){
		uint32_t size = packages.at(UPGRADE).size();

		string x = std::to_string(size) + ((size > 1)
		                                   ? " packages are upgradable"
		                                   : " package is upgradable");
		cout << x;

		needComma = true;
	}
	if (!packages.at(INSTALL).empty()){
		uint32_t size = packages.at(INSTALL).size();

		if (needComma){
			cout << ", ";
		}

		string x = std::to_string(size) +
		           ((size > 1)
		            ? " packages would be newly installed"
		            : " package would be newly installed");
		cout << x;

		needComma = true;
	}
	if (!packages.at(NOW_REMOVE).empty()){
		uint32_t size = packages.at(NOW_REMOVE).size();

		if (needComma){
			cout << ", ";
		}

		string x =
		std::to_string(size) + ((size > 1)
		                        ? " packages would be removed NOW"
		                        : " package would be removed NOW");

		cout << x;
	}

	const vector<string> text = {"Upgrading:", "Installing:", "", "Removing NOW:", "Withholding:"};

	for (uint8_t i = 0; i < 5; i++){
		if (i == 2 || packages.at(i).empty()){
			continue;
		}

		cout << '\n' << text.at(i) << '\n';
		outputVector(packages.at(i), cols);
	}

	if (flags[SIM]){
		return false;
	}

	char input;
	if (flags[YES]){
		input = 'y';
	}
	else{
		cout << "\nWould you like to update? (y/N) " << std::flush;
		std::cin >> input;
	}

	if (tolower(input) == 'y'){
		cout << std::flush;
		system(string("sudo apt dist-upgrade --yes" + args).c_str());
		return true;
	}

	cout << "Abort.\n";
	return false;
}

bool remove(const array<vector<string>, 5>& packages, const string& args, const uint32_t cols){
	using std::cout;
	string x;
	char   input;

	if (const uint32_t size = packages.at(REMOVE).size(); size > 1){
		x = std::to_string(packages.at(REMOVE).size()) + " packages are removable";
	}
	else{
		x = std::to_string(size) + " package is removable";
	}

	cout << '\n' << x << "\nRemoving:\n";
	outputVector(packages.at(REMOVE), cols);

	if (flags[SIM]){
		return false;
	}

	if (flags[YES]){
		input = 'y';
	}
	else{
		cout << "\nWould you like to remove these packages? (y/N) " << std::flush;
		std::cin >> input;
	}

	if (tolower(input) == 'y'){
		cout << std::flush;
		system(string("sudo apt autoremove --yes" + args).c_str());
		return true;
	}

	cout << "Abort.\n";
	return false;
}
