/*
TexasHoldEm.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Defines all necessary functions for making a game of Texas Hold Em.
*/

#include "StdAfx.h"
#include "TexasHoldEm.h"
#include "SevenCardStud.h"
#include "GameException.h"
#include "ndebug.h"

#include <iostream>
#include <functional>
#include <algorithm>
#include <assert.h>

using namespace std;

/*
Creates a new SevenCardStud game that uses a standard 52-card Deck.
*/
TexasHoldEm::TexasHoldEm() : Game(DECK_SIZE, MAX_PLAYERS), community(Hand())
{
	standardDeck();
	deck.shuffle();
}

/*
Runs the game until all players leave or there is an error.  
Calls stop_game() automatically.
*/
void TexasHoldEm::play()
{
	addPlayersPrompt();

	try{ while (true) {
		cout << endl;
		if (players.size() == 0)
		{
			cout << "There are no more players in the game.  Stopping..." << endl;
			stop_game();
			break;
		}

		int winner = before_round();
		if (winner)
		{
			earlyWin();
			cleanup();
			continue;
		}

		cout << endl;
		winner = round();
		if (winner)
		{
			earlyWin();
			cleanup();
			continue;
		}

		cout << endl;
		after_round();
	}}
	catch (GameException e)
	{
		cout << "Oh dear - " << e.what() << endl;
		cout << "The game ran into a problem.  Stopping..." << endl;
		stop_game();
	}
}

/*
Deals two face down card to this player.  

Returns 0 on success.  
Throws GameException if the deck runs out of cards.
*/
int TexasHoldEm::before_turn(Player & p)
{
	if (deck.size() < 2)
		throw GameException("Deck ran out of cards.");

	p.hand.add_card(deck, true);
	p.hand.add_card(deck, true);
	return 0;
}

/*
Deals the flop: adds three cards to the community Hand.  
Ignores the player parameter.

Returns 0 on success.  
Throws GameException if the deck runs out of cards.
*/
int TexasHoldEm::turn(Player & p)
{
	if (deck.size() < 3)
		throw GameException("Deck ran out of cards.");

	community.add_card(deck, false);
	community.add_card(deck, false);
	community.add_card(deck, false);
	return 0;
}

/*
Deals the turn or river: adds one card to the community Hand.  
Ignores the player parameter.

Returns 0 on success.  
Throws GameException if the deck runs out of cards.
*/
int TexasHoldEm::after_turn(Player & p)
{
	if (deck.size() == 0)
		throw GameException("Deck ran out of cards.");

	community.add_card(deck, false);
	return 0;
}

/*
Shuffles the deck.  Collects ante.  Deals initial hands.  Prints them.  
Does a round of betting.  

Returns 0 on success.  
Returns EARLY_WINNER if only one player remains due to folding.  
Throws GameException if there are more players than MAX_PLAYERS.  
Throws GameException if the deck runs out of cards.
*/
int TexasHoldEm::before_round()
{
	if (players.size() == 0)
		return 0;

	if (players.size() > MAX_PLAYERS)
		throw GameException("Not enough cards to support this number of players.");

	deck.shuffle();

	allJoinRound();
	collectAnte();
	goAround(std::mem_fun(&Game::before_turn)); // Throws GameException

	printTable();
	cout << endl;

	collectBets();
	if (playersInRound == 1)
		return EARLY_WINNER;

	return 0;
}

/*
Deals the flop, turn, and river.  Performs bets after each.  

Returns 0 on success.  
Returns EARLY_WINNER if only one player remains due to folding.  
Throws GameException if the deck runs out of cards.
*/
int TexasHoldEm::round()
{
	if (players.size() == 0)
		return 0;

	turn(*players[0]); // Flop.  Throws GameException
	printTable();
	cout << endl;
	collectBets();
	if (playersInRound == 1)
		return EARLY_WINNER;

	after_turn(*players[0]); // Turn.  Throws GameException
	printTable();
	cout << endl;
	collectBets();
	if (playersInRound == 1)
		return EARLY_WINNER;

	after_turn(*players[0]); // River.  Throws GameException
	printTable();
	cout << endl;
	collectBets();
	if (playersInRound == 1)
		return EARLY_WINNER;

	return 0;
}

/*
Finds who won the round, and adjusts wins/losses accordingly.  
Prints player stats.  Calls cleanup().  

Returns 0 on success.
*/
int TexasHoldEm::after_round()
{
	if (players.size() == 0)
		return 0;

	vector<pair<Player *, Hand *>> bestHands;
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (players[i]->inRound)
		{
			Hand temp(community);
			Card c = players[i]->hand[0];
			temp.add_card(c);
			c = players[i]->hand[1];
			temp.add_card(c);
			bestHands.push_back(pair<Player *, Hand *>(players[i], temp.bestStudHand()));
		}
		else
			players[i]->losses++;
	}

	sort(bestHands.begin(), bestHands.end(), SevenCardStud::compStudPlayers);

	vector<Player *> winners;
	winners.push_back(bestHands.front().first);
	winners.back()->hand.copyRank(*bestHands.front().second);
	for (unsigned int i = 1; i < bestHands.size(); i++)
	{
		if (bestHands.front().second->sameRankAs(*bestHands[i].second))
		{
			winners.push_back(bestHands[i].first);
			winners.back()->hand.copyRank(*bestHands.front().second);
		}
		else
			break;
	}

	awardWinners(winners);

	for (unsigned int i = winners.size(); i < bestHands.size(); i++)
		bestHands[i].first->losses++;

	for (unsigned int i = 0; i < bestHands.size(); i++)
		delete bestHands[i].second;

	printStatsAndHands();
	cleanup();

	return 0;
}

void TexasHoldEm::printTable()
{
	cout << "Status of the table:" << endl;
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (players[i]->inRound)
			cout << players[i]->name << ": " << players[i]->hand.toString_hideFaceDown() << endl;
	}
	cout << "Community cards: " << community << endl;
}

/*
Moves all cards back to the main deck.  Prompts to add and remove Players.  
Increments the dealer position.
*/
void TexasHoldEm::cleanup()
{
	size_t numComCards = community.size();
	for (unsigned int cardNum = 0; cardNum < numComCards; cardNum++)
	{
		deck.add_card(community.remove_card(0));
	}
	assert(community.size() == 0);

	allHandsToDeck();
	removePlayersPrompt();
	addPlayersPrompt();

	dealerPos++;
	if (dealerPos >= players.size())
		dealerPos = 0;
}
