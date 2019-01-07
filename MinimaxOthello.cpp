#include <iostream>
#include <string>
#include <windows.h>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

	const int boardsize = 6;
	string board[boardsize][boardsize];
	
	vector <pair<int,int>> avspot;
	vector <pair<int,int>> bdisk;
	vector <pair<int,int>> wdisk;

void gotoxy(short x, short y) {
  COORD coord = { x, y };  
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
  
}

void initializeBoard() {
	
	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			board[i][j] = "O";
		}
	}
	
	board[ (boardsize/2)-1 ] [ (boardsize/2)-1 ] = "W";
	wdisk.push_back( make_pair( (boardsize/2)-1, (boardsize/2)-1 ) );
	board[ (boardsize/2) ] [ (boardsize/2) ] = "W";
	wdisk.push_back( make_pair( (boardsize/2), (boardsize/2) ) );

	board[ (boardsize/2) ] [ (boardsize/2)-1 ] = "B";
	bdisk.push_back( make_pair( (boardsize/2), (boardsize/2)-1 ) );
	board[ (boardsize/2)-1 ] [ (boardsize/2) ] = "B";
	bdisk.push_back( make_pair( (boardsize/2)-1, (boardsize/2) ) );
	
}

void printBoard() {
	cout << endl << endl;
	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			cout << setw(5) << board[i][j] << " ";
		}
		cout << endl << endl;
	}
	
}

void printVector(vector<pair<int,int>> vect) {
	
	for (auto i=vect.begin(); i!=vect.end(); i++) {
		cout << i->first << " , " << i->second << "      ";
	}
	cout << endl << endl;
	
}

string checkEnemy(string disk) {
	if ( disk == "W" ) {
		return "B";
		
	} else if ( disk == "B" ) {
		return "W";
	}
}

void refreshDiskplace() {
	
	wdisk.clear();
	bdisk.clear();
	
	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			if ( board[i][j] == "O" ) continue;
			else if ( board[i][j] == "B") {
				bdisk.push_back( make_pair(j,i) );
			}
			else if ( board[i][j] == "W" ) {
				wdisk.push_back( make_pair(j,i) );
			}
		}
	}
	
}

void checkDirection(string disk, string enemy, int y, int x, int vy, int vx) {
	
//	cout << board[y][x] << endl;
//	cout << x << " , " << y << endl;
//	cout << vx << " & " << vy << endl;
//	cout << endl << endl;
	
	if ( x > boardsize-1 || y > boardsize-1 ) {
		return;
	} else if ( board[y][x] == "O" ) {
		avspot.push_back( make_pair(x,y) );
	} else if ( board[y][x] == enemy ){
		x += vx;
		y += vy;
		checkDirection(disk,enemy,y,x,vy,vx);
	}
	
}

void findAvailableSpot(string disk, vector<pair<int,int>> diskplace) {
	
	avspot.clear();
	string enemy = checkEnemy(disk);
	
	for (auto i=diskplace.begin(); i!=diskplace.end(); i++) {
		if ( board[i->second-1][i->first] == enemy ) checkDirection(disk, enemy, i->second-1, i->first, -1, 0); //NORTH
		
		if ( board[i->second][i->first+1] == enemy ) checkDirection(disk, enemy, i->second, i->first+1, 0, 1); //EAST
		
		if ( board[i->second+1][i->first] == enemy ) checkDirection(disk, enemy, i->second+1, i->first, 1, 0); //SOUTH
		
		if ( board[i->second][i->first-1] == enemy ) checkDirection(disk, enemy, i->second, i->first-1, 0, -1); //WEST
		
		if ( board[i->second-1][i->first-1] == enemy ) checkDirection(disk, enemy, i->second-1, i->first-1, -1, -1); //NORTHWEST
			
		if ( board[i->second-1][i->first+1] == enemy ) checkDirection(disk, enemy, i->second-1, i->first+1, -1, 1); //NORTHEAST
		
		if ( board[i->second+1][i->first+1] == enemy ) checkDirection(disk, enemy, i->second+1, i->first+1, 1, 1); //SOUTHEAST
			
		if ( board[i->second+1][i->first-1] == enemy ) checkDirection(disk, enemy, i->second+1, i->first-1, 1, -1); //SOUTHWEST
			
	}
	printVector(avspot);
}

void turnEnemyDisk(string disk, vector<pair<int,int>> &diskplace, string enemy, vector<pair<int,int>> &ediskplace, int x, int y) {
	
	//check diskplace, connect, turn
	
	int smallx;
	int smally;
	int bigx;
	int bigy;
	
	for (auto i=diskplace.begin(); i!=diskplace.end(); i++) {
		
		if ( abs(x - i->first) == abs(y - i->second) ) {
			
			smallx = min(x, i->first);
			smally = min(y, i->second);
			bigx = max(x, i->first);
			bigy = max(y, i->second);
			
			for ( smallx, smally; smallx!=bigx && smally!=bigy; smallx++, smally++ ) {
				
				if ( board[smallx][smally] == enemy ) {
					
					board[smallx][smally] == disk;
				}
			}
		}
		
		if ( x == i->first ) {
			
			smally = min(y, i->second);
			bigy = max(y, i->second);
			
			for (smally; smally!=bigy; smally++) {
				
				if ( board[smally][i->first] == enemy ) {
					
					board[smally][i->first] = disk;
				}
			}
		}
		
		else if ( y == i->second ) {
			
			smallx = min(x, i->first);
			bigx = max(x, i->first);
			
			for (smallx; smallx!=bigx; smallx++) {
				
				if ( board[i->second][smallx] == enemy ) {
					
					board[i->second][smallx] = disk;
				}
			}
		}
	}
	
}

void insertNewDisk(string disk, vector<pair<int,int>> &diskplace, string enemy, vector<pair<int,int>> &ediskplace, int x, int y) { 
	
	for (auto i=avspot.begin(); i!=avspot.end(); i++) {
		if (i->first == x & i->second == y) {
			board[y][x] =  disk;
			diskplace.push_back( make_pair(x,y) );
			turnEnemyDisk(disk, diskplace, enemy, ediskplace, x, y);
		} 
	}
	
	
}

void minimaxTurn() {
	
}

void greedyTurn() {
	
}

int main() {
	
	int count = 1;
	int x,y;
	
	initializeBoard();
	
	while (true) {
		
		system("CLS");
		gotoxy(0,0);
		refreshDiskplace();
		printBoard();
		cout << "WDISK" << endl;
		printVector(wdisk);
		cout << "BDISK" << endl;
		printVector(bdisk);
		
		if ( count % 2 == 0 ) {
			
			cout << "W TURN (AVAILABLE SPOTS)" << endl;
			
			findAvailableSpot("W", wdisk);
			
			cout << "INPUT X , Y" << endl;
			cin >> x >> y;
			
			insertNewDisk("W", wdisk, "B", bdisk, x, y);
			
		} else {
			
			cout << "B TURN (AVAILABLE SPOTS)" << endl;
			
			findAvailableSpot("B", bdisk);
			
			cout << "INPUT X , Y" << endl;
			cin >> x >> y;
			
			insertNewDisk("B", bdisk, "W", wdisk, x, y);
			
		}
		count++;
		
		getchar();
	}
	
}
