#include <csignal> //for sig_int capture
#include <cstdio>  //for sig_int capture
#include <iostream>
#include <string>
#include <sys/ioctl.h> //for rows and cols

#include "functions.h"

size_t currentStep = 0;

void sig_handler(int signal);

int main(int argc, char* argv[]){
	using std::cout, std::flush, std::endl;

	if (signal(SIGINT, sig_handler) == SIG_ERR){
		std::cerr << "Error catching SIGINT" << endl;
		exit(1);
	}

	size_t cols;
	bool quiet = false;
	bool assumeYes = false;
	bool simulate = false;
	string args;
	vector<vector<string>> packages; // = {UPGRADE, INSTALL, REMOVE, NOW_REMOVE, WITHHELD}
	packages.resize(5);

	{
		struct winsize size{};
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

		cols = size.ws_col;
	}

	if (!getOpts(argc, argv, args, quiet, assumeYes, simulate)) exit(1);

	cout << "Getting list of updated packages..." << flush;
	system("sudo apt-get update >/dev/null 2>&1");
	currentStep++;
	if (!quiet) cout << " Done" << flush;
	cout << endl;

	cout << "Sorting package information..." << flush;
	getPackages(packages);
	currentStep++;
	if (!quiet) cout << " Done" << flush;
	cout << endl;

	bool changes = false;
	bool add = false;
	bool remove = false;
	if (!packages.at(UPGRADE).empty() ||
	    !packages.at(INSTALL).empty() ||
	    !packages.at(NOW_REMOVE).empty())
		add = true;
	if (!packages.at(REMOVE).empty())
		remove = true;

	char input = '\0';

	if (add){
		bool needComma = false;

		if (!packages.at(UPGRADE).empty()){
			size_t size = packages.at(UPGRADE).size();

			cout << size << " package" << flush;
			if (size > 1)
				cout << "s are" << flush;
			else
				cout << " is" << flush;
			cout << " upgradable" << flush;

			needComma = true;
		}
		if (!packages.at(INSTALL).empty()){
			size_t size = packages.at(INSTALL).size();

			if (needComma) cout << ", " << flush;
			cout << size << " package" << flush;
			if (size > 1)
				cout << "s" << flush;
			cout << " will be newly installed" << flush;

			needComma = true;
		}
		if (!packages.at(NOW_REMOVE).empty()){
			size_t size = packages.at(NOW_REMOVE).size();

			if (needComma) cout << ", " << flush;
			cout << size << " package" << flush;
			if (size > 1)
				cout << "s" << flush;
			cout << " will be removed NOW" << flush;

			needComma = true;
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
		if (!packages.at(WITHHELD).empty()){
			cout << "Ignoring:" << endl;
			outputVector(packages.at(WITHHELD), cols);
		}
		if (!packages.at(NOW_REMOVE).empty()){
			cout << "Removing NOW:" << endl;
			outputVector(packages.at(NOW_REMOVE), cols);
		}

		if (!simulate){

			if (assumeYes) input = 'y';
			else{
				cout << endl;
				cout << "Would you like to update? (y/N) " << flush;
				std::cin >> input;
			}

			if (tolower(input) == 'y'){
				string command = "sudo apt dist-upgrade --yes" + args;
				system(command.c_str());
				changes = true;
			}
			else cout << "Abort." << endl;
		}
	}

	if (remove){
		input = '\0';

		{
			size_t size = packages.at(REMOVE).size();

			cout << packages.at(REMOVE).size() << " package" << flush;
			if (size > 1) cout << "s";
			cout << " will be removed" << endl;
		}

		cout << "Removing:" << endl;
		outputVector(packages.at(REMOVE), cols);

		if (!simulate){

			if (assumeYes) input = 'y';
			else{
				cout << endl;
				cout << "Would you like to remove these packages? (y/N) " << flush;
				std::cin >> input;
			}

			if (tolower(input) == 'y'){
				string command = "sudo apt autoremove --yes" + args;
				system(command.c_str());
				changes = true;
			}
			else cout << "Abort." << endl;
		}
	}

	cout << endl;
	cout << "Update Complete" << flush;
	if (!add && !remove) cout << ": Nothing to do" << flush;
	else if (!changes) cout << ": No action taken" << flush;
	cout << endl;

	return 0;
}

void sig_handler(int signal){
	using std::cerr, std::endl, std::flush;

	if (signal == SIGINT){
		cerr << endl << "Stopping update: " << flush;
		switch (currentStep){
			case 0: cerr << "Package list retrieval" << flush;
				break;
			case 1: cerr << "Package sorting" << flush;
				break;
			case 2: cerr << "Updating/Removal process" << flush;
				break;
			default:break;
		}

		cerr << " interrupted" << endl;
		exit(1);
	}
}
