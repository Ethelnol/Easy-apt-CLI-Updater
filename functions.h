/** 
  * Created by Caitlyn Briggs on 04/10/2025
  **/

#ifndef EASY_CLI_UPDATER_CPP_FUNCTIONS_H
#define EASY_CLI_UPDATER_CPP_FUNCTIONS_H

#include <string>
#include <vector>

using std::string;
using std::vector;

enum{
	UPGRADE, INSTALL, REMOVE, NOW_REMOVE, WITHHELD
};

enum{
	quiet, yes, sim
};

/**
  * Populates vectors under packages with appropriate packages
  * @param packages vector to be populated
  * @post packages are populated
  **/
void getPackages(vector<vector<string>>& packages);

/**
  * Checks for arguments passed when program is executed and assigns to vector
  * @param argc number of parameters
  * @param argv parameters
  * @param args string to be populated with parameters
  * @param quiet bool for if update() should have quieter output
  * @param assumeYes bool for if update() should prompt for user y/N, doesn't prompt if assumeYes is true
  * @param simulate bool for if --simulate, --assume-no, or alternatives are passed.  used to skip y/N prompts if true
  * @pre all args are valid arguments of apt-get(8)
  * @post args is (over)written and relevant flags are set to true
  * @return false if -h or --help is passed as an argument
  **/
bool getOpts(int argc, char* argv[], string& args, bool flags[], size_t cols);

/**
  * Outputs contents of vector cleanly
  * @param vect vector to be output
  * @param cols width of terminal window
  * @pre vect is not empty
  * @post output is sent to stdout
  * @return true if packages could be output cleanly and won't wrap around
  * @return false if packages cannot be output without wrapping around
  **/
bool outputVector(const vector<string>& vect, size_t cols);

#endif //EASY_CLI_UPDATER_CPP_FUNCTIONS_H
