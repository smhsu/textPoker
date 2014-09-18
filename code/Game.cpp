/*
Game.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Gives concrete definitions for everything in Game that is not pure virtual.
*/

#include "StdAfx.h"
#include "Game.h"
#include "GameException.h"
#include "FiveCardDraw.h"
#include "SevenCardStud.h"
#include "TexasHoldEm.h"
#include "utils.h"
#include "ndebug.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <conio.h>
#include <assert.h>

using namespace std;

Game * Game::gameInstance = 0;

/*
Returns the Game that's currently running.  

Throws GameException if there is no game running
*/
Game * Game::instance()
{
	if (!gameInstance)
		throw GameException("Instance not available - use start_game() to get one");

	return gameInstance;
}

/*
Removes all players from this Game.
*/
Game::~Game()
{
	for (unsigned int i = 0; i < players.size(); i++)
	{
		delete players[i];
	}
}

/*
Starts a Game with the specifed name.  

Throws GameException if there is no such Game, or if another 
game is already in progess.
*/
void Game::start_game(const std::string & name)
{
	if (gameInstance)
		throw GameException("A game is already running");

	if (name.find("FiveCardDraw") != std::string::npos) // Found a valid substring
	{
		gameInstance = new FiveCardDraw();
		cout << "Starting a game of Five Card Draw..." << endl;
		return;
	}
	else if (name.find("SevenCardStud") != std::string::npos)
	{
		gameInstance = new SevenCardStud();
		cout << "Starting a game of Seven Card Stud..." << endl;
		return;
	}
	else if (name.find("TexasHoldEm") != std::string::npos)
	{
		gameInstance = new TexasHoldEm();
		cout << "Starting a game of Texas Hold 'Em..." << endl;
		return;
	}

	throw GameException("Unknown game");
}

/*
Stops (deletes) the currently running Game.  

Throws GameException if no Game is running.
*/
void Game::stop_game()
{
	if (!gameInstance)
		throw GameException("No game in progress");

	delete gameInstance;
	gameInstance = 0;
}

unsigned int Game::getNumPlayers() const
{
	return players.size();
}

/*
Adds a Player with the specified name to the Game.  If the Player 
has 0 chips, prompts to either reset chips or leave.

Throws GameException if the Player is already in the Game or if the name 
contains illegal characters.

Complexity: n
*/
void Game::add_player(const std::string & name)
{
	if (hasIllegalChar(name))
		throw GameException("Names may not contain the characters " + illegalChars);
	
	Player * p = find_player(name);

	if (p)
		throw GameException(p->name + " is already playing!");
	
	p = new Player(name.c_str());
	bool staying = true;
	if (p->chips == 0)
		staying = handle0Chips(p);

	if (staying)
	{
		players.push_back(p);
		cout << p->name << " joined the game." << endl;
	}
	else
		delete p;
}

void Game::add_player(Player * const p)
{
	if (hasIllegalChar(p->name))
		throw GameException("Names may not contain the characters " + illegalChars);

	if (find_player(p->name))
		throw GameException(p->name + " is already playing");

	bool staying = true;
	if (p->chips == 0)
		staying = handle0Chips(p);

	if (staying)
	{
		players.push_back(p);
		cout << p->name << " joined the game." << endl;
	}
}

/*
Removes a Player with the specified name from the game.  
If the Player does not exist, does nothing.
Also tries to save Player information to a file with the same name as the Player.
*/
void Game::remove_player(const std::string & name)
{
	Player * p = find_player(name);

	if (p)
	{
		int error = p->saveToFile();
		if (error)
		{
			cout << "WARNING: could not save " << name << "'s info." << endl;
		}

		cout << p->name << " left the game." << endl;
		delete p;
		players.erase(std::find(players.begin(),players.end(),p));
	}
}

/*
Removes the nth Player from the Game.  
Throws out_of_range if there is no such Player.
*/
void Game::remove_player(size_t n)
{
	Player * leaving = players[n]; // Throws out_of_range
	int error = leaving->saveToFile();
	if (error)
	{
		cout << "WARNING: could not save " << leaving->name << "'s info." << endl;
	}

	cout << leaving->name << " left the game." << endl;
	delete leaving;
	players.erase(players.begin()+n);
}

