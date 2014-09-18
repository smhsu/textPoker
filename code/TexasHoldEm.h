/*
TexasHoldEm.h
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Declares all necessary functions for making a game of Texas Hold Em.
*/

#ifndef TEXAS_HOLDEM_H
#define TEXAS_HOLDEM_H

#include "Game.h"

class TexasHoldEm : public Game
{
public:
	TexasHoldEm();

	virtual void play();
	virtual int before_turn(Player & p);
	virtual int turn(Player & p);
	virtual int after_turn(Player & p);
	virtual int before_round();
	virtual int round();
	virtual int after_round();

	static const size_t DECK_SIZE = 52;
	static const size_t COMMUNITY_SIZE = 5;
	static const size_t HAND_SIZE = 2;
	static const size_t MAX_PLAYERS = (DECK_SIZE - COMMUNITY_SIZE)/HAND_SIZE;

protected:
	Hand community;

	void printTable();
	void cleanup();
};

#endif
