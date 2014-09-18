/*
GameException.h
Silas Hsu // hsu.silas@wustl.edu
Last updated November 14, 2013

Self-explanatory.
*/

#ifndef GAME_EXCEPT_H
#define GAME_EXCEPT_H

#include <stdexcept>

class GameException : public std::runtime_error
{
public:
	GameException(std::string message) : std::runtime_error(message) {}
};

#endif
