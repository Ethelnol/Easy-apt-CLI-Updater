/** 
  * Created by Caitlyn Briggs on 10 04 2025
  **/

#ifndef EASY_CLI_UPDATER_CPP_FUNCTIONS_H
#define EASY_CLI_UPDATER_CPP_FUNCTIONS_H

#include <string>
#include <filesystem>
#include <vector>

using namespace std::filesystem;
using std::string;
using std::vector;

enum{
	UPGRADE, INSTALL, REMOVE, NOW_REMOVE, WITHHELD
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
  * @post args is populated, and quiet and assumeYes are set to true if an argument modifying them was passed
  * @return false if -h or --help is passed as an argument
  **/
bool getOpts(int argc, char* argv[], string& args, bool& quiet, bool& assumeYes, bool& simulate);

/**
  * Outputs contents of vector cleanly
  * @param vect vector to be output
  * @param cols width of terminal window
  * @pre vect is not empty
  * @post output is sent to stdout
  **/
void outputVector(const vector<string>& vect, size_t cols);

#endif //EASY_CLI_UPDATER_CPP_FUNCTIONS_H
