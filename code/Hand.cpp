/*
Hand.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Implements all functions in the Hand class.
*/

#include "stdafx.h"

#include "Hand.h"
#include "Deck.h"
#include "ndebug.h"

#include <assert.h>
#include <algorithm>

//                                            High card,    Pair,      Two pair,     Three of a kind, Straight,    Flush,        Full house,     Four of a kind, Straight flush
const pokerRankFxnPtr Hand::rankingFxns[9] = {&compInOrder, &compPair, &compTwoPair, &compThreeKind, &compInOrder, &compInOrder, &compFullHouse, &compFourKind, &compInOrder};

/*
Constructs an empty Hand.
*/
Hand::Hand() : cards(std::list<Card>()), rank(UNKNOWN) {}

/*
Extends a list of Cards into a Hand.
*/
Hand::Hand(std::list<Card> list)
	: cards(list), rank(UNKNOWN)
{
	cards.sort();
}

Hand::Hand(const Hand & other)
{
	cards = other.cards;
	rank = other.getRank();
	memcpy(this->duplicateRanks, other.duplicateRanks, sizeof(CardRank) * (MAX_SAME_RANK - 1) );
}

Hand & Hand::operator= (const Hand & other)
{
	if (this == &other)
		return *this;

	cards = other.cards;
	rank = other.getRank();
	memcpy(this->duplicateRanks, other.duplicateRanks, sizeof(CardRank) * (MAX_SAME_RANK - 1) );
	return *this;
}

/*
Inserts a Card such that the Cards in the Hand are kept in
sorted order.  Invalidates any rank the Hand may have had.  
*/
void Hand::add_card(Card & c)
{
	std::list<Card>::const_iterator iter = cards.begin();
	while (iter != cards.end() && *iter < c)
	{
		iter++;
	}
	cards.insert(iter, c);

	rank = UNKNOWN;
}

/*
Removes the first Card from the specified Deck and puts it in
the specified Hand such that the Cards in the Hand are kept in
sorted order.  Invalidates any rank the Hand may have had.  

Throws invalid_argument if the Deck is empty.
*/
void Hand::add_card(Deck & deck, bool isfaceDown)
{
	if (deck.cards.size() == 0)
		throw std::invalid_argument("Deck has no more Cards!");

	Card c = deck.cards.front();
	c.faceDown = isfaceDown;
	deck.cards.pop_front();

	std::list<Card>::const_iterator iter = cards.begin();
	while (iter != cards.end() && *iter < c)
	{
		iter++;
	}
	cards.insert(iter, c);

	rank = UNKNOWN;
}

/*
Removes a Card from this hand which is at the specified index.  
Invalidates any rank the Hand may have had.
*/
Card Hand::remove_card(size_t n)
{
	if (n < 0 || n >= cards.size())
		throw std::out_of_range("Index out of bounds");

	int pos = 0;
	for (std::list<Card>::iterator iter = cards.begin(); iter != cards.end(); iter++)
	{
		if (pos == n)
		{
			Card toReturn = *iter;
			cards.erase(iter);
			rank = UNKNOWN;
			return toReturn;
		}

		pos++;
	}

	assert(false);
	return Card();
}

/*
Removes all Cards from this hand.
*/
void Hand::clear()
{
	cards.clear();
}

void Hand::copyRank(Hand & other)
{
	rank = other.rank;
}

/*
Returns true iff two hands are completely identical, including order.
*/
bool Hand::operator== (const Hand & other) const
{
	return (cards == other.cards);
}

/*
Compares the order of the cards in the hands lexicographically.
*/
bool Hand::operator< (const Hand & other) const
{
	std::list<Card>::const_iterator thisIter = cards.begin();
	std::list<Card>::const_iterator otherIter = other.cards.begin();

	if (this->cards.size() < other.cards.size())
	{
		while (thisIter != cards.end())
		{
			if (*thisIter < *otherIter)
				return true;
			else if (*thisIter > *otherIter)
				return false;
			
			thisIter++; otherIter++;
		}

		return true; // The hands are identical, except the other hand has more cards
	}

	else
	{
		while (otherIter != other.cards.end())
		{
			if (*thisIter < *otherIter)
				return true;
			else if (*thisIter > *otherIter)
				return false;
			
			thisIter++; otherIter++;
		}

		return false;
	}

}

