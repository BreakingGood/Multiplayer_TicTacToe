

#include "globals.h"

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "symbol.h"
#include "client_network_thread.h"



sf::Socket::Status connectToServer(sf::TcpSocket & socket, sf::IpAddress serverIP);
sf::Socket::Status retryConnection(sf::TcpSocket & socket, char & yn, sf::IpAddress serverIP);
char assignSymbol(sf::TcpSocket & socket); 
void drawGrid(sf::RenderWindow & window, std::vector<sf::RectangleShape> & gridX, std::vector<sf::RectangleShape> & gridY);
void drawSymbolGrid(sf::RenderWindow & window, std::vector<symbol> & symbolGrid, sf::Font & font);
void setupLogicalGrid(sf::RenderWindow & window, std::vector<logicalGridRect> & logicalGrid, sf::Color & logicalGridColor);
void updateSymbolGrid(std::vector<symbol> & symbolGrid, unsigned int i, char playerSymbol);
bool checkMove(std::vector<symbol> & symbolGrid, unsigned int i);
void updateConsoleText(sf::Text & consoleText, std::string text, sf::Color color, sf::RenderWindow & window);

int main()
{

	std::cout << "Connecting to server...\n";
	sf::IpAddress serverIP;
	std::cout << "Type in the IP displayed on the server to join..." << std::endl;
	std::cin >> serverIP;

	sf::TcpSocket socket;
	sf::Socket::Status status;

	status = connectToServer(socket, serverIP); //establish connection to server

	if (status != sf::Socket::Done) //if connection failed, offer the chance to retry
	{
		char yn = 'y';
		
		while ((status != sf::Socket::Done) && (yn == 'y'))
		{
			status = retryConnection(socket, yn, serverIP);

			if (status == sf::Socket::Status::NotReady) //if connection was not succesful and user did not enter 'y', end program with error
				return 1;
		}
	}
	std::cout << "Connected!" << std::endl;
	std::cout << "Waitng for other player..."; //This player has successfully connected and is just waiting for the next player

	char playerSymbol = assignSymbol(socket); //assign symbol the player will be using (X or O)
	char opponentSymbol;
	if (playerSymbol == 'x')
		opponentSymbol = 'o';
	else
		opponentSymbol = 'x';
	

	//ENTER GAME WMWMWMWWMWMWMWMW ENTER GAME WMWMWMWWMWMWMWMW ENTER GAME WMWMWMWWMWMWMWMW ENTER GAME WMWMWMWWMWMWMWMW ENTER GAME 
	//ENTER GAME WMWMWMWMWMWMWMMW ENTER GAME WMWMWMWMWMWMWMMW ENTER GAME WMWMWMWMWMWMWMMW ENTER GAME WMWMWMWMWMWMWMMW ENTER GAME 

	int command = 0;
	int opponentCoord;
	boost::thread networkThread(boost::bind(&checkNetwork,boost::ref(command), boost::ref(socket), boost::ref(opponentCoord))); 
	//^^ Begin listening for communication from the server on another thread

	const float consoleSize = 30; //black bar at the bottom of the screen where helpful info is displayed
	sf::RenderWindow window(sf::VideoMode(300, 300 + consoleSize), "Tic-Tac-Toe");

	sf::RectangleShape background;
	background.setFillColor(sf::Color::Blue);
	background.setSize(sf::Vector2f(window.getSize().x,window.getSize().y - consoleSize));//draws the blue background

	std::vector<sf::RectangleShape> gridX;
	std::vector<sf::RectangleShape> gridY;
	drawGrid(window,gridX,gridY); //draws the grid of white lines

	sf::Font font;
	font.loadFromFile("Assets/OpenSans-Regular.ttf");
	std::vector<symbol> symbolGrid;
	drawSymbolGrid(window, symbolGrid, font); //draw the grid of symbols

	std::vector<logicalGridRect> logicalGrid;
	sf::Color logicalGridColor(187,188,242,100);
	setupLogicalGrid(window, logicalGrid, logicalGridColor); //Checks where the mouse in to determine selected square when clciking/hovering

	sf::Text consoleText;
	consoleText.setCharacterSize(20);
	consoleText.setFont(font);
	
	if (playerSymbol == 'x')
		updateConsoleText(consoleText, "YOUR TURN", sf::Color::Green, window);
	else
		updateConsoleText(consoleText, "OPPONENT'S TURN", sf::Color::Red, window);
		

	//EVENTS IIIIIII EVENTS IIIIIIIIIII EVENTS IIIIIIIIIIIIIII EVENTS IIIIIIIIIIII EVENTS

	int moveCoord = 0;
	sf::Packet packetSent;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if ((event.type == sf::Event::MouseButtonPressed) && (command == PLAY_TURN)) 
			{
				for(unsigned int i = 0; i < logicalGrid.size(); i++)
				{
					if((logicalGrid[i].checkMouse(window)) && (checkMove(symbolGrid,i)))
					{
						packetSent.clear();
						packetSent << PLAY_TURN << i;
						socket.send(packetSent);
						updateSymbolGrid(symbolGrid,i,playerSymbol);
					}
				}
			}
		} 

	//LOGIC IIIIIIIIIIII LOGIC IIIIIIIIIIIIII LOGIC IIIIIIIIIIIIII LOGIC   IIIIIIIIIIIIIII LOGIC IIIIIIIIIIIIIIII LOGIC

		if(command == VALID_MOVE)
		{
			command = 0;
		}
		else if (command == OPPONENT_MOVE)
		{
			updateSymbolGrid(symbolGrid, opponentCoord, opponentSymbol);
			command = 0;
		}
		else if (command == PLAY_TURN)
		{
			updateConsoleText(consoleText, "YOUR TURN", sf::Color::Green, window);
		}
		else
		{
			updateConsoleText(consoleText, "OPPONENT'S TURN", sf::Color::Red, window);
		}

		if (command == WIN)
		{
			updateConsoleText(consoleText, "YOU WON", sf::Color::White, window);
		}
		else if (command == LOSE)
		{
			updateConsoleText(consoleText, "YOU LOST", sf::Color::White, window);
		}
		else if (command == TIE)
		{
			updateConsoleText(consoleText, "TIE GAME", sf::Color::Yellow, window);
		}


	//RENDERING IIIIIIIIIIIIIII RENDERING IIIIIIIIIIIIII RENDERING IIIIIIIIIIIIIII RENDERING IIIIIIIIIIIIIIIIIII RENDERING
		window.clear();
		window.draw(background);

		for (unsigned int i = 0; i < gridX.size(); i++)
		{
			window.draw(gridX[i]);
			window.draw(gridY[i]);
		}

		for (unsigned int i = 0; i < symbolGrid.size(); i++)
			window.draw(symbolGrid[i]);

		for(unsigned int i = 0; i < logicalGrid.size(); i++)
		{
			if (logicalGrid[i].checkMouse(window)) //check if mouse is inside the square
			{
				window.draw(logicalGrid[i]); //if so, highlight it
			}
		}

		window.draw(consoleText);

		window.display(); 
	}

	return 0;
}

