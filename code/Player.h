/*
Player.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated November 17, 2013

Players are structs that store their names, Hands, wins, and losses.
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "Hand.h"

typedef unsigned long ChipAmt;

struct Player
{
	Player(const char * name);
	int saveToFile();

	std::string name;
	Hand hand;
	unsigned wins;
	unsigned losses;
	ChipAmt chips;
	ChipAmt amtPaid;
	bool inRound;

	static const int SAVE_ERR = 1;
	static const ChipAmt DEFAULT_CHIPS = 20;
	static bool compHands(Player * one, Player * two);
};

std::ostream & operator<< (std::ostream & os, const Player & p);

#endif
