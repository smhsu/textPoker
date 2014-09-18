/*
FiveCardDraw.h
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Declares all necessary functions for making a game of five card draw.
*/

#ifndef FIVE_CARD_DRAW_H
#define FIVE_CARD_DRAW_H

#include "Game.h"
#include "Deck.h"

#include <string>

class FiveCardDraw : public Game
{
public:
	FiveCardDraw();

	virtual void play();
	virtual int before_turn(Player & p);
	virtual int turn(Player & p);
	virtual int after_turn(Player & p);
	virtual int before_round();
	virtual int round();
	virtual int after_round();

	static const size_t DECK_SIZE = 52;
	static const size_t MAX_PLAYERS = DECK_SIZE/Hand::POKER_HAND_SIZE;

protected:
	void cleanup();
	
	Deck discard;
};

#endif
