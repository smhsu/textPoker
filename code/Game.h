/*
Game.h
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

An abstract class from which all poker games derive.  All Games 
follow the Singleton paradigm.
*/

#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Deck.h"

#include <vector>
#include <functional>

typedef unsigned long ChipAmt;

enum PokerChoice
{
	ALL_IN = 'A',
	BET = 'B',
	CHECK = 'C',
	CALL = 'C',
	FOLD = 'F',
	RAISE = 'R'
};

class Game
{
public:
	static Game * instance();
	virtual ~Game();

	static std::string gameNamePrompt();
	static void start_game(const std::string & name);
	static void stop_game();

	// Player modification
	unsigned int getNumPlayers() const;
	void add_player(const std::string & name);
	void add_player(Player * const p);
	void remove_player(const std::string & name);
	void remove_player(size_t n);
	Player * find_player(const std::string & find) const;

	virtual void play() = 0;
	virtual int before_turn(Player & p) = 0;
	virtual int turn(Player & p) = 0;
	virtual int after_turn(Player & p) = 0;
	virtual int before_round() = 0;
	virtual int round() = 0;
	virtual int after_round() = 0;

protected:
	Game(size_t deckSize, size_t maxPlayers);
	void standardDeck();

	void allJoinRound();
	int goAround(std::mem_fun1_t<int, Game, Player &> doWhat, bool includeFolded = false);

	void collectAnte();
	void collectBets();

	void earlyWin();
	void awardWinners(std::vector<Player *> & winners);
	void dividePot(std::vector<Player *>::iterator beg, std::vector<Player *>::iterator end);

	void printStatsAndHands();
	void removePlayersPrompt();
	void addPlayersPrompt();
	void allHandsToDeck();

	static Game * gameInstance;
	Deck deck;
	std::vector<Player *> players;
	size_t playersInRound;
	size_t dealerPos;
	ChipAmt pot;

	static const int OUT_OF_CARDS = 1;
	static const int EARLY_WINNER = 2;

private:
	// Undefined, so that no copies can be made.
	Game(const Game & other);
	Game & operator= (const Game & other);

	// For collectBets()
	char checkBetPrompt(Player * p);
	char callRaiseFoldPrompt(Player * p, ChipAmt callAmt);
	void handleCall(Player * p, ChipAmt bet);
	ChipAmt getBet(ChipAmt max = MAX_BET);
	ChipAmt handleBet(Player * p);
	
	bool handle0Chips(Player * p);
	char leaveResetPrompt(Player * p);

	const size_t DECK_SIZE;
	const size_t MAX_PLAYERS;
	static const ChipAmt MIN_BET = 1;
	static const ChipAmt MAX_BET = 2;
	static const char LEAVE = 'L';
	static const char RESET_CHIPS = 'R';
};

#endif
