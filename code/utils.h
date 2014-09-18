/*
utils.h
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Misc. useful functions.
*/

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <deque>

//void printCombos(std::deque<int> & before, std::deque<int> & after, size_t select, int selectStart);

/*
Returns true if a string has any characters that don't belong in file names.
*/
const std::string illegalChars = "\\/:?\"<>|*";
bool hasIllegalChar(const std::string & str);

/*
Same as std::string::compare, but case insensitive.
*/
int strComp_ignoreCase(const std::string & one, const std::string & two);

#endif
