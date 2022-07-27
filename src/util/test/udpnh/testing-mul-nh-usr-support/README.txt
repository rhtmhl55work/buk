Introduction:
--------------
	This directory contains one .cpp file. This is written to 
ensuer UDP NH supports existing multiple user and multiple NH.

Working.
--------
   1) Local Setup.
	*) Open tester.cpp
	*) Change ip address according to setup.
	*) make sure 4 port mentioned in tester.cpp are not already in
use in local and remote.)
 	*) make CLIENT=1
   2) Remote Setup
	*) Copy the tester.cpp to remote machine
	*) Change the Ipadress according to setup [Note dont change Ip
	Address]
	*) make SERVER = 1

  3) Run Both Test program
  4) Enter 1, to send data to other end, q to quit.
-----------------
End
-----------------	