/*
Gets how many cards are in this hand.
*/
int Hand::size() const
{
	return cards.size();
}

const Card & Hand::operator[] (size_t n)
{
	if (n < 0 || n >= cards.size())
		throw std::out_of_range("Index out of bounds");

	int pos = 0;
	for (std::list<Card>::iterator iter = cards.begin(); iter != cards.end(); iter++)
	{
		if (pos == n)
		{
			return *iter;
		}
			
		pos++;
	}

	assert(false);
	return cards.front(); // Just something to stop the compiler from complaining.
}

/*
Returns a string representation of all the cards in this Hand.  
It will be in the format "[card_1, card_2, .. , card_n]"
*/
std::string Hand::toString() const
{
	if (cards.size() == 0)
		return "[]";

	std::string ans = "[";
	for (std::list<Card>::const_iterator iter = cards.begin(); iter != --cards.end(); iter++)
	{
		Card c = *iter;
		std::string toAdd = c.toString() + ", ";
		ans += toAdd;
	}

	// Don't add a space for the last card
	Card c = cards.back();
	ans += c.toString();

	ans += ']';

	return ans;
}

/*
Returns a string representation of all the cards in this Hand.  
Face down cards will appear first as '*'.
*/
std::string Hand::toString_hideFaceDown() const
{
	if (cards.size() == 0)
		return "[]";

	std::string ans = "[";
	std::list<Card> copy(cards);
	copy.sort(&Card::compFaceDownLowest);

	std::list<Card>::iterator iter = copy.begin();
	while ((*iter).faceDown && iter != --copy.end())
	{
		ans += "*, ";
		iter++;
	}
	while (iter != --copy.end())
	{
		ans += (*iter).toString() + ", ";
		iter++;
	}

	if (copy.back().faceDown)
		ans += '*';
	else
		ans += copy.back().toString();

	ans += ']';

	return ans;
}

