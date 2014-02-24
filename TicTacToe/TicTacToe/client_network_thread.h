
void checkNetwork(int & command, sf::TcpSocket & socket, int & opponentCoord);

void checkNetwork(int & command, sf::TcpSocket & socket, int & opponentCoord)
{
	sf::Packet packetReceived;
	sf::Packet packetSent;

	int action = 0;
	
	packetSent << 1;
	socket.send(packetSent); //tell server we are ready to begin

	bool go = true;
	while(go)
	{
		
		socket.receive(packetReceived);
		packetReceived >> action;
		switch (action)
		{
		case PLAY_TURN:
			std::cout << "It's your turn" << std::endl;
			command = action;
			action = 0;
			break;
		case VALID_MOVE:
			std::cout << "Valid move" << std::endl;
			command = action;
			action = 0;
			break;
		case INVALID_MOVE:
			std::cout << "Invalid move" << std::endl;
			break;
		case OPPONENT_MOVE:
			std::cout << "Opponent made a valid move" << std::endl;
			command = action;
			packetReceived >> opponentCoord;
			break;
		case WIN:
			std::cout << "You win!" << std::endl;
			command = action;
			go = false;
			break;
		case LOSE:
			std::cout << "You lose!" << std::endl;
			command = action;
			go = false;
			break;
		case TIE:
			std::cout << "Tie game!" << std::endl;
			command = action;
			go = false;
			break;
		}
		
	}
}
