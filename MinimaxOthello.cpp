#include <iostream>
#include <string>
#include <windows.h>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <limits>
#include <cstring>

using namespace std;

	// MAX == BLACK
	// MIN == WHITE

	const int boardsize = 8;
	string board[boardsize][boardsize];
	
	vector <pair<int,int>> avspot;
	vector <pair<int,int>> bdisk;
	vector <pair<int,int>> wdisk;
	
	int bestscore;
	int bestx;
	int besty;
	int asd = 0;

void gotoxy(short x, short y) {
  COORD coord = { x, y };  
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
  
}

void initializeBoard() {
	
	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			board[i][j] = "-";
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

void cloneBoard(string newboard[boardsize][boardsize], string gameboard[boardsize][boardsize]) {
	
//	memcpy( newboard, gameboard, sizeof(gameboard) );

	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			newboard[i][j] = gameboard[i][j];
		}
	}
}

void refreshDiskplace(string gameboard[boardsize][boardsize]) {
	
	wdisk.clear();
	bdisk.clear();
	
	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			if ( gameboard[i][j] == "-" ) continue;
			else if ( gameboard[i][j] == "B") {
				bdisk.push_back( make_pair(j,i) );
			}
			else if ( gameboard[i][j] == "W" ) {
				wdisk.push_back( make_pair(j,i) );
			}
		}
	}
	
}

void checkDirection(string gameboard[boardsize][boardsize], string disk, int y, int x, int vy, int vx) {
	
	string enemy = checkEnemy(disk);
	
	if ( x < 0 || x > boardsize-1 || y < 0 || y > boardsize-1 ) {
		return;
	} else if ( gameboard[y][x] == "-" ) {
		avspot.push_back( make_pair(x,y) );
	} else if ( gameboard[y][x] == enemy ){
		x += vx;
		y += vy;
		checkDirection(gameboard, disk,y,x,vy,vx);
	}
	cout << "CHECK DIRECTION DONE" << endl;
}

void findAvailableSpot(string gameboard[boardsize][boardsize], string disk, vector <pair<int,int>> &diskplace) {
	
	avspot.clear();
	refreshDiskplace(gameboard);
	string enemy = checkEnemy(disk);
	
	for (auto i=diskplace.begin(); i!=diskplace.end(); i++) { // if corner
		if ( i->second-1 > 0 && gameboard[i->second-1][i->first] == enemy ) checkDirection(gameboard, disk, i->second-1, i->first, -1, 0); //NORTH
		
		if ( i->first+1 < boardsize && gameboard[i->second][i->first+1] == enemy ) checkDirection(gameboard,disk, i->second, i->first+1, 0, 1); //EAST
		
		if ( i->second+1 < boardsize && gameboard[i->second+1][i->first] == enemy ) checkDirection(gameboard,disk, i->second+1, i->first, 1, 0); //SOUTH
		
		if ( i->first-1 > 0 && gameboard[i->second][i->first-1] == enemy ) checkDirection(gameboard,disk, i->second, i->first-1, 0, -1); //WEST
		
		if ( i->first-1 > 0 && i->second-1 > 0 && gameboard[i->second-1][i->first-1] == enemy ) checkDirection(gameboard,disk, i->second-1, i->first-1, -1, -1); //NORTHWEST
			
		if ( i->second-1 > 0 && i->first+1 < boardsize && gameboard[i->second-1][i->first+1] == enemy ) checkDirection(gameboard,disk, i->second-1, i->first+1, -1, 1); //NORTHEAST
		
		if ( i->second+1 < boardsize && i->first+1 < boardsize && gameboard[i->second+1][i->first+1] == enemy ) checkDirection(gameboard,disk, i->second+1, i->first+1, 1, 1); //SOUTHEAST
			
		if ( i->second+1 < boardsize && i->first-1 > 0 && gameboard[i->second+1][i->first-1] == enemy ) checkDirection(gameboard,disk, i->second+1, i->first-1, 1, -1); //SOUTHWEST
			
	}
	sort(avspot.begin(), avspot.end());
	avspot.erase( unique(avspot.begin(), avspot.end()), avspot.end() );
	
	cout << "FIND AVAILABLE SPOT DONE" << endl;
}

void turnEnemyDisk(string gameboard[boardsize][boardsize], string disk, vector<pair<int,int>> &diskplace, int x, int y) {
	
	//check diskplace, connect, turn
	
	int smallx;
	int smally;
	int bigx;
	int bigy;
	
	string enemy = checkEnemy(disk);
	
	for (auto i=diskplace.begin(); i!=diskplace.end(); i++) {
		
		if ( abs(x - i->first) == abs(y - i->second) ) {
			
			smallx = min(x, i->first);
			smally = min(y, i->second);
			bigx = max(x, i->first);
			bigy = max(y, i->second);
			
			for ( smallx, smally; smallx!=bigx && smally!=bigy; smallx++, smally++ ) {
				
				if ( gameboard[smally][smallx] == enemy ) {
					
					gameboard[smally][smallx] = disk;
				}
			}
		}
		
		if ( x == i->first ) {
			
			smally = min(y, i->second);
			bigy = max(y, i->second);
			
			for (smally; smally!=bigy; smally++) {
				
				if ( gameboard[smally][i->first] == enemy ) {
					
					gameboard[smally][i->first] = disk;
				}
			}
		}
		
		else if ( y == i->second ) {
			
			smallx = min(x, i->first);
			bigx = max(x, i->first);
			
			for (smallx; smallx!=bigx; smallx++) {
				
				if ( gameboard[i->second][smallx] == enemy ) {
					
					gameboard[i->second][smallx] = disk;
				}
			}
		}
	}
	cout << "TURN ENEMY DISK DONE" << endl;
}

