
enum actionEnum{NONE, PLAY_TURN, VALID_MOVE, INVALID_MOVE, OPPONENT_MOVE, WIN, LOSE, PLAY_AGAIN, QUIT, TIE};
//				  0      1          2           3				4		   5	 6		 7		   8	 9



// PLAY_TURN: packet >> action# >> square chosen (0-8)
// VALID/INVALID_MOVE: packet >> validMove
// OPPONENT_MOVE: packet >> action# >> square chosen (0-8)