/*
Determines the rank for this Hand if it is the size of a poker hand.  

Complexity: roughly 3n; n == POKER_HAND_SIZE  
Throws: domain_error if this Hand is not the size of a poker hand.
*/
void Hand::calculateRank()
{
	if (cards.size() != POKER_HAND_SIZE)
		throw std::domain_error("Hand is not the size of a poker hand.");

	int duplicateCnts[MAX_SAME_RANK-1] = {0,0,0}; // Tracks # of pairs, three of a kinds, and four of a kinds
	for (int i = 0; i < MAX_SAME_RANK-1; i++)
	{
		duplicateRanks[i] = (CardRank)0; // Zero out duplicateRanks
	}
	int sameRanks = 0; // 1: pair // 2: triple // 3: quad
	int sameSuits = 0;
	int numConsecutive = 0;
	bool isStraight = false;
	bool isFlush = false;

	// Loop works because hand is sorted!
	for (std::list<Card>::const_iterator iter = ++cards.begin(); iter != cards.end(); iter++)
	{
		std::list<Card>::const_iterator prev = iter; prev--;
		// Detect duplicate ranks
		if ((*prev).getRank() == (*iter).getRank())
		{
			sameRanks++;
			continue; // No way could there be a straight or flush now!
		}
		else if (sameRanks > 0)
		{
			assert(sameRanks < MAX_SAME_RANK); // Should be true if there are no bad Cards and no duplicate Cards
			duplicateCnts[sameRanks-1]++; // sameRanks ==1: pair // ==2: triple // ==3: quad

			// duplicateRanks saves the ranks of any pairs, three of a kinds, or four of a kinds
			if (duplicateRanks[PAIR_INDEX] != 0) // Store any second pairs in the four of a kind index
				duplicateRanks[SECOND_PAIR_INDEX] = (*prev).getRank();
			else
				duplicateRanks[sameRanks-1] = (*prev).getRank(); 

			sameRanks = 0;
		}

		// Detect straights
		if  ( ((*prev).getRank() + 1) == (*iter).getRank() )
			numConsecutive++;

		// Detect flushes
		if ( (*prev).getSuit() == (*iter).getSuit() )
			sameSuits++;
	}
	
	if (sameRanks > 0)
	{
		assert(sameRanks < MAX_SAME_RANK); // Should be true if there are no bad Cards and no duplicate Cards
		duplicateCnts[sameRanks-1]++;

		// duplicateRanks saves the ranks of any pairs, three of a kinds, or four of a kinds
		if (duplicateRanks[PAIR_INDEX] != 0) // Store any second pairs in the four of a kind index
			duplicateRanks[QUAD_INDEX] = cards.back().getRank();
		else
			duplicateRanks[sameRanks-1] = cards.back().getRank();
	}
	/*
	Since ACE is 14 and TWO is 2, the following is a special case that the
	straight-checking code in the loop will not catch.  TWO will first
	in the array after sorting; ACE will be last.
	*/
	if ( (cards.front().getRank() == TWO) && (cards.back().getRank() == ACE) )
	{
		numConsecutive++;
	}

	if (numConsecutive >= STRAIGHT_THRESHOLD)
		isStraight = true;
	if (sameSuits >= FLUSH_THRESHOLD)
		isFlush = true;

	// Whew!  Finally done gathering info!
	if (isStraight && isFlush)
		rank = STRAIGHT_FLUSH;
	else if (duplicateCnts[QUAD_INDEX] == 1)
		rank = FOUR_KIND;
	else if ( (duplicateCnts[TRIPLE_INDEX] == 1) && (duplicateCnts[PAIR_INDEX] == 1) )
		rank = FULL_HOUSE;
	else if (isFlush)
		rank = FLUSH;
	else if (isStraight)
		rank = STRAIGHT;
	else if (duplicateCnts[TRIPLE_INDEX] == 1)
		rank = THREE_KIND;
	else if (duplicateCnts[PAIR_INDEX] == 2)
		rank = TWO_PAIR;
	else if (duplicateCnts[PAIR_INDEX] == 1)
		rank = PAIR;
	else
		rank = HIGH_CARD;
}

/*
Get this Hand's rank as an int
*/
pokerRank Hand::getRank() const
{
	return rank;
}

/*
Get this Hand's rank as a string
*/
std::string Hand::getStrRank() const
{
	if (rank == UNKNOWN)
	{
		return "Unknown";
	}

	std::string strRanks[STR_RANKS_COUNT] = STR_RANKS;
	return strRanks[rank];
}

bool Hand::sameRankAs(const Hand & other) const
{
	if (cards.size() != other.cards.size())
		return false;
	
	if (rank != other.rank)
		return false;

	std::list<Card>::const_iterator thisIter = cards.begin();
	std::list<Card>::const_iterator otherIter = other.cards.begin();
	while (thisIter != cards.end())
	{
		if ((*thisIter).getRank() != (*otherIter).getRank())
			return false;

		thisIter++;
		otherIter++;
	}

	return true;
}

/*
Chooses the five cards out of this StudHand that make the best rank.  
Creates a new Hand on the heap and returns it.

Throws domain_error if this StudHand is not STUD_HAND_SIZE cards.
*/
Hand * Hand::bestStudHand()
{
	if (cards.size() != STUD_HAND_SIZE)
		throw std::domain_error("Hand is not the size of a stud hand.");

	Hand * highestHand = new Hand(cards);
	highestHand->cards.pop_back();
	highestHand->cards.pop_back();
	highestHand->calculateRank();
	bestStudHandHelper(std::list<Card>(), Hand::POKER_HAND_SIZE, cards.begin(), highestHand);

	rank = highestHand->rank;
	return highestHand;
}

