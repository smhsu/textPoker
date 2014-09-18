/*
SevenCardStud.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Defines all necessary functions for making a game of Seven Card Stud.
*/

#include "StdAfx.h"
#include "SevenCardStud.h"
#include "GameException.h"

#include <iostream>
#include <algorithm>

using namespace std;

/*
Creates a new SevenCardStud game that uses a standard 52-card Deck.
*/
SevenCardStud::SevenCardStud() : Game(DECK_SIZE, MAX_PLAYERS)
{
	standardDeck();
	deck.shuffle();
}

/*
Runs the game until all players leave or there is an error.  
Calls stop_game() automatically.
*/
void SevenCardStud::play()
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
Deals an inital hand of two face down cards and one face up card.  

Returns 0 on success.  
Throws GameException if the deck runs out of cards.
*/
int SevenCardStud::before_turn(Player & p)
{
	try
	{
		p.hand.add_card(deck, true);
		p.hand.add_card(deck, true);
		p.hand.add_card(deck, false);
	}
	catch (invalid_argument ia)
	{
		throw GameException("Deck ran out of cards.");
	}

	return 0;
}

/*
Deals one face up card.  

Returns 0 on success.  
Throws GameException if the deck runs out of cards.
*/
int SevenCardStud::turn(Player & p)
{
	if (deck.size())
		p.hand.add_card(deck, false);
	else
		throw GameException("Deck ran out of cards.");

	return 0;
}

/*
Deals one face down card.  

Returns 0 on success.  
Throws GameException if the deck runs out of cards.
*/
int SevenCardStud::after_turn(Player & p)
{
	if (deck.size())
		p.hand.add_card(deck, true);
	else
		throw GameException("Deck ran out of cards.");

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
int SevenCardStud::before_round()
{
	if (players.size() == 0)
		return 0;

	if (players.size() > MAX_PLAYERS)
		throw GameException("Not enough cards to support this number of players.");

	deck.shuffle();

	allJoinRound();
	collectAnte();
	goAround(mem_fun(&Game::before_turn)); // Throws GameException

	printAllHands();
	cout << endl;

	collectBets();
	if (playersInRound == 1)
		return EARLY_WINNER;

	return 0;
}

/*
Deals more cards, with a round of betting after each dealing.  

Returns 0 on success.  
Returns EARLY_WINNER if only one player remains due to folding.  
Throws GameException if the deck runs out of cards.
*/
int SevenCardStud::round()
{
	if (players.size() == 0)
		return 0;

	for (int i = 0; i < MIDDLE_TURNS; i++)
	{
		goAround(mem_fun(&Game::turn)); // Throws GameException 
		printAllHands();
		
		cout << endl;
		collectBets();
		if (playersInRound == 1)
			return EARLY_WINNER;
	}

	goAround(mem_fun(&Game::after_turn)); // Throws GameException
	printAllHands();
	
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
int SevenCardStud::after_round()
{
	if (players.size() == 0)
		return 0;

	vector<pair<Player *, Hand *>> bestHands;
	for (unsigned int i = 0; i < players.size(); i++)
	{ // bestStudHand will throw an exception on hands that are not 7 cards.
		if (players[i]->inRound) // Only players that haven't folded should have 7 cards.
			bestHands.push_back(pair<Player *, Hand *>(players[i], players[i]->hand.bestStudHand()));
		else
			players[i]->losses++;
	}

	sort(bestHands.begin(), bestHands.end(), compStudPlayers);

	vector<Player *> winners;
	winners.push_back(bestHands.front().first);
	for (unsigned int i = 1; i < bestHands.size(); i++)
	{
		if (bestHands.front().second->sameRankAs(*bestHands[i].second))
			winners.push_back(bestHands[i].first);
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

bool SevenCardStud::compStudPlayers(std::pair<Player *, Hand *> one, std::pair<Player *, Hand *> two)
{
	return Hand::poker_rank(*one.second, *two.second);
}

/*
Moves all Players' cards back to the main deck.  Prompts to add and remove Players.  
Increments the dealer position.
*/
void SevenCardStud::cleanup()
{
	allHandsToDeck();
	removePlayersPrompt();
	addPlayersPrompt();

	dealerPos++;
	if (dealerPos >= players.size())
		dealerPos = 0;
}

void SevenCardStud::printAllHands()
{
	cout << "Here's everybody's hands:" << endl;
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (players[i]->inRound)
			cout << players[i]->name << ": " << players[i]->hand.toString_hideFaceDown() << endl;
	}
}
