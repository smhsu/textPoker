/*
Card.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 8, 2013

Defines all functions in the Card class.
*/

#include "stdafx.h"
#include "Card.h"

#include <string>
#include <sstream>

/*
Constructs a face-up card with BAD_RANK and BAD_SIUIT
*/
Card::Card() : rank(BAD_RANK), suit(BAD_SUIT), faceDown(false) {}

/*
Constructs a face-up card with specified rank and suit
*/
Card::Card(CardRank rank, CardSuit suit)
	: rank(rank), suit(suit), faceDown(false) {}

/*
Constructs a face-up card represented by the input string.
If the string is improperly formatted, then constructs
a card with bad rank or bad suit (or both).
*/
Card::Card(std::string raw)
	: faceDown(false)
{
	if ( (raw.length() < MIN_CARD_STR_LEN) && (raw.length() > MAX_CARD_STR_LEN) )
	{
		this->rank = BAD_RANK; this->suit = BAD_SUIT;
		return;
	}

	std::istringstream stream(raw);
	int intRank = 0;
	char charRank = '\0';
	char suit = '\0';

	//////////
	// RANK //
	//////////
	if (isdigit(raw[0])) // First char a digit?
	{
		stream >> intRank;

		if ( (intRank < MIN_NUM_RANK) || (intRank > MAX_NUM_RANK) )
		{
			this->rank = BAD_RANK; this->suit = BAD_SUIT;
			return;
		}
		if ( (intRank < 10) && (raw.length() != 2) )
		{
			// Single digit: only acceptable length is 2
			this->rank = BAD_RANK; this->suit = BAD_SUIT;
			return;
		}
	}
	else
	{ // Not digit: only acceptable length is 2
		if (raw.length() != 2)
		{
			this->rank = BAD_RANK; this->suit = BAD_SUIT;
			return;
		}

		stream >> charRank;
		intRank = charRankToInt(charRank);
		// intRank may be equal to BAD_RANK
	}

	//////////
	// SUIT //
	//////////
	stream >> suit;
	if ( !Card::isValidSuit(suit) )
		suit = BAD_SUIT;

	this->rank = (CardRank)intRank;
	this->suit = (CardSuit)tolower(suit);
}

CardRank Card::getRank() const
{
	return rank;
}

CardSuit Card::getSuit() const
{
	return suit;
}

/*
Returns a string which represents this Card's
rank and suit data.
*/
std::string Card::toString() const
{
	std::string ans = "";

	// RANK: convert int rank to a string
	int rank = this->rank;
	switch (rank)
	{
		case JACK:
			ans += JACK_CHAR;
			break;
		case QUEEN:
			ans += QUEEN_CHAR;
			break;
		case KING:
			ans += KING_CHAR;
			break;
		case ACE:
			ans += ACE_CHAR;
			break;
		case BAD_RANK:
			ans += BAD_RANK_CHAR;
			break;
		default:
			std::stringstream ss;
			ss << rank;
			ans.append(ss.str());
	}

	// SUIT: stored as char; much easier
	ans += this->suit;

	return ans;
}

bool Card::operator< (const Card & other) const
{
	if (this->rank < other.rank)
	{
		return true;
	}
	// '?' < 'C' < 'D' < 'H' < 'S'
	else if (this->rank == other.rank && this->suit < other.suit)
	{
		return true;
	}
	return false;
}

bool Card::operator> (const Card & other) const
{
	if (this->rank > other.rank)
	{
		return true;
	}
	// 'S' > 'H' > 'D' > 'C' > '?'
	else if (this->rank == other.rank && this->suit > other.suit)
	{
		return true;
	}
	return false;
}

bool Card::operator== (const Card & other) const
{
	return ( (this->suit == other.suit) && (this->rank == other.rank) );
}

/*
Same as operator <, but with face down cards the lowest
*/
bool Card::compFaceDownLowest(const Card & one, const Card & two)
{
	if (one.faceDown != two.faceDown)
	{
		return one.faceDown;
	}

	return (one < two);
}

/*
Returns a rank suitable to construct a Card with
*/
CardRank Card::charRankToInt(char rank)
{
	switch (toupper(rank))
	{
		case JACK_CHAR:
			return JACK;
		case QUEEN_CHAR:
			return QUEEN;
		case KING_CHAR:
			return KING;
		case ACE_CHAR:
			return ACE;
		default:
			return BAD_RANK;
	}
}

/*
This function relies on the condition that valid ranks are in consecutive order!
*/
bool Card::isValidRank(int rank)
{
	return ( (rank >= LOWEST_RANK) && (rank <= HIGHEST_RANK) );
}

bool Card::isValidSuit(char suit)
{
	char lsuit = tolower(suit);
	return (lsuit == CLUBS ||
			lsuit == DIAMONDS ||
			lsuit == HEARTS ||
			lsuit == SPADES);
}
