#include <iostream>
#include <string>
#include <windows.h>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <limits>
#include <ctime>

using namespace std;

	// MAX == BLACK
	// MIN == WHITE

	const int boardsize = 8;
	string board[boardsize][boardsize];
	
	vector <pair<int,int>> bdisk;
	vector <pair<int,int>> wdisk;
	
	int bestscore;
	int bestx;
	int besty;

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

void printBoard(string gameboard[boardsize][boardsize]) {
	cout << endl << endl;
	for (int i=0; i<boardsize; i++) {
		for (int j=0; j<boardsize; j++) {
			cout << setw(5) << gameboard[i][j] << " ";
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
	
//	memcpy( newboard, gameboard, boardsize * boardsize * sizeof(gameboard) );

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

void checkDirection(string gameboard[boardsize][boardsize], vector<pair<int,int>> &avspot, string disk, int y, int x, int vy, int vx) {
	
	string enemy = checkEnemy(disk);
	
	if ( x < 0 || x > boardsize-1 || y < 0 || y > boardsize-1 ) {
		return;
	} else if ( gameboard[y][x] == "-" ) {
		avspot.push_back( make_pair(x,y) );
	} else if ( gameboard[y][x] == enemy ){
		x += vx;
		y += vy;
		checkDirection(gameboard, avspot, disk,y,x,vy,vx);
	}
//	cout << "CHECK DIRECTION DONE" << endl;
}

void findAvailableSpot(string gameboard[boardsize][boardsize], vector<pair<int,int>> &avspot, string disk, vector <pair<int,int>> &diskplace) {
	
	avspot.clear();
	refreshDiskplace(gameboard);
	string enemy = checkEnemy(disk);
	
	for (auto i=diskplace.begin(); i!=diskplace.end(); i++) { // if corner
		if ( i->second-1 > 0 && gameboard[i->second-1][i->first] == enemy ) checkDirection(gameboard, avspot, disk, i->second-1, i->first, -1, 0); //NORTH
		
		if ( i->first+1 < boardsize && gameboard[i->second][i->first+1] == enemy ) checkDirection(gameboard, avspot, disk, i->second, i->first+1, 0, 1); //EAST
		
		if ( i->second+1 < boardsize && gameboard[i->second+1][i->first] == enemy ) checkDirection(gameboard, avspot, disk, i->second+1, i->first, 1, 0); //SOUTH
		
		if ( i->first-1 > 0 && gameboard[i->second][i->first-1] == enemy ) checkDirection(gameboard, avspot, disk, i->second, i->first-1, 0, -1); //WEST
		
		if ( i->first-1 > 0 && i->second-1 > 0 && gameboard[i->second-1][i->first-1] == enemy ) checkDirection(gameboard, avspot, disk, i->second-1, i->first-1, -1, -1); //NORTHWEST
			
		if ( i->second-1 > 0 && i->first+1 < boardsize && gameboard[i->second-1][i->first+1] == enemy ) checkDirection(gameboard, avspot, disk, i->second-1, i->first+1, -1, 1); //NORTHEAST
		
		if ( i->second+1 < boardsize && i->first+1 < boardsize && gameboard[i->second+1][i->first+1] == enemy ) checkDirection(gameboard, avspot, disk, i->second+1, i->first+1, 1, 1); //SOUTHEAST
			
		if ( i->second+1 < boardsize && i->first-1 > 0 && gameboard[i->second+1][i->first-1] == enemy ) checkDirection(gameboard, avspot, disk, i->second+1, i->first-1, 1, -1); //SOUTHWEST
			
	}
	sort(avspot.begin(), avspot.end());
	avspot.erase( unique(avspot.begin(), avspot.end()), avspot.end() );
	
//	cout << "FIND AVAILABLE SPOT DONE" << endl;
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
			
			if ( (x < i->first && y < i->second) || ( x > i->first && y > i->second ) ) { // NORTHWEST & SOUTHEAST
				smallx = min(x, i->first);
				smally = min(y, i->second);
				bigx = max(x, i->first);
				bigy = max(y, i->second);
				
				for (smallx, smally; smallx!=bigx && smally!=bigy; smallx++, smally++) {
				
					if ( gameboard[smally][smallx] == "-" ) break;
					else if ( gameboard[smally][smallx] == enemy ) {
						gameboard[smally][smallx] = disk;
					}
				}
			}
			if ( x < i->first && y > i->second) { // SOUTHWEST
				smallx = x;
				smally = i->second;
				bigx = i->first;
				bigy = y;
				
				for (bigx, smally; bigx!=smallx && smally!=bigy; bigx--, smally++) {
					if ( gameboard[smally][bigx] == "-" ) break;
					else if ( gameboard[smally][bigx] == enemy ) {
						gameboard[smally][bigx] = disk;
					}
				}
			}
			if ( x > i->first && y < i->second) { // NORTHEAST
				smallx = i->first;
				smally = y;
				bigx = x;
				bigy = i->second;
				
				for (smallx, bigy; smallx!=bigx && bigy!=smally; smallx++, bigy--) {
					if ( gameboard[bigy][smallx] == "-" ) break;
					else if ( gameboard[bigy][smallx] == enemy ) {
						gameboard[bigy][smallx] = disk;
					}
				}
			}
			
		}
		
		else if ( x == i->first ) {
			
			smally = min(y, i->second);
			bigy = max(y, i->second);
			
			for (smally; smally!=bigy; smally++) {
				
				if ( gameboard[smally][i->first] == "-" ) break;
				else if ( gameboard[smally][i->first] == enemy ) {
					gameboard[smally][i->first] = disk;
				}
			}
		}
		
		else if ( y == i->second ) {
			
			smallx = min(x, i->first);
			bigx = max(x, i->first);
			
			for (smallx; smallx!=bigx; smallx++) {
				
				if ( gameboard[i->second][smallx] == "-" ) break;
				else if ( gameboard[i->second][smallx] == enemy ) {
					gameboard[i->second][smallx] = disk;
				}
			}
		}
	}
//	cout << "TURN ENEMY DISK DONE" << endl;
}

void insertNewDisk(string gameboard[boardsize][boardsize], vector<pair<int,int>> &avspot, string disk, vector<pair<int,int>> &diskplace, int x, int y) { 
	
	string enemy = checkEnemy(disk);
	
	for (auto i=avspot.begin(); i!=avspot.end(); i++) {
		if ( i->first == x && i->second == y ) {
			gameboard[y][x] =  disk;
			turnEnemyDisk(gameboard, disk, diskplace, x, y);
		}
	}
	
//	cout << "INSERT NEW DISK DONE" << endl;
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
	
	vector<pair<int,int>> avspot;
	
	findAvailableSpot(gameboard, avspot, "B", bdisk);
	blacktiles = avspot.size();
	findAvailableSpot(gameboard, avspot, "W", wdisk);
	whitetiles = avspot.size();
	
	if ( blacktiles > whitetiles )
		mobility = ( 100 * blacktiles ) / ( blacktiles + whitetiles );
	else if ( blacktiles < whitetiles )
		mobility = -(100 * whitetiles) / ( blacktiles + whitetiles );
	
	// FINAL WEIGHTED SCORE
	double score = (10 * pieces) + (801.724 * corners) + (382.026 * nearcorners) + (78.922 * mobility) + (74.396 * frontiers) + (10 * dif);
	return score;
}

int minimax(string gameboard[boardsize][boardsize], vector<pair<int,int>> &avspot, int depth, string disk) { 
	
	if ( depth == 0 ) return evaluate(gameboard);
	
	else {
		
		if ( disk == "B" ) {
			//max
			bestscore = numeric_limits<int>::min();
			findAvailableSpot(gameboard, avspot, "B", bdisk);
			for (int i=0; i<avspot.size(); i++) {
				
				string newboard[boardsize][boardsize];
				cloneBoard(newboard, gameboard);
				vector <pair<int,int>> newavspot;
				
				auto it = avspot.begin() + i;
				
				insertNewDisk(newboard, newavspot, "B", bdisk, it->first, it->second);
				
				int v = minimax(newboard, newavspot, depth-1, "W");
				
				if ( v > bestscore ) {
					bestscore = v;
					bestx = it->first;
					besty = it->second;
				}
				
			}
			return bestscore;
		}
		else if ( disk == "W" ) {
			//min
			bestscore = numeric_limits<int>::max();
			findAvailableSpot(gameboard, avspot, "W", wdisk);
			for (int i=0; i<avspot.size(); i++) {
				
				string newboard[boardsize][boardsize];
				cloneBoard(newboard, gameboard);
				vector <pair<int,int>> newavspot;
				
				auto it = avspot.begin() + i;
				
				insertNewDisk(newboard, newavspot, "W", wdisk, it->first, it->second);
				
				int v = minimax(newboard, newavspot, depth-1, "B");
				
				if ( v < bestscore ) {
					bestscore = v;
					bestx = it->first;
					besty = it->second;
				}
				
			}
			return bestscore;
		}
		
	}

}

void greedy(string gameboard[boardsize][boardsize], vector<pair<int,int>> &avspot, string disk, vector<pair<int,int>> &diskplace) {
	
	bestscore = 0;
	findAvailableSpot(gameboard, avspot, disk, diskplace);
	
	for (auto i=avspot.begin(); i!=avspot.end(); i++) {
		
		int diskcount = 0;
		string newboard[boardsize][boardsize];
		cloneBoard(newboard, gameboard);
		
		insertNewDisk(newboard, avspot, disk, diskplace, i->first, i->second);
		
		for (int i=0; i<boardsize; i++) {
			for (int j=0; j<boardsize; j++) {
				if ( newboard[i][j] == disk ) diskcount++;
			}
		}
		
		cout << i->first << "-" << i->second << endl;
		cout << diskcount << endl;
		cout << bestscore << endl;
		
		if ( bestscore < diskcount ) {
			bestscore = diskcount;
			bestx = i->first;
			besty = i->second;
		}
	}
	
}

int main() {
	
	vector <pair<int,int>> availspot;
	int count = 1;
	
	initializeBoard();
	
	while (true) {
		
		system("CLS");
		gotoxy(0,0);
		
		
		if ( count % 2 == 0 ) {
			
//			printBoard(board);
//			
//			cout << "W TURN (AVAILABLE SPOTS)" << endl;
//			
//			findAvailableSpot(board, availspot, "W", wdisk);
//			
//			if ( availspot.empty() ) {
//				cout << "NO MOVE AVAILABLE, MOVING ON" << endl;
//			}
//			else {
//				printVector(availspot);
//			
//				cout << "INPUT X , Y" << endl;
//				cin >> x >> y;
//				
//				insertNewDisk(board, availspot, "W", wdisk, x, y);
//			}
//			
//			printBoard(board);

			printBoard(board);
			
			findAvailableSpot(board, availspot, "B", bdisk);
			if ( availspot.empty() ) {
				cout << "NO MOVE AVAILABLE, MOVING ON" << endl;
			}
			else {
				clock_t begin = clock();
			
				greedy(board, availspot, "W", wdisk);
				
				clock_t end = clock();
				double elapsed_secs = double(end-begin) / CLOCKS_PER_SEC;
				
				insertNewDisk(board, availspot, "W", wdisk, bestx, besty);
				
				cout << "GREEDY WHITE FILLED " << bestx << " , " << besty << endl;
				cout << "TIME ELAPSED " << elapsed_secs << " SECONDS" << endl; 
			}
			
			
			printBoard(board);
			
		} else {
			
			printBoard(board);
			
			findAvailableSpot(board, availspot, "B", bdisk);
			if ( availspot.empty() ) {
				cout << "NO MOVE AVAILABLE, MOVING ON" << endl;
			}
			else {
				int depth = 7;
				clock_t begin = clock();
				
				minimax(board, availspot, depth, "B");
				
				clock_t end = clock();
				double elapsed_secs = double(end-begin) / CLOCKS_PER_SEC;
			
				insertNewDisk(board, availspot, "B", bdisk, bestx, besty);
				
				cout << "MINIMAX BLACK FILLED " << bestx << " , " << besty << endl;
				cout << "DEPTH " << depth << endl;
				cout << "TIME ELAPSED " << elapsed_secs << " SECONDS" << endl; 
			}
			
			
			printBoard(board);
			
		}
		count++;
		getchar();
	}
	
}