void insertNewDisk(string gameboard[boardsize][boardsize], string disk, vector<pair<int,int>> &diskplace, int x, int y) { 
	
	string enemy = checkEnemy(disk);
	
	for (auto i=avspot.begin(); i!=avspot.end(); i++) {
		if (i->first == x && i->second == y) {
			gameboard[y][x] =  disk;
			turnEnemyDisk(gameboard, disk, diskplace, x, y);
		}
	}
	
	cout << "INSERT NEW DISK DONE" << endl;
}

int evaluate(string gameboard[boardsize][boardsize]) { 
	
	int blacktiles;
	int whitetiles;
	int bfronttiles;
	int wfronttiles;
	
	int X1[] = {-1, -1, 0, 1, 1, 1, 0, -1};
	int Y1[] = {0, 1, 1, 1, 0, -1, -1, -1};
	int V[8][8] = 
	{
		{20, -3, 11, 8, 8, 11, -3, 20},
	    {-3, -7, -4, 1, 1, -4, -7, -3},
	    {11, -4, 2, 2, 2, 2, -4, 11},
	    {8, 1, 2, -3, -3, 2, 1, 8},
	    {8, 1, 2, -3, -3, 2, 1, 8},
	    {11, -4, 2, 2, 2, 2, -4, 11},
	    {-3, -7, -4, 1, 1, -4, -7, -3},
		{20, -3, 11, 8, 8, 11, -3, 20}
	};
    	
	// PIECE DIFFERENCE, FRONTIER DISC AND DISC SQUARES
	double dif;
	double frontiers;
	double pieces;
	int x,y;
	
	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			if ( gameboard[i][j] == "B" ) {
				dif += V[i][j];
				blacktiles++;
			}
			else if ( gameboard[i][j] == "W" ) {
				dif -= V[i][j];
				whitetiles++;
			}
			if ( gameboard[i][j] != "-" ) {
				for (int k=0; k<boardsize; k++) {
					x = i + X1[k];
					y = j + Y1[k];
					if ( x >= 0 && x < boardsize && y >= 0 && y < boardsize && gameboard[x][y] == "-" ) {
						if ( gameboard[i][j] == "B" ) bfronttiles++;
						else wfronttiles++;
						break;
					}
				}
			}
		}
	}
	
	if ( blacktiles > whitetiles )
		pieces = ( 100 * blacktiles ) / ( blacktiles + whitetiles );
	else if ( blacktiles < whitetiles )
		pieces = -( 100 * whitetiles ) / ( blacktiles + whitetiles );
	else pieces = 0;
	
	if ( bfronttiles > wfronttiles )
		frontiers = -( 100 * bfronttiles ) / ( bfronttiles + wfronttiles );
	else if ( bfronttiles < wfronttiles )
		frontiers = ( 100 * wfronttiles ) / ( bfronttiles + wfronttiles );
	else frontiers = 0;
	
	// CORNER OCCUPANCY
	double corners; 
	
	blacktiles = 0;
	whitetiles = 0;
	
	if ( gameboard[0][0] == "B" )  blacktiles;
	else if ( gameboard[0][0] == "W" ) whitetiles;
	if ( gameboard[0][boardsize-1] == "B" ) blacktiles;
	else if ( gameboard[0][boardsize-1] == "W" ) whitetiles;
	if ( gameboard[boardsize-1][0] == "B" ) blacktiles++;
	else if ( gameboard[boardsize-1][0] == "W" ) whitetiles++;
	if ( gameboard[boardsize-1][boardsize-1] == "B" ) blacktiles++;
	else if ( gameboard[boardsize-1][boardsize-1] == "W" ) whitetiles++;
	
	corners = 25 * ( blacktiles - whitetiles );
	
	// CORNER CLOSENESS
	double nearcorners; 
	
	blacktiles = 0;
	whitetiles = 0;
	
	if ( gameboard[0][0] == "-" ) {
		if ( gameboard[0][1] == "B" ) blacktiles++;
		else if ( gameboard[0][1] == "W" ) whitetiles++;
		if ( gameboard[1][1] == "B" ) blacktiles++;
		else if ( gameboard[1][1] == "W" ) whitetiles++;
		if ( gameboard[1][0] == "B" ) blacktiles++;
		else if ( gameboard[1][0] == "W" ) whitetiles++;
	}
	else if ( gameboard[0][boardsize-1] == "-" ) {
		if ( gameboard[0][boardsize-2] == "B" ) blacktiles++;
		else if ( gameboard[0][boardsize-2] == "W" ) whitetiles++;
		if ( gameboard[1][boardsize-2] == "B" ) blacktiles++;
		else if ( gameboard[1][boardsize-2] == "W" ) whitetiles++;
		if ( gameboard[1][boardsize-1] == "B" ) blacktiles++;
		else if ( gameboard[1][boardsize-1] == "W" ) whitetiles++;
	}
	else if ( gameboard[boardsize-1][0] == "-" ) {
		if ( gameboard[boardsize-1][1] == "B" ) blacktiles++;
		else if ( gameboard[boardsize-1][1] == "W" ) whitetiles++;
		if ( gameboard[boardsize-2][1] == "B" ) blacktiles++;
		else if ( gameboard[boardsize-2][1] == "W" ) whitetiles++;
		if ( gameboard[boardsize-2][0] == "B" ) blacktiles++;
		else if ( gameboard[boardsize-2][0] == "W" ) whitetiles++;
	}
	else if ( gameboard[boardsize-1][boardsize-1] == "-" ) {
		if ( gameboard[boardsize-2][boardsize-1] == "B" ) blacktiles++;
		else if ( gameboard[boardsize-2][boardsize-1] == "W" ) whitetiles++;
		if ( gameboard[boardsize-2][boardsize-2] == "B" ) blacktiles++;
		else if ( gameboard[boardsize-2][boardsize-2] == "W" ) whitetiles++;
		if ( gameboard[boardsize-1][boardsize-2] == "B" ) blacktiles++;
		else if ( gameboard[boardsize-1][boardsize-2] == "W" ) whitetiles++;
	}
	
	nearcorners = -12.5 * ( blacktiles - whitetiles );
	
	// MOBILITY
	double mobility; 
	
	findAvailableSpot(gameboard, "B", bdisk);
	blacktiles = avspot.size();
	findAvailableSpot(gameboard, "W", wdisk);
	whitetiles = avspot.size();
	
	if ( blacktiles > whitetiles )
		mobility = ( 100 * blacktiles ) / ( blacktiles + whitetiles );
	else if ( blacktiles < whitetiles )
		mobility = -(100 * whitetiles) / ( blacktiles + whitetiles );
	
	// FINAL WEIGHTED SCORE
	double score = (10 * pieces) + (801.724 * corners) + (382.026 * nearcorners) + (78.922 * mobility) + (74.396 * frontiers) + (10 * dif);
	return score;
}