/*
Finds a Player with the specified name in the Game (case insensitive).  Returns 
a null pointer if there is no such player.  

Complexity: n
*/
Player * Game::find_player(const std::string & find) const
{
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (strComp_ignoreCase(players[i]->name, find) == 0)
			return players[i];
	}

	return 0;
}

/*
Deducts one chip from each Player, adding them to the pot.
*/
void Game::collectAnte()
{
	for (unsigned int i = 0; i < players.size(); i++)
	{
		assert(players[i]->chips > 0);
		players[i]->chips--;
	}
	pot += players.size();
}

/*
Performs a round of betting.
*/
void Game::collectBets()
{
	if (players.size() == 0)
		return;

	size_t finalResponder = dealerPos;
	size_t playerNum = dealerPos;
	bool betMade = false;
	ChipAmt bet = 0;

	do
	{
		playerNum++;
		if (playerNum >= players.size())
			playerNum = 0;

		Player * p = players[playerNum];
		if (!p->inRound || p->chips == 0)
			continue;

		if (betMade)
		{
			char choice = callRaiseFoldPrompt(p, bet - p->amtPaid);
			switch (choice)
			{
			case CALL:
				handleCall(p, bet);
				break;

			case RAISE: // callRaiseFold() allows raise iff player has enough
				handleCall(p, bet);
				bet += handleBet(p);

				if (playerNum == 0) // Set finalResponder to be the player immediately before
					finalResponder = players.size() - 1;
				else
					finalResponder = playerNum - 1;
				break;

			case FOLD:
				p->inRound = false;
				playersInRound--;
				break;
			}
		}
		else
		{
			char choice = checkBetPrompt(p);
			if (choice == BET)
			{
				betMade = true;
				bet = handleBet(p);

				if (playerNum == 0)// Set finalResponder to be the player immediately before
					finalResponder = players.size() - 1;
				else
					finalResponder = playerNum - 1;
			}
			// Check: do nothing
		}

	} while (playerNum != finalResponder);

	for (size_t i = 0; i < players.size(); i++)
		players[i]->amtPaid = 0;
}

/*
If everybody except one has folded, announces the winner and transfers the pot.  
Increments everyone else's losses.

Throws logic_error if there is not only one player in round.
*/
void Game::earlyWin()
{
	if (players.size() == 0)
		return;

	if (playersInRound != 1)
		throw logic_error("Trying to determine early winner when number playersInRound is not 1");

	unsigned int winnerNum = 0; // Find winner.  Increments losses.
	bool winnerFound = false;
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (players[i]->inRound)
		{
			if (winnerFound)
				throw logic_error("Found more than one player still in round");
			winnerNum = i;
			winnerFound = true;
		}
		else
			players[i]->losses++;
	}

	Player * winner = players[winnerNum];
	assert(winner->inRound);
	cout << winner->name << " wins pot of " << pot << "!" << endl;
	winner->wins++;
	winner->chips += pot;
	pot = 0;
}

/*
Awards all players in the vector with the pot and increments their win count
*/
void Game::awardWinners(vector<Player *> & winners)
{
	if (winners.size() == 0)
		return;

	if (winners.size() == 1)
	{
		cout << winners[0]->name << " wins pot of " << pot << " with a " << winners[0]->hand.getStrRank() << "!" << endl;
		winners[0]->wins++;
		winners[0]->chips += pot;
		pot = 0;
	}
	else // Tie
	{
		for (unsigned int i = 0; i < winners.size() - 1; i++)
		{
			cout << winners[i]->name << " and ";
			winners[i]->wins++;
		}
		cout << winners.back()->name;
		winners.back()->wins++;
		cout << " tie with a " << winners[0]->hand.getStrRank() << "!" << endl;
		cout << "They split a pot of " << pot << '.' << endl;
		dividePot(winners.begin(), winners.end());
	}
}

/*
Divide the pot as evenly as possible among a range of Players.
*/
void Game::dividePot(vector<Player *>::iterator beg, vector<Player *>::iterator end)
{
	unsigned int numPlayers = distance(beg, end);
	if (numPlayers == 0)
		return;

	ChipAmt share1 = pot / numPlayers;
	ChipAmt share2 = pot % numPlayers;

	vector<Player *>::iterator begCopy(beg);
	for (ChipAmt i = 0; i < share2; i++) // Distribute remainder
	{
		(*begCopy)->chips++;
		begCopy++;
	}

	while (beg != end) // Distribute primary share
	{
		(*beg)->chips += share1;
		beg++;
	}

	pot = 0;
}

