Multiplayer_TicTacToe
=====================

A simple multiplayer version of TicTacToe, written in C++ using TCP sockets with SFML and Boost.Thread


This game uses a client-server architecture. The clients will talk to the server with a command,
the command will be processed and the server will determine whether or not it was valid to prevent cheating. If the command was valid, the server will tell one or both clients what they should do. 


To use: Start up TicTacToe_SERVER.exe, this is the server. 

Start up 2 instances of TicTacToe.exe (this is the client), and follow the instructions displayed on the server in order to connect. 
