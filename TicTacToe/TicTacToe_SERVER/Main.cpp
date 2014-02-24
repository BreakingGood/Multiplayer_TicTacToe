#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>

bool checkPlayerMove(std::vector<int> & gridStates, int playerMovePosition);
void adjustGridStates(std::vector<int> & gridStates, int playerMovePosition, int playerTurn);
bool takeTurn(sf::Packet & sent, sf::Packet & received, sf::TcpSocket & player, sf::TcpSocket & otherPlayer, std::vector<int> & gridStates, int playerTurn);
int checkVictory(std::vector<int> gridStates);

//GLOBAL GLOBAL GLOBAL GLOBAL GLOBAL GLOBAL GLOBAL GLOBAL GLOBAL 

enum actionEnum{NONE, PLAY_TURN, VALID_MOVE, INVALID_MOVE, OPPONENT_MOVE, WIN, LOSE, PLAY_AGAIN, QUIT, TIE};
//				  0      1          2           3				4		   5     6		 7		  8     9
// PLAY_TURN packet format: packet >> action# >> square chosen (0-8)
// VALID/INVALID_MOVE : packet >> validMove


int main()
{
	sf::IpAddress IP;
	std::cout <<"MULTIPLAYER TIC-TAC-TOE v1.0 by /u/_BreakingGood_\n" << std::endl;
	std::cout << "Initializing server and waiting for connections...\n" << std::endl;
	std::cout << "Your local IP is: " << IP.getLocalAddress() << " players must type this to join over LAN\n" << std::endl;
	std::cout << "To connect over the internet you will need to enter your global IP\nand enable port forwarding on port 53000" << std::endl;
	sf::TcpListener listener;
	if (listener.listen(53000) != sf::Socket::Done) //Bind to port 53000 and wait for connections on that port
	{
		std::cout << "Could not bind to port 53000, Press a key to end" << std::endl;
		std::cin.get();
		return 1;
	}
	
	sf::TcpSocket player1;
	if (listener.accept(player1) != sf::Socket::Done) //Connect with player 1 
	{
		std::cout << "Failed to estalish connection with Player 1, Press a key to end" << std::endl;
		std::cin.get();
		return 1;
	}

	std::cout << "Player 1 connected..." << std::endl;

	sf::TcpSocket player2;
	if (listener.accept(player2) != sf::Socket::Done) //Conect w/ player 2
	{
		std::cout << "Failed to estalish connection with Player 2, Press a key to end" << std::endl;
		std::cin.get();
		return 1;
	}

	std::cout << "Player 2 connected..." << std::endl;

	sf::Packet playerOneSent;
	sf::Packet playerOneReceived;
	sf::Packet playerTwoSent;
	sf::Packet playerTwoReceived;

	sf::Int16 playerNumber = 1;
	playerOneSent << playerNumber;
	playerNumber = 2;
	playerTwoSent << playerNumber;

	player1.send(playerOneSent); //send numbers to players 1 and 2 telling them which symbol they play as
	player2.send(playerTwoSent);

	int ready = 0;
	int playerOneTempInt = 0;
	int playerTwoTempInt = 0;

	player1.receive(playerOneReceived);
	playerOneReceived >> playerOneTempInt;
	ready = ready + playerOneTempInt;

	
	player2.receive(playerTwoReceived);
	playerTwoReceived >> playerTwoTempInt;
	ready = ready + playerTwoTempInt;

	playerOneTempInt = 0;
	playerTwoTempInt = 0;

	//THE GAME HAS BEGUN IIIIIIIIIIIIIII GAME HAS BEGUN IIIIIIIIIIIIIIIIIIIIIIIII GAME HAS BEGUN IIIIIIIIIIIIIIIIIII

	std::vector<int> gridStates;
	for (unsigned int i = 0; i < 9; i++)
	{
		gridStates.push_back(0);
	}

	int action = 0;
	int playerMovePosition = 0;
	int playerTurn = 1;
	bool valid = false;

	bool start = false;
	if (ready == 2)
	{
		start = true;
		std::cout << "Game is starting..." << std::endl;
	}

	int winner = 0;

	while (start == true)
	{
		if (winner == 0)
		{
			playerTurn = 1;
			playerOneSent.clear();
			playerTwoSent.clear(); //clear all packets
			playerOneReceived.clear();
			playerTwoReceived.clear();

			playerOneSent << PLAY_TURN; //tell player 1 it is their turn
			player1.send(playerOneSent);
		
			while (!valid)
			{
				playerOneReceived.clear();
				player1.receive(playerOneReceived); //receive action from player 1
				playerOneReceived >> action;//the first byte of evey packet will contain an enum with the command ID

				switch (action)//decide what to do with that action
				{
				case 1: //make a move

					valid = takeTurn(playerOneSent, playerOneReceived, player1, player2, gridStates, playerTurn);
					action = 0;
					break;
				};

			}

			valid = false;
		}

		winner = checkVictory(gridStates);

		if (winner == 0)
		{
			playerTurn = 2;
			playerOneSent.clear();
			playerTwoSent.clear(); //clear all packets
			playerOneReceived.clear();
			playerTwoReceived.clear();

			playerTwoSent << PLAY_TURN; //tell player 2 it is their turn
			player2.send(playerTwoSent);
		
			while (!valid)
			{
				playerTwoReceived.clear();
				player2.receive(playerTwoReceived); //receive action from player 1
				playerTwoReceived >> action;//the first byte of evey packet will contain an enum with the command ID

				switch (action)//decide what to do with that action
				{
				case 1: //make a move

					valid = takeTurn(playerTwoSent, playerTwoReceived, player2, player1, gridStates, playerTurn);
					action = 0;
					break;
				};

			}
			valid = false;
		}

		winner = checkVictory(gridStates);

		if (winner == 1)
		{
			playerOneSent.clear();
			playerOneSent << WIN; //inform clients of a winner
			playerTwoSent.clear();
			playerTwoSent << LOSE;

			player1.send(playerOneSent);
			player2.send(playerTwoSent);
		}
		else if (winner == 2)
		{
			playerTwoSent.clear();
			playerTwoSent << WIN;
			playerOneSent.clear();
			playerOneSent << LOSE;

			player1.send(playerOneSent);
			player2.send(playerTwoSent);
		}
		else if (winner == 3)
		{
			playerTwoSent.clear();
			playerTwoSent << TIE;
			playerOneSent.clear();
			playerOneSent << TIE;

			player1.send(playerOneSent);
			player2.send(playerTwoSent);
		}

	}

	return 0;
}

