/*
FiveCardDraw.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Defines all necessary functions for making a game of five card draw.
*/

#include "StdAfx.h"
#include "FiveCardDraw.h"
#include "GameException.h"
#include "utils.h"
#include "ndebug.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <algorithm>
#include <assert.h>

using namespace std;

/*
Creates a new FiveCardDraw game that uses a standard 52-card Deck.
*/
FiveCardDraw::FiveCardDraw()
	: Game(DECK_SIZE, MAX_PLAYERS), discard(Deck())
{
	standardDeck();
	deck.shuffle();
}

/*
Runs the game until all players leave or there is an error.  
Calls stop_game() automatically.
*/
void FiveCardDraw::play()
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
Prompts a Player to select Cards to discard from his/her Hand; 
these Cards will go into the discard Deck.

Returns 0 on success.
*/
int FiveCardDraw::before_turn(Player & p)
{
	cout << p.name << ": " << p.hand << endl;
	set<size_t> toDiscard;
	string s;
	istringstream line;

	cout << "Select cards to discard by typing their position numbers, separated by spaces." << endl;
	while (!line.eof()) // Prompt until we get through a line without failing
	{
		toDiscard.clear();
		getline(cin, s);
		line.str(s);
		unsigned int i = 0;

		while (line.peek() != EOF)
		{
			if (isspace(line.peek()))
			{
				line.get();
				continue;
			}

			line >> i;
			if ( (!line) || (i < 1) || (i > Hand::POKER_HAND_SIZE) )
			{
				cout << "Type numbers between 1 and " << Hand::POKER_HAND_SIZE << " only." << endl;
				line.clear(); // So line.eof() will be false and the loop will continue
				break;
			}
			if (!toDiscard.insert(i).second) // try to insert i, and if already seen before...
			{
				cout << "You have duplicate numbers." << endl;
				line.clear();
				break;
			}
		}
	}

	for (set<size_t>::reverse_iterator iter = toDiscard.rbegin(); iter != toDiscard.rend(); iter++)
	{
		discard.add_card(p.hand[*iter - 1]);
		p.hand.remove_card(*iter - 1);
	}

	return 0;
}

/*
Deal a Player's Hand as many cards as needed to have five Cards in his/her Hand.  

Returns 0 on success.  

Throws GameException if there are not enough Cards in the main or discard Deck.
*/
int FiveCardDraw::turn(Player & p)
{
	for (unsigned int i = p.hand.size(); i < Hand::POKER_HAND_SIZE; i++)
	{
		if (deck.size())
			p.hand << deck;
		else if (discard.size())
		{
			discard.shuffle();
			p.hand << discard;
		}
		else
			throw GameException("Ran out of cards in main and discard decks");
	}
	return 0;
}

/*
Prints the Player's name and Hand's contents.  If they have folded, does not 
print their Hand contents, but an indicator that they have folded.

Returns 0 on success.
*/
int FiveCardDraw::after_turn(Player & p)
{
	if (p.inRound)
		cout << p.name << ": " << p.hand;
	else
		cout << p.name << ": [folded]";

	return 0;
}

/*
Shuffles the deck.  Collects ante.  Then, starting with the Player 
just past the dealer position, deals five-Card Hands to everybody 
in the game.  Performs a round of betting.  Finally, has each Player 
discard cards with before_turn().  

Returns 0 on success.  
Returns WINNER if only one player remains due to folding.  
Throws GameException if there are more players than MAX_PLAYERS.
Throws GameException if the deck runs out of cards.
*/
int FiveCardDraw::before_round()
{
	deck.shuffle();

	if (players.size() == 0)
		return 0;
	if ( players.size() > MAX_PLAYERS )
		throw GameException("Not enough cards to support this number of players.");

	allJoinRound();
	collectAnte();

	try 
	{
	for (int i = 0; i < Hand::POKER_HAND_SIZE; i++) // Deal
	{
		size_t playerNum = dealerPos;
		do 
		{
			playerNum++;
			if (playerNum >= players.size())
				playerNum = 0;
			players[playerNum]->hand << deck;

		} while (playerNum != dealerPos);
	}
	}
	catch (invalid_argument ia)
	{
		throw GameException("Deck ran out of cards.");
	}

	collectBets();
	if (playersInRound == 1)
		return EARLY_WINNER;

	size_t playerNum = dealerPos; // Discard cards
	do 
	{
		playerNum++;
		if (playerNum >= players.size())
			playerNum = 0;

		if (players[playerNum]->inRound)
			before_turn(*(players[playerNum]));

	} while (playerNum != dealerPos);

	return 0;
}

/*
Starting with the Player just past the dealer position, restores five-card hands 
with turn().  Then, does a round of betting.  Finally, prints player hands with 
after_turn().  

Returns 0 on success.  
Returns WINNER if only one player remains due to folding.  
Throws GameException: lets them propagate onward from turn().
*/
int FiveCardDraw::round()
{
	if (players.size() == 0)
		return 0;

	size_t playerNum = dealerPos;
	do
	{
		playerNum++;
		if (playerNum >= players.size())
			playerNum = 0;

		turn(*(players[playerNum])); // Restore five-card hands
	} while (playerNum != dealerPos);

	collectBets();
	if (playersInRound == 1)
		return EARLY_WINNER;

	cout << endl;

	playerNum = dealerPos;
	do
	{
		playerNum++;
		if (playerNum >= players.size())
			playerNum = 0;

		after_turn(*(players[playerNum])); // Print info
		cout << endl;
	} while (playerNum != dealerPos);

	return 0;
}

/*
Finds who won the round, and adjusts wins/losses accordingly.  
Prints player stats.  Calls cleanup().  

Returns 0 on success.
*/
int FiveCardDraw::after_round()
{
	if (players.size() == 0)
		return 0;

	vector<Player *> bestHands; // Get a vector of players who are still playing
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (players[i]->inRound)
		{
			players[i]->hand.calculateRank();
			bestHands.push_back(players[i]);
		}
		else
			players[i]->losses++; // Increment losses for folders
	}

	sort(bestHands.begin(), bestHands.end(), Player::compHands);

	vector<Player *> winners; // From those who haven't folded, get winners
	winners.push_back(bestHands.front());
	for (unsigned int i = 1; i < bestHands.size(); i++)
	{
		if (bestHands.front()->hand.sameRankAs(bestHands[i]->hand))
			winners.push_back(bestHands[i]);
		else
			break;
	}

	awardWinners(winners);

	for (unsigned int i = winners.size(); i < bestHands.size(); i++)
		bestHands[i]->losses++;

	printStatsAndHands();
	cleanup();

	return 0;
}

/*
Moves all cards back to the main deck.  Prompts to remove and add players.  
Increments the dealer position.
*/
void FiveCardDraw::cleanup()
{
	deck.add_cards(discard);
	discard.clear();
	allHandsToDeck();

	removePlayersPrompt();
	addPlayersPrompt();

	dealerPos++;
	if (dealerPos >= players.size())
		dealerPos = 0;
}
