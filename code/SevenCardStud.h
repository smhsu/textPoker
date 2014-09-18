/*
SevenCardStud.h
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Declares all necessary functions for making a game of Seven Card Stud.
*/

#ifndef SEVEN_CARD_STUD_H
#define SEVEN_CARD_STUD_H

#include "Game.h"

class SevenCardStud : public Game
{
public:
	SevenCardStud();

	virtual void play();
	virtual int before_turn(Player & p);
	virtual int turn(Player & p);
	virtual int after_turn(Player & p);
	virtual int before_round();
	virtual int round();
	virtual int after_round();
	
	static bool compStudPlayers(std::pair<Player *, Hand *> one, std::pair<Player *, Hand *> two);

	static const size_t DECK_SIZE = 52;
	static const size_t MAX_PLAYERS = DECK_SIZE/Hand::STUD_HAND_SIZE;

protected:
	void cleanup();
	void printAllHands();
	static const int MIDDLE_TURNS = 3;
};

#endif
