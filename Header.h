/*
Title:					  Maze Runner V 3.0
Description:		A retro-style maze game
Author:							Tim Johnson
Date:							 10-13-2022
© 2023 Riley Media Inc. All Righs Reserved.
*/

#ifndef HEADER_INCLUDE
#define HEADER_INCLUDE
#include    <iostream>
#include     <iomanip>
#include     <fstream>
#include     <cstdlib>
#include      <string>
#include   <windows.h>  
#include  <mmsystem.h>
#include     <conio.h>
#include      <vector>
#endif
using namespace std;

struct _SpriteTypes
{
	const char $square = 219;
	const char $left   = 221;
	const char $right  = 222;
	const char $top    = 223;
	const char $bottom = 220;
	const char $blank  = ' ';
};

enum _Ink
{
	   black = 0x00,  darkBlue = 0x01,   darkGreen = 0x02,  darkCyan = 0x03,
	 darkRed = 0x04,    purple = 0x05,  darkYellow = 0x06, lightGrey = 0x07,
	darkGrey = 0x08, lightBlue = 0x09,  lightGreen = 0x0A, lightCyan = 0x0B,
	lightRed = 0x0C,   magenta = 0x0D, lightYellow = 0x0E,     white = 0x0F
};

struct _ScreenCoords
{
	int x, y;

	_ScreenCoords(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
};

struct _Player
{
	_Ink spriteColor, mazeColor;

	_Player(_Ink _spriteColor, _Ink _mazeColor)
	{
		spriteColor = _spriteColor;
		mazeColor   =   _mazeColor; 
	}
};

string fileNameBuilder(string difficulty, string level)
{
	string fileName = "mazes\\" + difficulty + "\\" + level + ".mcp";
	return fileName;
}

_SpriteTypes SpriteTypes;
bool endGame = false;

_Player createPlayerData(void)
{
	ofstream outFile;
	ifstream inFile;
	inFile.open("playerData.cfg");
	int spriteColorCode = 0, mazeColorCode = 0;
	_Ink spriteColor, mazeColor;

	if (inFile.fail())
	{
		cout    << endl; 
		outFile.open("playerData.cfg");
		cout    << "    Welcome to maze runner!" << endl;
		cout    << "    Enter your desired sprite color (See user guide for color codes): ";
		cin     >> spriteColorCode;
		cout    << "    Enter your desired maze color: ";
		cin     >> mazeColorCode;
		outFile << spriteColorCode << "\n" << mazeColorCode;
		outFile.close();
	}

	else
	{
		inFile >> spriteColorCode >> mazeColorCode; 
		inFile.close(); 
	}

	spriteColor = static_cast<_Ink>(spriteColorCode);
	mazeColor = static_cast<_Ink>(mazeColorCode);
	_Player Player(spriteColor, mazeColor);
	system("cls"); 
	Sleep(500); 
	return Player;
}

class _ConsoleEngine
{
private:
	COORD coordScreen;
	HANDLE consoleOutput;

public:

	_ConsoleEngine(void)
	{
		coordScreen.X = 0;
		coordScreen.Y = 0;
		consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void plotPoint(int x, int y, char type)
	{
		coordScreen.X = x;
		coordScreen.Y = y;
		SetConsoleCursorPosition(consoleOutput, coordScreen);
		cout << type;
		cout.flush();
	}

	void setPlotColor(_Ink Ink)
	{
		SetConsoleTextAttribute(consoleOutput, Ink);
	}
};

class _GameSystem
{
private:
	_ConsoleEngine ConsoleEngine; 
	vector<_ScreenCoords*> MapGrid;	
	ifstream inFile;
	ofstream outFile; 
	bool fileError = false; 
	
public:

