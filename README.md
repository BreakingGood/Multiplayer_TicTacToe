Multiplayer_TicTacToe
=====================

A simple multiplayer version of TicTacToe, written in C++ using TCP sockets with SFML and Boost.Thread


This game uses a client-server architecture. The clients will talk to the server with a command,
the command will be processed and the server will determine whether or not it was valid to prevent cheating. If the command was valid, the server will tell one or both clients what they should do. 

SFML http://www.sfml-dev.org is used both for it's networking library as well as for it's graphics library. 

Boost.Thread http://www.boost.org/doc/libs/1_55_0/doc/html/thread.html is used to start a thread on the client which constantly listens for input from the server. A separate thread is required so that the window can continue updating and accepting input from the user. SFML has a threading library but it is not sufficient for the purposes of this project (Threads cannot be started on functions that use more than 1 argument, and said argument cannot be passed by reference) 

the font used is OpenSans http://www.google.com/fonts/specimen/Open+Sans and it is used under the Apache 2.0 license. 


To use: Start up TicTacToe_SERVER.exe, this is the server. 

Start up 2 instances of TicTacToe.exe (this is the client), and follow the instructions displayed on the server in order to connect. 
