/*
Card.h
Silas Hsu // hsu.silas@wustl.edu
Last updated December 8, 2013

Declares the Card class, all valid values for its
ranks and suits, and a bunch of utility functions.
*/

#ifndef CARD_H
#define CARD_H

#include <string>

enum CardRank
{
	BAD_RANK = 0,
	LOWEST_RANK = 2,
	TWO = 2,
	THREE = 3,
	FOUR = 4,
	FIVE = 5,
	SIX = 6,
	SEVEN = 7,
	EIGHT = 8,
	NINE = 9,
	TEN = 10,
	JACK = 11,
	QUEEN = 12,
	KING = 13,
	ACE = 14,
	HIGHEST_RANK = 14
};

enum CardSuit
{
	BAD_SUIT = '?',
	CLUBS = 'c',
	DIAMONDS = 'd',
	HEARTS = 'h',
	SPADES = 's'
};

class Card
{
public:
	Card();
    Card(CardRank rank, CardSuit suit);
	Card(std::string raw);
	
	// Information
	CardRank getRank() const;
	CardSuit getSuit() const;
	bool faceDown;
	std::string toString() const;

	// Comparison
	bool operator < (const Card & other) const;
	bool operator > (const Card & other) const;
	bool operator == (const Card & other) const;
	static bool compFaceDownLowest(const Card & one, const Card & two);

	// Information, static
	static CardRank charRankToInt(char rank);
	// isValidRank() relies on the condition that valid ranks are in consecutive order!
	static bool isValidRank(int rank);
	static bool isValidSuit(char suit);


private:
	CardRank rank;
    CardSuit suit;

	// For the constructor that takes a string
	static const int MIN_CARD_STR_LEN = 2;
	static const int MAX_CARD_STR_LEN = 3;
	static const int MIN_NUM_RANK = 2;
	static const int MAX_NUM_RANK = 10;

	// For converting rank, which is stored as int, to chars and vice-versa
	static const char JACK_CHAR = 'J';
	static const char QUEEN_CHAR = 'Q';
	static const char KING_CHAR = 'K';
	static const char ACE_CHAR = 'A';
	static const char BAD_RANK_CHAR = '?';
};

#endif
