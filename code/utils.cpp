/*
utils.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Misc. useful functions.
*/
#include "stdafx.h"
#include "utils.h"

// used for printCombos
//#include <iostream>
//#include <iterator>
////

using namespace std;

/*
Scratchwork for bestStudHand()
*/
//inline void printCombos(deque<int> & before, deque<int> & after, size_t select, int selectStart)
//{
//	if (before.size() == select)
//	{
//		copy (before.begin(), before.end(), ostream_iterator<int>(cout, " "));
//		cout << endl;
//		return;
//	}
//
//	for (unsigned int i = selectStart; i <= after.size() - select + before.size(); i++)
//	{
//		before.push_back(after[i]);
//		printCombos(before, after, select, i+1);
//		before.pop_back();
//	}
//}

/*
Returns true if a string has any characters that don't belong in file names.
*/
bool hasIllegalChar(const string & str)
{
	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (illegalChars.find(str[i]) != string::npos)
			return true;
	}

	return false;
}

/*
Same as std::string::compare, but case insensitive.
*/
int strComp_ignoreCase(const string & one, const string & two)
{
	int shortStr;
	if (one.length() > two.length())
		shortStr = two.length();
	else
		shortStr = one.length();

	for (int i = 0; i < shortStr; i++)
	{
		if (toupper(one[i]) > toupper(two[i]))
			return 1;
		if (toupper(one[i]) < toupper(two[i]))
			return -1;
	}
	
	if (one.length() > two.length())
		return 1;
	if (one.length() < two.length())
		return -1;

	return 0;
}