sf::Socket::Status connectToServer(sf::TcpSocket & socket, sf::IpAddress serverIP)
{
	sf::Socket::Status status = socket.connect(serverIP,53000);
	return status;
}

sf::Socket::Status retryConnection(sf::TcpSocket & socket, char & yn, sf::IpAddress serverIP)
{
	std::cout << "Failed to connect to server... Try again? (y/n)" << std::endl;
	std::cin >> yn;

	if (yn == 'y')
		return connectToServer(socket, serverIP);
	else 
		return sf::Socket::Status::NotReady;
}

char assignSymbol(sf::TcpSocket & socket)
{
	sf::Packet packet;
	socket.receive(packet);
	sf::Int16 playerNumber;
	packet >> playerNumber;
	std::cout << "Ready!" << std::endl;  //Once both players have connected the server will return a number to each client
	if (playerNumber == 1)				//The number will determine whether they're X or O
	{
		std::cout << "The game has begun, you will play as X" << std::endl;
		return 'x';
	}
	else
	{
		std::cout << "The game has begun, you will player as O" << std::endl;
		return 'o';
	}
}

void drawGrid(sf::RenderWindow & window, std::vector<sf::RectangleShape> & gridX, std::vector<sf::RectangleShape> & gridY)
{
	sf::RectangleShape line;
	line.setFillColor(sf::Color::White);

	gridX.push_back(line);
	gridX.push_back(line);
	gridY.push_back(line);
	gridY.push_back(line);

	float position;
	position = window.getSize().x / 3;

	int j = 0;
	for (unsigned int i = 1; i < 3; i = i++)
	{
		gridX[j].setPosition(position * i, 0);
		gridX[j].setSize(sf::Vector2f(1.f,window.getSize().x));
		j++;
	}

	j = 0;
	for (unsigned int i = 1; i < 3; i = i++)
	{
		gridY[j].setPosition(0,position * i);
		gridY[j].setSize(sf::Vector2f(window.getSize().x, 1.f));
		j++;
	}
}

void drawSymbolGrid(sf::RenderWindow & window, std::vector<symbol> & symbolStates, sf::Font & font)
{
	symbol symbolTemp(font);
	float position;
	position = window.getSize().x / 3;

	int j = 0;
	for (unsigned int q = 1; q < 6; q = q + 2)
	{
		for (unsigned int i = 1; i < 6; i = i + 2)
		{
			symbolStates.push_back(symbolTemp);
			symbolStates[j].setPosition(((position / 2) * i), ((position / 2) * q) - 30); //30 is adjusting for console size
			j++;
		}
	}
}

void setupLogicalGrid(sf::RenderWindow & window, std::vector<logicalGridRect> & logicalGrid, sf::Color & gridColor)
{
	logicalGridRect tempRect;
	for (unsigned int i = 0; i < 9; i++)
	{
		logicalGrid.push_back(tempRect);
	}
	int q = 0;
	for(unsigned int i = 0; i < 3; i++)
	{
		for(unsigned int j = 0; j < 3; j++)
		{
			logicalGrid[q].setPosition(window.getSize().x/3 * j, window.getSize().x/3 * i);
			logicalGrid[q].setSize(sf::Vector2f(window.getSize().x/3, window.getSize().x/3));
			logicalGrid[q].setFillColor(gridColor);
			q++;
		}
	}
}

bool checkMove(std::vector<symbol> & symbolGrid, unsigned int i)
{
	if (symbolGrid[i].symbolState != 0)
	{
		return false;
	}
	else
		return true;
}

void updateSymbolGrid(std::vector<symbol> & symbolGrid, unsigned int i, char playerSymbol)
{
	std::string string1;
	string1 = playerSymbol;
	symbolGrid[i].symbolState = playerSymbol;
	symbolGrid[i].setString(string1);
}

void updateConsoleText(sf::Text & consoleText, std::string text, sf::Color color, sf::RenderWindow & window)
{
	consoleText.setString(text);
	consoleText.setColor(color);
	consoleText.setOrigin(consoleText.getGlobalBounds().width/2, consoleText.getGlobalBounds().height/2);
	consoleText.setPosition(window.getSize().x/2,window.getSize().y-20);
}