/*
Prints player stats and non-folded hands.
*/
void Game::printStatsAndHands()
{
	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (players[i]->inRound)
			cout << *players[i] << " | " << players[i]->hand << endl;
		else
			cout << *players[i] << " | " << "[folded]" << endl;
	}
}

/*
Removes all players that the user wishes to remove.  Then, prompts all Players with 
0 chips to either reset chips or leave.
*/
void Game::removePlayersPrompt()
{
	if (players.size() == 0)
		return;

	cout << "Do any players wish to leave the game?  Enter a name, or nothing if not: ";
	string name;
	getline(cin, name);
	while (name.length() > 0)
	{
		remove_player(name);
		if (players.size() == 0)
			return;

		cout << "Any more leavers?  Enter a name, or nothing if not: ";
		getline(cin, name);
	}

	for (unsigned int i = 0; i < players.size(); i++)
	{
		if (players[i]->chips == 0 && !handle0Chips(players[i]))
			remove_player(i);
	}
}

/*
Adds all players the user wishes to add, up to a maximum of MAX_PLAYERS.
*/
void Game::addPlayersPrompt()
{
	if (players.size() >= MAX_PLAYERS)
		return;

	cout << "Do any players wish to join the game?  Enter a name, or nothing if not: ";
	string name;
	getline(cin, name);
	while (name.length() > 0)
	{
		try { add_player(name); }
		catch (GameException e)
		{
			cout << "Did not add player: " << e.what() << endl;
		}

		if (players.size() == MAX_PLAYERS)
		{
			cout << "The game is now full with " << MAX_PLAYERS << " players." << endl;
			break;
		}

		cout << "Any more joiners?  Enter a name, or nothing if not: ";
		getline(cin, name);
	}
}

/*
Move any cards that may be in Players' hands to the deck.
*/
void Game::allHandsToDeck()
{
	for (unsigned int i = 0; i < players.size(); i++)
	{
		size_t numCards = players[i]->hand.size();
		for (unsigned int cardNum = 0; cardNum < numCards; cardNum++)
		{
			deck.add_card(players[i]->hand.remove_card(0));
		}
		assert(players[i]->hand.size() == 0);
	}
	assert(deck.size() == DECK_SIZE);
}

/*
Game default constructor.  Copies deckSize and maxPlayers, initializes an empty Deck 
and vector of Player pointers, and zeros everything else.  If maxPlayers is 0, 
there is no limit.
*/
Game::Game(size_t deckSize, size_t maxPlayers)
	: deck(Deck()), players(std::vector<Player *>()), playersInRound(0), dealerPos(0),
	pot(0), DECK_SIZE(deckSize), MAX_PLAYERS( (maxPlayers == 0 ? -1 : maxPlayers) ) {}

/*
Replaces the deck with a standard 52-card deck.
*/
void Game::standardDeck()
{
	for (int rank = LOWEST_RANK; rank <= HIGHEST_RANK; rank++)
	{
		deck.add_card( Card((CardRank)rank, CLUBS) );
		deck.add_card( Card((CardRank)rank, DIAMONDS) );
		deck.add_card( Card((CardRank)rank, SPADES) );
		deck.add_card( Card((CardRank)rank, HEARTS) );
	}
}

/*
Has all players be in the round (i.e. not folded).
*/
void Game::allJoinRound()
{
	playersInRound = players.size();
	for (size_t i = 0; i < players.size(); i++)
	{
		players[i]->inRound = true;
	}
}

/*
Starting with the player after the dealer position, performs 
doWhat on each player.  If includeFolded, will also do it to players 
that have folded.  

Returns 0 on success.  Will stop and return doWhat's return value 
if it is not 0.
*/
int Game::goAround(mem_fun1_t<int, Game, Player &> doWhat, bool includeFolded)
{
	if (players.size() == 0)
		return 0;

	size_t playerNum = dealerPos;
	do
	{
		playerNum++;
		if (playerNum >= players.size())
			playerNum = 0;

		if (!includeFolded && !players[playerNum]->inRound)
			continue;

		int error = doWhat(this, *(players[playerNum]));
		if (error)
			return error;

	} while (playerNum != dealerPos);

	return 0;
}

