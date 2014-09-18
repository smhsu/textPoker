/*
Lab5.cpp
Silas Hsu // hsu.silas@wustl.edu
Last updated December 9, 2013

Main entry point.
*/

#include "stdafx.h"
#include "Game.h"

#include <iostream>
#include <conio.h>

using namespace std;

enum MenuChoice
{
	FIVE_CARD_DRAW = '1',
	SEVEN_CARD_STUD = '2',
	TEXAS_HOLD_EM = '3',
	QUIT = '4'
};

char mainMenu()
{
	cout << "Welcome to text poker!  Main menu:\n" <<
		(char)FIVE_CARD_DRAW << " - play Five Card Draw\n" <<
		(char)SEVEN_CARD_STUD << " - play Seven Card Stud\n" <<
		(char)TEXAS_HOLD_EM << " - play Texas Hold 'Em\n" <<
		(char)QUIT << " - Quit" << endl;
	cout << "-> ";

	char ans = toupper(_getch());
	cout << ans << endl;
	while (ans < FIVE_CARD_DRAW || ans > QUIT)
	{
		cout << "Choose a valid option.\n-> ";
		ans = toupper(_getch());
		cout << ans << endl;
	}

	return ans;
}

int main (int argc, char * argv[])
{
	do
	{
		char ans = mainMenu();
		switch (ans)
		{
		case FIVE_CARD_DRAW:
			Game::start_game("FiveCardDraw");
			break;
		case SEVEN_CARD_STUD:
			Game::start_game("SevenCardStud");
			break;
		case TEXAS_HOLD_EM:
			Game::start_game("TexasHoldEm");
			break;
		case QUIT:
			return 0;
		}
		
		Game::instance()->play();
		cout << endl;

	} while (true);

	return 0;
}