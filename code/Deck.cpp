/*
Deck.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated November 17, 2013

Implements the Deck class and its ostream insertion operator.
*/

#include "StdAfx.h"
#include "Deck.h"

#include <fstream>
#include <ostream>

#include <string>
#include <algorithm>
#include <set>
#include <time.h>

/*
Constructs an empty deck.  Also reseeds the random number generator 
used for shuffling.
*/
Deck::Deck() : cards(std::deque<Card>())
{
	reseed();
}

/*
Essentially calls the default constructor and then load().
*/
Deck::Deck(const char fileName[]) : cards(std::deque<Card>())
{
	load(fileName);
	reseed();
}

void Deck::add_card(const Card & c)
{
	cards.push_back(c);
}

/*
Adds all the Cards in another Deck to this Deck.
*/
void Deck::add_cards(const Deck & other)
{
	cards.insert(cards.end(), other.cards.begin(), other.cards.end() );
}

/*
Reads valid card definition strings from the specified file and
pushes them back into this Deck's card deque.  

Throws fstream::failure if the file cannot be opened
*/
void Deck::load(const char fileName[])
{
	std::ifstream cardFile;
	cardFile.open(fileName);
	if (!cardFile)
		throw std::fstream::failure("File could not be opened");

	std::string cardStr;
	while (cardFile >> cardStr)
	{
		Card card(cardStr);
		if ( (card.getRank() != BAD_RANK) && (card.getSuit() != BAD_SUIT) )
			cards.push_back(card);
	}

	cardFile.close();
}

/*
Removes all cards from this Deck.
*/
void Deck::clear()
{
	cards.clear();
}

void Deck::shuffle()
{
	int numTimes = rand();
	for (int i = 0; i < numTimes % SHUFFLE_LIMIT; i++)
	{
		std::random_shuffle(cards.begin(), cards.end());
	}
}

int Deck::size() const
{
	return cards.size();
}

/*
Returns true if there are duplicate cards in this deck.
*/
bool Deck::hasDuplicates() const
{
	std::set<Card> seenBefore;
	for (unsigned int i = 0; i < cards.size(); i++)
	{
		if (seenBefore.find(cards[i]) != seenBefore.end()) // Card was found
			return true;
		else
			seenBefore.insert(cards[i]);
	}

	return false;
}

/*
Inserts a string representation of a Deck into a ostream.
It will be in the format "[card_1, card_2, ... , card_n]"
*/
std::ostream & operator<< (std::ostream &out, Deck &deck)
{
	out << '[';
	for (int i = 0; i < (int)deck.cards.size() - 1; i++)
	{
		out << deck.cards[i].toString() << ", ";
	}
	if (deck.cards.size() > 0)
		out << deck.cards.back().toString(); // Last card has no comma or space

	out << ']';

	return out;
}

/*
Re-initializes the pseudo-random number generator used by the shuffle()
*/
void Deck::reseed()
{
	srand((unsigned int)time(0));
}
