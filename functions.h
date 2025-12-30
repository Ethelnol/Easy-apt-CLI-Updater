/* 
 * Created by Caitlyn Briggs on 04/10/2025
 */

#ifndef EASY_CLI_UPDATER_CPP_FUNCTIONS_H
#define EASY_CLI_UPDATER_CPP_FUNCTIONS_H

#include <array>
#include <string>
#include <vector>

using std::array;
using std::string;
using std::vector;

enum pkgType{
	UPGRADE, INSTALL, REMOVE, NOW_REMOVE, WITHHELD
};

enum flag{
	QUIET, YES, SIM
};

/*
 * Populates vectors under packages with appropriate packages
 * @param packages vector to be populated
 * @post packages are populated
 */
void getPackages(array<vector<string>, 5>& packages);

/*
 * Checks packages[INSTALL] for packages beginning with "linux"
 * If a match is found, getPackages() is called again to account for old images
 */
void checkForImageUpdate(array<vector<string>, 5>& packages);

/*
 * Checks for arguments passed when program is executed and assigns to vector
 * @param argc number of parameters
 * @param argv parameters
 * @param args string to be populated with parameters
 * @pre all args are valid arguments of apt-get(8)
 * @post args is (over)written and relevant flags are set to true
 * @return false if -h or --help is passed as an argument
 */
bool getOpts(int argc, char* argv[], string& args);

#endif //EASY_CLI_UPDATER_CPP_FUNCTIONS_H
