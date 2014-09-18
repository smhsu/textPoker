/*
Player.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 8, 2013

Defines Player constructor and Player-related utility functions.
*/

#include "StdAfx.h"
#include "Player.h"

#include <ostream>
#include <fstream>

Player::Player(const char * name)
	: name(name), hand(Hand()), wins(0), losses(0), chips(Player::DEFAULT_CHIPS), amtPaid(0), inRound(true)
{
	std::ifstream playerFile;
	playerFile.open(name);
	if (!playerFile)
		return;

	std::getline(playerFile, this->name);
	playerFile >> this->wins;
	playerFile >> this->losses;
	playerFile >> this->chips;

	playerFile.close();
}

/*
Saves this player's info to a file with the same name as the player.  
Overwrites the contents of the file if it already existed.  

Returns 0 on success.
Returns SAVE_ERR if there was an error opening the file.
*/
int Player::saveToFile()
{
	std::fstream playerFile;
	playerFile.open(name, std::ios_base::out | std::ios_base::trunc);
	if (playerFile)
	{
		playerFile << name << std::endl;
		playerFile << wins << std::endl;
		playerFile << losses << std::endl;
		playerFile << chips << std::endl;

		playerFile.close();
		return 0;
	}
	else
		return SAVE_ERR;
}

/*
Returns true if the first Player has a higher ranking hand than the other.
*/
bool Player::compHands(Player * one, Player * two)
{
	return Hand::poker_rank(one->hand, two->hand);
}

std::ostream & operator<< (std::ostream & os, const Player & p)
{
	os << p.name << " | " <<
		"Wins: " << p.wins << " | " <<
		"Losses: " << p.losses << " | " <<
		"Chips: " << p.chips;

	return os;
}