/*
Prints a Player's hand and chip balance, then prompts to either check or bet.
*/
char Game::checkBetPrompt(Player * p)
{
	char ans = '\0';
	cout << p->name << ": " << p->hand << ", " << p->chips << " chips" << endl;
	do
	{
		cout << "Check (" << (char)CHECK << ") or Bet (" << (char)BET << ")? ";
		ans = toupper(_getch());
		cout << ans << endl;
	} while (ans != CALL && ans != BET);

	return ans;
}

/*
Prints a Player's hand and chip balance.  Then, prompts to call, raise, or fold.  
"callAmt" is the cost in chips to call.  

If the Player does not have enough chips to raise, he is given
no such option; therefore, a return value of 'r' guarantees the player has 
enough chips to raise.
*/
char Game::callRaiseFoldPrompt(Player * p, ChipAmt callAmt)
{
	char ans = '\0';
	cout << p->name << ": " << p->hand << ", " << p->chips << " chips" << endl;

	if (p->chips > callAmt)
	{
		do
		{
			cout << "Call " << callAmt << " (" << (char)CALL << "), Raise (" << (char)RAISE << "), or Fold (" << (char)FOLD << ")? ";
			ans = toupper(_getch());
			cout << ans << endl;
		} while (ans != CALL && ans != RAISE && ans != FOLD);

		return ans;
	}

	else
	{
		do
		{
			cout << "All In (" << (char)ALL_IN << ") or Fold (" << (char)FOLD << ")? ";
			ans = toupper(_getch());
			cout << ans << endl;
		} while (ans != ALL_IN && ans != FOLD);

		if (ans == ALL_IN) // Count an all-in as a call
			return CALL;

		return ans;
	}
}

/*
Deducts chips to pay a call and adds them to the pot.  "Bet" 
is the TOTAL amount players must pay in order to stay in the round.  
If a Player does not have enough chips, then he simply goes all in.
*/
void Game::handleCall(Player * p, ChipAmt bet)
{
	ChipAmt toPay = bet - p->amtPaid;
	if (p->chips < toPay)
	{
		pot += p->chips;
		p->chips = 0;
	}
	else
	{
		p->chips -= toPay;
		p->amtPaid += toPay;
		pot += toPay;
	}
}

/*
Prompts for a unsigned long between MIN_BET and "max"  
Throws invalid_argument if "max" is lower than MIN_BET.
*/
ChipAmt Game::getBet(ChipAmt max)
{
	if (max < MIN_BET)
	{
		ostringstream err;
		err << "Max bet of " << max << " is less than MIN_BET of " << MIN_BET;
		throw invalid_argument(err.str());
	}

	ChipAmt amt = 0;
	string raw;
	istringstream line;

	while (true)
	{
		line.clear();
		cout << "Enter amount between " << MIN_BET << " and " << max << ": ";
		getline(cin, raw);
		line.str(raw);
		if (line >> amt)
		{
			while (isspace(line.peek()))
				line.get(); // Extract whitespace

			if ( (line.eof()) && (amt >= MIN_BET) && (amt <= max) )
				return amt;
		}
	}

	assert(false);
	return amt;
}

/*
Gets how much a Player is betting or raising. 
Deducts the Player's bet and adds it to the pot.
*/
ChipAmt Game::handleBet(Player * p)
{
	assert(p->chips > 0);

	ChipAmt bet;
	if (p->chips < MAX_BET)
		bet = getBet(p->chips);
	else
		bet = getBet();

	p->chips -= bet;
	p->amtPaid += bet;
	pot += bet;

	return bet;
}

/*
Returns true if the Player reset his chips, and false if the Player left the game.
*/
bool Game::handle0Chips(Player * p)
{
	assert(p->chips == 0);
	
	char ans = leaveResetPrompt(p);
	if (ans == RESET_CHIPS)
	{
		p->chips = Player::DEFAULT_CHIPS;
		return true;
	}
	else
	{
		return false;
	}
}

/*
Prompts the player to either reset his/her chips to 20 or leave the game.  
Assumes the player has 0 chips.
*/
char Game::leaveResetPrompt(Player * p)
{
	char ans = '\0';
	do
	{
		cout << p->name << " has 0 chips.  Reset chips to 20 (" << RESET_CHIPS << ") or Leave (" << LEAVE << ")? ";
		ans = toupper(_getch());
		cout << ans << endl;
	} while (ans != RESET_CHIPS && ans != LEAVE);

	return ans;
}
