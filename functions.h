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
  * @param args vector to be populated with parameters
  * @param quiet bool for if update() should have quieter output
  * @param assumeYes bool for if update() should prompt for user Y/n, doesn't prompt if assumeYes is true
  * @post args is populated, and quiet and assumeYes are set to true if an argument modifying them was passed
  * @return false if -h or --help is passed as an argument
  **/
bool getOpts(int argc, char* argv[], string& args, bool& quiet, bool& assumeYes, bool& simulate);

/**
  * Outputs contents of vector cleanly
  * @param vect vector to be output
  * @param cols width of terminal window
  * @post output is sent to stdout
  **/
void outputVector(const vector<string>& vect, size_t cols);

/**
  * Converts c to lowercase letter if c is a letter, otherwise returns c
  * @param c char to be checked
  * @return lowercase c or c
  **/
//char toLower(char c);

/**
  * Checks if str1 is alphabetically smaller than str2
  * @param str1 string to be checked
  * @param str2 string to be checked against
  * @return true if str1 is alphabetically smaller than str2
  * @return false if str1 is alphabetically identical or larger than str2
  **/
//bool isSmallerAlphabetically(const string& str1, const string& str2);

//bool upgradePackage(vector<string> upgrade, bool quiet);

/**
  * Gets output of stdout and stderr and returns output as string
  * @param cmd command to be passed to system
  * @return string of stdout and stderr from cmd
  * @author Jeremy Morgan
  * @source www.jeremymorgan.com/tutorials/c-programming/how-to-capture-the-output-of-a-linux-command-in-c
  **/
string GetStdoutFromCommand(string cmd);

#endif //EASY_CLI_UPDATER_CPP_FUNCTIONS_H