/*
Returns true if h1 has a higher poker rank than the other.

Throws invalid_argument if either of the hands are unranked.
*/
bool Hand::poker_rank(const Hand & h1, const Hand & h2)
{
	if (h1.rank == UNKNOWN || h2.rank == UNKNOWN)
		throw std::invalid_argument("One or both hands unranked");

	if (h1.rank > h2.rank)
	{
		return true;
	}
	else if (h1.rank < h2.rank)
	{
		return false;
	}

	assert(h1.size() == Hand::POKER_HAND_SIZE && h2.size() == Hand::POKER_HAND_SIZE);

	return Hand::rankingFxns[h1.rank](h1, h2);
}

/////////////////////////////////////////////////////////
// poker_rank() may call any of the ranking functions  //
// below, and therefore all assume the hands are of    //
// the same poker rank, size 5, and sorted.            //
/////////////////////////////////////////////////////////

/*
Compare the cards in each hand from highest to lowest.
*/
bool Hand::compInOrder(const Hand & h1, const Hand & h2)
{
	std::list<Card>::const_reverse_iterator h1Iter = h1.cards.rbegin();
	std::list<Card>::const_reverse_iterator h2Iter = h2.cards.rbegin();

	while (h1Iter != h1.cards.rend())
	{
		if ((*h1Iter).getRank() > (*h2Iter).getRank())
			return true;
		else if ((*h1Iter).getRank() < (*h2Iter).getRank())
			return false;

		h1Iter++;
		h2Iter++;
	}

	return false;
}

/*
Compare the cards in each hand from highest to lowest, skipping 
"skip" comparisons when a rank of "ignore" is encountered.
*/
bool Hand::compInOrder_ignore(const Hand & h1, const Hand & h2, CardRank ignore, int skip)
{
	std::list<Card>::const_reverse_iterator h1Iter = h1.cards.rbegin();
	std::list<Card>::const_reverse_iterator h2Iter = h2.cards.rbegin();

	while (h1Iter != h1.cards.rend())
	{
		if ((*h1Iter).getRank() == ignore)
		{
			std::advance(h1Iter, skip);
			if (h1Iter == h1.cards.rend())
				break;
		}
		if ((*h2Iter).getRank() == ignore)
		{
			std::advance(h2Iter, skip);
			if (h2Iter == h2.cards.rend())
				break;
		}

		if ((*h1Iter).getRank() > (*h2Iter).getRank())
			return true;
		else if ((*h1Iter).getRank() < (*h2Iter).getRank())
			return false;

		h1Iter++;
		h2Iter++;
	}

	return false;
}

bool Hand::compPair(const Hand & h1, const Hand & h2)
{
	CardRank h1PairRank = h1.duplicateRanks[Hand::PAIR_INDEX];
	CardRank h2PairRank = h2.duplicateRanks[Hand::PAIR_INDEX];
	if (h1PairRank > h2PairRank)
		return true;
	else if (h1PairRank < h2PairRank)
		return false;
	else
		return compInOrder_ignore(h1, h2, h1PairRank, 2);
}

bool Hand::compTwoPair(const Hand & h1, const Hand & h2)
{
	CardRank hiPairh1 = h1.duplicateRanks[Hand::PAIR_INDEX];
	CardRank loPairh1 = h1.duplicateRanks[Hand::SECOND_PAIR_INDEX];
	if (loPairh1 > hiPairh1)
		std::swap(loPairh1,hiPairh1);

	CardRank hiPairh2 = h2.duplicateRanks[Hand::PAIR_INDEX];
	CardRank loPairh2 = h2.duplicateRanks[Hand::SECOND_PAIR_INDEX];
	if (loPairh2 > hiPairh2)
		std::swap(loPairh2,hiPairh2);

	if (hiPairh1 > hiPairh2)
		return true;
	else if (hiPairh1 < hiPairh2)
		return false;

	// The high pair is the same...
	if (loPairh1 > loPairh2)
		return true;
	else if (loPairh1 < loPairh2)
		return false;

	// Both pairs the same...
	CardRank kickerH1 = BAD_RANK;
	CardRank kickerH2 = BAD_RANK;
	for (std::list<Card>::const_iterator iter = h1.cards.begin(); iter != h1.cards.end(); iter++)
	{
		if ((*iter).getRank() != hiPairh1 && (*iter).getRank() != loPairh1)
		{
			kickerH1 = (*iter).getRank();
			break;
		}
	}
	for (std::list<Card>::const_iterator iter = h2.cards.begin(); iter != h2.cards.end(); iter++)
	{
		if ((*iter).getRank() != hiPairh2 && (*iter).getRank() != loPairh2)
		{
			kickerH2 = (*iter).getRank();
			break;
		}
	}

	if (kickerH1 > kickerH2)
		return true;
	else
		return false;
}