	void playerControl(string difficulty)
	{
		int y = 0;
		_ScreenCoords* PlayerCoords = new _ScreenCoords(0, 0);

		if (fileError == false)
		{
			if (difficulty == "Easy")
			{
				y = 12;
			}

			else if (difficulty == "Normal")
			{
				y = 16;
			}

			else if (difficulty == "Hard")
			{
				y = 20;
			}

			else if (difficulty == "Expert")
			{
				y = 24;
			}
			
			PlayerCoords->x = 6;
			PlayerCoords->y = y;
			ConsoleEngine.plotPoint(PlayerCoords->x, PlayerCoords->y, SpriteTypes.$square);
		}

		while (endGame == false && fileError == false)
		{
			while (_kbhit())
			{
				char c = tolower(_getch());

				switch (c)
				{
				case 'w':
					PlayerCoords->y--;
					break;

				case 's':
					PlayerCoords->y++;
					break;

				case 'a':
					PlayerCoords->x--;
					break;

				case 'd':
					PlayerCoords->x++;
					break;

				case 'q':
					endGame = true; 
					break;

				default:
					break;
				}
			}

			if (endGame == false)
			{
				for (_ScreenCoords* MapCoords : MapGrid)
				{
					if (PlayerCoords->x == MapCoords->x && PlayerCoords->y == MapCoords->y)
					{
						system("cls");
						ConsoleEngine.setPlotColor(white);
						cout << endl;
						cout << "    Oh No! You've crashed." << endl;
						cout << "    ";
						Beep(415, 500);
						Beep(392, 500);
						Beep(330, 1000);
						system("pause");
						Beep(200, 250);
						MapGrid.clear();
						endGame = true;
						break;
					}
				}

				ConsoleEngine.plotPoint(PlayerCoords->x, PlayerCoords->y, SpriteTypes.$square);

				if (PlayerCoords->y == 0)
				{
					Sleep(500);
					system("cls");
					ConsoleEngine.setPlotColor(white);
					cout << endl;
					cout << "    Maze Complete" << endl;
					cout << "    ";
					Beep(330, 500);
					Beep(392, 500);
					Beep(415, 1000);
					system("pause");
					Beep(200, 250);
					endGame = true;
					break;
				}
			}
		}

		delete PlayerCoords;
		endGame = false;
		fileError = false;
		ConsoleEngine.setPlotColor(white); 
	}

	void renderMap(string mapFilePath)
	{
		int x = 0;
		int y = 0;
		_ScreenCoords* MapCoords = new _ScreenCoords(0,0);
		inFile.open(mapFilePath); 

		if (!inFile.fail())
		{
			while (!inFile.eof())
			{
				inFile >> x >> y; 
				MapCoords = new _ScreenCoords(x+4, y+1); 
				MapGrid.push_back(MapCoords); 
			}

			for (_ScreenCoords* MapCoords : MapGrid)
			{
				ConsoleEngine.plotPoint(MapCoords->x, MapCoords->y, SpriteTypes.$square);
				Sleep(10);				
			}

			Beep(261,  500);
			Beep(261,  500);
			Beep(261,  500); 
			Beep(330, 1000);
		}

		else
		{
			ConsoleEngine.setPlotColor(white); 
			cout << endl; 
			cout << "    Error: File Not Found" << endl;
			cout << "    "; 
			system("pause"); 
			Beep(200, 250);
			fileError = true; 
		}

		delete MapCoords; 
		inFile.close();
	}

	bool createMapGrid(string mapPlotFile, string mapGridFile)
	{
		outFile.open(mapGridFile); 
		inFile.open(mapPlotFile); 
		vector<string> mapLines; 
		string mapLine; 
		bool good = true; 
		int x = 0;
		int y = 0;
		
		if (!inFile.fail())
		{
			while (!inFile.eof())
			{
				getline(inFile, mapLine);
				mapLines.push_back(mapLine); 
			}

			for (string line : mapLines)
			{
				for (char c : line)
				{
					if (c == '#')
					{
						outFile << x << "\t" << y << endl;
					}

					x++; 
				}

				y++;
				x = 0; 
			}
		}

		else
		{
			good = false; 
		}

		inFile.close(); 
		outFile.close(); 
		return good; 
	}
};

class _MenuSystem
{
private:
	_GameSystem GameSystem;
	_ConsoleEngine ConsoleEngine; 
	ifstream inFile;
	ofstream outFile; 
	bool menuBreak = false;	

