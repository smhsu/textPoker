/*
Deck.h
Silas Hsu // hsu.silas@wustl.edu
Last updated November 30, 2013

Declares the Deck class, a wrapper for a deque of Cards.
Most importantly, Decks have a shuffle function and a 
ostream insertion operator.
*/

#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include "Hand.h"
#include <deque>

class Deck
{
public:
	Deck();
	Deck(const char fileName[]);
	// Default destructor is fine since Deck objects only have static memory (i.e. no calls to "new")

	// Modify, including shuffle
	void add_card(const Card & c);
	void add_cards(const Deck & other);
	void load(const char fileName[]);
	void clear();
	void shuffle();

	// Info
	int size() const;
	bool hasDuplicates() const;

	// Friends
	friend std::ostream & operator<< (std::ostream &out, Deck & deck);
	friend Hand & operator<< (Hand & hand, Deck & deck); // Implemented in Hand.cpp
	friend void Hand::add_card(Deck & deck, bool isFaceDown); // Implemented in Hand.cpp

private:
	void reseed();

	std::deque<Card> cards;
	static const int SHUFFLE_LIMIT = 100;
};

#endif
