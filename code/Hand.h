/*
Hand.h
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Declares the Hand class, which wraps a std::list of Cards and allows
comparisons and poker ranking comparisons.
*/

#ifndef HAND_H
#define HAND_H

class Deck;
class Hand;

#include "Card.h"

#include <list>

#define STR_RANKS_COUNT 9
#define STR_RANKS {"High card", "One pair", "Two pair", "Three of a kind", "Straight", "Flush", "Full house", "Four of a kind", "Straight flush"}
enum pokerRank
{
	UNKNOWN = -1,
	HIGH_CARD = 0,
	PAIR = 1,
	TWO_PAIR = 2,
	THREE_KIND = 3,
	STRAIGHT = 4,
	FLUSH = 5,
	FULL_HOUSE = 6,
	FOUR_KIND = 7,
	STRAIGHT_FLUSH = 8
};

typedef bool (*pokerRankFxnPtr)(const Hand & h1, const Hand & h2);

class Hand
{
public:
	// Constructors and assignment
	Hand();
	Hand(std::list<Card> list);
	Hand(const Hand & other);
	Hand & operator= (const Hand & other);
	// Default destructor is fine since Hand objects only have static memory (i.e. no calls to "new")
	
	// Modify
	void add_card(Card & c);
	void add_card(Deck & deck, bool isFaceDown = false);
	Card remove_card(size_t n);
	void clear();
	void copyRank(Hand & other);

	// Comparison
	bool operator== (const Hand & other) const;
	bool operator< (const Hand & other) const;

	// Information
	int size() const;
	const Card & operator[] (size_t n);
	std::string toString() const;
	std::string toString_hideFaceDown() const;

	// Ranking
	virtual void calculateRank();
	pokerRank getRank() const;
	std::string getStrRank() const;
	bool sameRankAs(const Hand & other) const;
	Hand * bestStudHand();
	static bool poker_rank(const Hand & h1, const Hand & h2);

	friend Hand & operator<< (Hand & hand, Deck & deck); // FYI: Found at bottom of Hand.cpp

	static const size_t STUD_HAND_SIZE = 7;
	static const size_t POKER_HAND_SIZE = 5;

protected:
	std::list<Card> cards;
	pokerRank rank;

private:
	static bool compInOrder(const Hand & h1, const Hand & h2);
	static bool compInOrder_ignore(const Hand & h1, const Hand & h2, CardRank ignore, int skip);
	static bool compPair(const Hand & h1, const Hand & h2);
	static bool compTwoPair(const Hand & h1, const Hand & h2);
	static bool compThreeKind(const Hand & h1, const Hand & h2);
	static bool compFullHouse(const Hand & h1, const Hand & h2);
	static bool compFourKind(const Hand & h1, const Hand & h2);
	static const pokerRankFxnPtr rankingFxns[9];

	void bestStudHandHelper(std::list<Card> & partialHand, size_t select, std::list<Card>::const_iterator selectStart, Hand * highestHand) const;

	static const int MAX_SAME_RANK = 4; // Means we can't have more than a four of a kind
	static const int STRAIGHT_THRESHOLD = 4; // Pairs of consecutive cards in a row before we consider it a straight
	static const int FLUSH_THRESHOLD = 4; // Pairs of cards with the same suit before we consider it a flush

	// The following are keys for the ranks of pairs, three of a kinds, etc. stored in duplicateRanks
	static const int PAIR_INDEX = 0;
	static const int TRIPLE_INDEX = 1;
	static const int QUAD_INDEX = 2;
	static const int SECOND_PAIR_INDEX = 2;

	CardRank duplicateRanks[MAX_SAME_RANK - 1]; // Saves the ranks of any pairs, three of a kinds, or four of a kinds
};

std::ostream & operator<< (std::ostream &out, Hand & hand);

#endif