bool checkPlayerMove(std::vector<int> & gridStates, int playerMovePosition)
{
	if((gridStates[playerMovePosition] == 1) || (gridStates[playerMovePosition] == 2))
	{
		std::cout << "Player has selected an invalid spot" << std::endl;
		return false;
	}
	else
		return true;
}

void adjustGridStates(std::vector<int> & gridStates, int playerMovePosition, int playerTurn)
{
	if (playerTurn == 1)
	{
		gridStates[playerMovePosition] = 1;
	}
	else
	{
		gridStates[playerMovePosition] = 2;
	}

	std::cout << "Player " << playerTurn << " has chosen square " << playerMovePosition << std::endl; 
}

bool takeTurn(sf::Packet & sent, sf::Packet & received, sf::TcpSocket & player, sf::TcpSocket & otherPlayer, std::vector<int> & gridStates, int playerTurn)
{
	int playerMovePosition;

	received >> playerMovePosition;
	if (checkPlayerMove(gridStates,playerMovePosition)) //check if move is valid
	{
		adjustGridStates(gridStates,playerMovePosition,playerTurn); //if so, make move
		sent.clear();
		sent << VALID_MOVE; //tell player the move is OK
		player.send(sent);
		sent.clear();
		sent << OPPONENT_MOVE << playerMovePosition;
		otherPlayer.send(sent);
		return true;
	}
	else //move was invalid
	{
		sent.clear();
		sent << INVALID_MOVE;
		player.send(sent);
		return false;
	}
}

int checkVictory(std::vector<int> gridStates)
{

	int tie = 0;

	for(unsigned int i = 0; i < gridStates.size(); i++)
	{
		if (gridStates[i] != 0)
		{
			tie++;
		}
	}

	if ((gridStates[0] == gridStates[1]) && (gridStates[0] == gridStates[2]) && (gridStates[0] != 0)) //all possible victories
		return gridStates[0];
	
	else if ((gridStates[3] == gridStates[4]) && (gridStates[3] == gridStates[5]) && (gridStates[3] != 0))
		return gridStates[3];
	
	else if ((gridStates[6] == gridStates[7]) && (gridStates[6] == gridStates[8]) && (gridStates[6] != 0))
		return gridStates[6];
	
	else if ((gridStates[0] == gridStates[3]) && (gridStates[0] == gridStates[6]) && (gridStates[0] != 0))
		return gridStates[0];
	
	else if ((gridStates[1] == gridStates[4]) && (gridStates[1] == gridStates[7]) && (gridStates[1] != 0))
		return gridStates[1];
	
	else if ((gridStates[2] == gridStates[5]) && (gridStates[2] == gridStates[8]) && (gridStates[2] != 0))
		return gridStates[2];
	
	else if ((gridStates[0] == gridStates[4]) && (gridStates[0] == gridStates[8]) && (gridStates[0] != 0))
		return gridStates[0];
	
	else if ((gridStates[2] == gridStates[4]) && (gridStates[2] == gridStates[6]) && (gridStates[2] != 0))
		return gridStates[2];

	else if (tie == 9)
		return 3;
	
	else
		return 0;
}