	int mainMenu(void)
	{
		int choice;
		cout << endl;
		cout << "                 Main Menu             " << endl;
		cout << "    ===================================" << endl; 
		cout << endl;
		cout << "    Play                              1" << endl;
		cout << endl;
		cout << "    Create a Maze                     2" << endl;
		cout << endl;
		cout << "    Info                              3" << endl;
		cout << endl; 
		cout << "    Exit                              4" << endl;
		cout << endl;
		cout << "    ===================================" << endl; 
		cout << endl;
		cout << "    Select desired option: ";
		cin >> choice;
		Beep(200, 250);
		system("cls");
		Sleep(500); 
		return choice; 
	}

	string difficultyMenu(void)
	{
		int choice; 
		string difficulty = " ";
		cout << endl; 
		cout << "              Difficulty Select        " << endl;
		cout << "    ===================================" << endl; 
		cout << endl;
		cout << "    Easy:                             1" << endl;
		cout << endl;
		cout << "    Normal:                           2" << endl;
		cout << endl;
		cout << "    Hard:                             3" << endl;
		cout << endl;
		cout << "    Expert:                           4" << endl;
		cout << endl;
		cout << "    ===================================" << endl;
		cout << endl;
		cout << "    Select desired option: ";
		cin >> choice;
		Beep(200, 250);
		system("cls"); 
		Sleep(500); 

		switch (choice)
		{
		case 1: 
			difficulty = "Easy";
			break;

		case 2: 
			difficulty = "Normal";
			break;

		case 3: 
			difficulty = "Hard";
			break;

		case 4: 
			difficulty = "Expert"; 
			break;

		default: 
			cout << endl;
			cout << "    Error: Invalid Option" << endl; 
			cout << "    ";
			system("pause"); 
			Beep(200, 250);
			menuBreak = true; 
			break;
		}
		
		return difficulty;
	}

	string levelMenu(void)
	{
		int choice; 
		string level;
		cout << endl; 
		cout << "    Enter level #: ";
		cin >> choice;
		Beep(200, 250);
		level = "Level" + to_string(choice);
		Sleep(500); 
		return level;
	}	

	void createAMaze(void)
	{
		string inputFilename, outputFilename;
		cout << endl;
		cout << "    Welcome to the maze builder." << endl;
		cout << "    Refer to the user guide on how to create a maze plot." << endl;
		cout << "    Enter the file name for the maze plot: ";
		cin >> inputFilename;
		outputFilename = inputFilename;
		outputFilename += ".mcp"; 
		inputFilename += ".txt"; 
		cout << endl;
		bool success = GameSystem.createMapGrid(inputFilename, outputFilename);

		if (success == true)
		{			
			cout << "    map grid creation successfull" << endl;
			cout << endl;
		}

		else
		{
			cout << "    map grid creation failed" << endl; 
			cout << endl;
		}

		cout << "    "; 
		system("pause"); 
		Beep(200, 250);
	}	

public:
	void fileChecker(void)
	{
		const string difficulties[4] = { "Easy", "Normal", "Hard", "Expert" };
		const string levels[4] = { "Level1","Level2","Level3","Level4" };
		string file = "";
		bool fail = false;
		ConsoleEngine.setPlotColor(white); 
		cout << "Checking maze files..." << endl;
		cout << endl; 
		for (string difficulty : difficulties)
		{
			for (string level : levels)
			{
				ConsoleEngine.setPlotColor(white); 
				file = fileNameBuilder(difficulty, level);
				inFile.open(file);

				if (inFile.fail())
				{
					cout << "file\t\t" << file << " ";
					ConsoleEngine.setPlotColor(lightRed);
					cout << "  \tERROR" << endl;
					fail = true;
				}

				else
				{
					cout << "file\t\t" << file << " ";
					ConsoleEngine.setPlotColor(lightGreen);
					cout << "  \tOK" << endl;
				}
				
				Sleep(100); 
				inFile.close(); 
			}

		}

		cout << endl; 

		if (fail == true)
		{
			ConsoleEngine.setPlotColor(lightRed); 
			cout << "One or more files are either corrupted or missing." << endl;
			cout << "Please recheck files and run the file checker again." << endl;
			cout << "If the issue persists, re-install the game." << endl;

			for (int i = 0; i < 10; i++)
			{
				Beep(500, 100); 
			}
		}

		else
		{
			cout << "All files are accounted for." << endl;
			Beep(500, 1000); 
		}

		ConsoleEngine.setPlotColor(white); 
		system("pause");
		system("cls"); 

		if (fail == true)
		{
			exit(1); 
		}
	}