int minimax(string gameboard[boardsize][boardsize], int depth, string disk) { // NOT CHECKED
	
	if ( depth == 0 ) return evaluate(gameboard); // nyantol di depth == 1
	else {
		
		if ( disk == "B" ) {
			
			bestscore = numeric_limits<int>::min();
			findAvailableSpot(gameboard, disk, bdisk);
			
			for (auto i=avspot.begin(); i!=avspot.end(); i++) {
				
				string newboard[boardsize][boardsize];
				cloneBoard(newboard, gameboard);
				insertNewDisk(newboard, "B", bdisk, i->first, i->second);
				int v = minimax(newboard, depth-1, "W");
				
				if ( v > bestscore ) {
					bestscore = v;
					bestx = i->first;
					besty = i->second;
				}
		
			}
			
			return bestscore;
		}
		else if ( disk == "W" ) {
			
			bestscore = numeric_limits<int>::max();
			findAvailableSpot(gameboard, disk, wdisk);
			
			for (auto i=avspot.begin(); i!=avspot.end(); i++) {
				
				string newboard[boardsize][boardsize];
				cloneBoard(newboard, gameboard);
				insertNewDisk(newboard, "W", wdisk, i->first, i->second);
				int v = minimax(newboard, depth-1, "B");
				
				if ( v < bestscore ) {
					bestscore = v;
					bestx = i->first;
					besty = i->second;
				}
				
			}
			
			return bestscore;
		}
		
	}
}

void greedyTurn() {
	
}

int main() {
	
	int count = 1;
	int x,y;
	
	initializeBoard();
	
	while (true) {
		
//		system("CLS");
//		gotoxy(0,0);
		printBoard();
		cout << "WDISK" << endl;
		printVector(wdisk);
		cout << "BDISK" << endl;
		printVector(bdisk);
		if ( count % 2 == 0 ) {
			
			cout << "W TURN (AVAILABLE SPOTS)" << endl;
			
			findAvailableSpot(board, "W", wdisk);
			printVector(avspot);
			
			cout << "INPUT X , Y" << endl;
			cin >> x >> y;
			
			insertNewDisk(board, "W", wdisk, x, y);
			
		} else {
			
//			bestscore = minimax(board, 3, "B");
//			
//			insertNewDisk(board, "B", bdisk, bestx, besty);
//			
//			cout << "B FILL " << bestx << " , " << besty;
			
			cout << "B TURN (AVAILABLE SPOTS)" << endl;
			
			findAvailableSpot(board, "B", bdisk);
			printVector(avspot);
			
			cout << "INPUT X , Y" << endl;
			cin >> x >> y;
			
			insertNewDisk(board, "B", bdisk, x, y);
			
		}
		count++;
		
		getchar();
	}
	
}
