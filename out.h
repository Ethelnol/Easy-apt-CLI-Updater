/** 
  * Created by Caitlyn Briggs on 03/09/2025
  **/
#ifndef EASY_CLI_UPDATER_CPP_OUT_H
#define EASY_CLI_UPDATER_CPP_OUT_H

#include <cstdint>
#include <string>
#include <vector>

using std::string;
using std::vector;

// getOpts helper function, outputs help information if "-h" or "--help" are passed as arguments
void outputHelp(uint32_t cols);

/**
  * Outputs contents of vect cleanly
  * @param cols width of terminal window
  * @pre vect is not empty
  * @return true if packages could be output cleanly and won't wrap around
  * @return false if packages cannot be output without wrapping around
  **/
bool outputVector(const vector<string>& vect, uint16_t cols);

/**
  * outputVector helper function, returns length of largest package in column
  * @param column column to check
  * @param maxRows maximum number of packages in each column
  * @return length of largest package in column
  **/
vector<string::size_type> getLargestInColumn(const vector<string>& vect, uint16_t maxRows);

/**
  * outputVector helper function, checks if desired number of columns doesn't exceed terminal width
  * @param largestPerCol vector of largest package size in each column
  * @param cols width of terminal window
  **/
bool isValidLayout(const vector<string::size_type>& largestPerCol, uint16_t cols);

/**
  * outputVector helper function, outputs packages in vect cleanly
  * @param vect vector of packages to be output
  * @param largestPerCol vector of largest package length in each column
  * @post output sent to stdout
  * @return false if unclean output
  */
bool vectorToStdout(const vector<string>& vect, const vector<string::size_type>& largestPerCol, uint16_t maxRows);

#endif //EASY_CLI_UPDATER_CPP_OUT_H