	void startMenuSystem(void)
	{
		system("cls"); 
		Sleep(500);
		_Player Player = createPlayerData(); 
		int playerChoice = mainMenu();
		string difficulty = "";
		string level = ""; 
		string filename = ""; 

		switch (playerChoice)
		{
		case 1:
			difficulty = difficultyMenu(); 
			if (menuBreak == false)
			{
				level = levelMenu();
				filename = fileNameBuilder(difficulty, level);
				system("cls");
				ConsoleEngine.setPlotColor(Player.mazeColor);
				GameSystem.renderMap(filename);
				ConsoleEngine.setPlotColor(Player.spriteColor);
				GameSystem.playerControl(difficulty);
			}
			break;

		case 2: 
			createAMaze();
			break;

		case 3:
			cout << endl; 
			cout << "    =========== Program Info =================="  << endl; 
			cout << "    Maze Runner"								   << endl;
			cout << "    Version 3.0"								   << endl;
			cout << "    Riley Media Inc."						       << endl; 
			cout << "    Developed by: Tim 'Nomad' Johnson"			   << endl; 
			cout << endl;
			cout << "    ============ Special Thanks ==============="  << endl; 
			cout << "    Dr. Carlos Monroy"							   << endl;
			cout << "    Dr. Charles Thangaraj"						   << endl;
			cout << "    Dr. John Starner"							   << endl;
			cout << "    Dr. Jack Follis"						       << endl;
			cout << "    Dr. Bill Booth"							   << endl;
			cout << "    Thank you for all your help and support :-)!" << endl; 
			cout << endl;
			cout << "    ";
			system("Pause");
			Beep(200, 250);
			break; 

		case 4: 
			exit(0x00); 
			break;

		default: 
			cout << endl;
			cout << "    Error: Invalid Option" << endl;
			cout << "    "; 
			system("pause");
			Beep(200, 250);
			break;
		}

		menuBreak = false; 
	}
};

void loadingScreen(void)
{
	_ConsoleEngine ConsoleEngine;
	cout << endl;
	cout << "    MazeRunner V. 3.0" << endl;
	cout << "    (C) 2023 Riley Media Inc. All Righs Reserved." << endl;
	Sleep(2000);
	system("cls"); 
	Sleep(500); 
	cout << endl;
	cout << "    Loading" << endl;
	cout << endl;
	Sleep(500);
	cout << "    ";
	ConsoleEngine.setPlotColor(darkGrey);

	for (int i = 0; i < 50; i++)
	{
		cout << SpriteTypes.$square;
		Sleep(50);
	}

	ConsoleEngine.setPlotColor(white);
	Sleep(500);
	system("cls");
	Sleep(500);
}

void mainDriver(void)
{	
	_MenuSystem MenuSystem;
	MenuSystem.fileChecker(); 
	loadingScreen();

	while (true)
	{		
		MenuSystem.startMenuSystem(); 
	}	
}