bool Hand::compThreeKind(const Hand & h1, const Hand & h2)
{
	CardRank h1ThreeRank = h1.duplicateRanks[Hand::TRIPLE_INDEX];
	CardRank h2ThreeRank = h2.duplicateRanks[Hand::TRIPLE_INDEX];
	if (h1ThreeRank > h2ThreeRank)
		return true;
	else if (h1ThreeRank < h2ThreeRank)
		return false;
	else
		return compInOrder_ignore(h1, h2, h1ThreeRank, 3);
}

bool Hand::compFullHouse(const Hand & h1, const Hand & h2)
{
	CardRank h1ThreeRank = h1.duplicateRanks[Hand::TRIPLE_INDEX];
	CardRank h2ThreeRank = h2.duplicateRanks[Hand::TRIPLE_INDEX];
	if (h1ThreeRank > h2ThreeRank)
		return true;
	else if (h1ThreeRank < h2ThreeRank)
		return false;

	// The three of a kind is the same for both hands...
	if (h1.duplicateRanks[Hand::PAIR_INDEX] > h2.duplicateRanks[Hand::PAIR_INDEX])
		return true;
	else
		return false;
}

bool Hand::compFourKind(const Hand & h1, const Hand & h2)
{
	CardRank h1QuadRank = h1.duplicateRanks[Hand::QUAD_INDEX];
	CardRank h2QuadRank = h2.duplicateRanks[Hand::QUAD_INDEX];
	if (h1QuadRank > h2QuadRank)
		return true;
	else if (h1QuadRank < h2QuadRank)
		return false;
	else
		return compInOrder_ignore(h1, h2, h1QuadRank, 4);
}

/*
Gets all 5-card combinations out of this Hand and stores the Hand with 
the highest poker rank in highestHand.
*/
void Hand::bestStudHandHelper(std::list<Card> & partialHand, size_t select, std::list<Card>::const_iterator selectStart, Hand * highestHand) const
{
	if (partialHand.size() == select)
	{
		Hand candidate = Hand(partialHand);
		candidate.calculateRank();
		if (Hand::poker_rank(candidate, *highestHand))
		{
			*highestHand = candidate;
		}

		return;
	}
	
	// Invariant 1: the card at selectStart is always found after the last card of partialHand.
	// Invariant 2: there will always be enough cards after selectStart to complete partialHand.
	std::list<Card>::const_iterator end = cards.end();
	std::advance(end, (int)partialHand.size() - (int)select + 1);
	while (selectStart != end)
	{
		partialHand.push_back(*selectStart);
		selectStart++;
		bestStudHandHelper(partialHand, select, selectStart, highestHand);
		partialHand.pop_back();
	}
}


/*
Uses add_card; the card will be face up.
*/
Hand & operator<< (Hand & hand, Deck & deck)
{
	hand.add_card(deck, false);
	return hand;
}

/*
Inserts a string representation of all the cards in this Hand into an ostream.  
It will be in the format "[card_1, card_2, .. , card_n]"
*/
std::ostream & operator<< (std::ostream &out, Hand & hand)
{
	out << hand.toString();
	return out;
}
