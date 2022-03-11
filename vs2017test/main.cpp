/*
	Names:
	Elad Shoham - 206001752
	Tal Saar - 209151380
*/

#include <iostream>
#include <vector>
#include <time.h>
#include "glut.h"
#include "Cell.h"

using namespace std;

const int MSZ = 100; // maze size

enum types { WALL, SPACE, START, TARGET, BLACK, GRAY, TARGET_GRAY, PATH };
enum choice { BIDIRECTIONAL_SEARCH };

int maze[MSZ][MSZ] = { 0 };
vector<Cell*> startGrays;
vector<Cell*> targetGrays;
bool startBidirectionalSearch = false;

void initMaze()
{
	int i, j, rowTarget, colTarget;

	for (i = 1; i < MSZ - 1; i++)
	{
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 0) // mostly walls
				maze[i][j] = rand() % 100 > 35 ? WALL : SPACE;
			else // mostly spaces
				maze[i][j] = rand() % 100 > 20 ? SPACE : WALL;
		}
	}

	maze[MSZ / 2][MSZ / 2] = START;
	Cell* ps = new Cell(MSZ / 2, MSZ / 2, nullptr);
	startGrays.push_back(ps);

	rowTarget = rand() % MSZ;
	colTarget = rand() % MSZ;
	maze[rowTarget][colTarget] = TARGET;
	Cell* pt = new Cell(rowTarget, colTarget, nullptr);
	targetGrays.push_back(pt);
}

void init()
{	//          Red Green Blue 
	glClearColor(0, 0, 0.4, 0);// color of window background
	
	// set the main axes
	glOrtho(0, MSZ, 0, MSZ, -1, 1); 

	srand(time(NULL));

	initMaze();
}

void showMaze() 
{
	int i, j;

	for (i = 0; i < MSZ; i++)
	{
		for (j = 0; j < MSZ; j++)
		{
			// set color for cell (i,j)
			switch (maze[i][j]) 
			{
			case WALL:
				glColor3d(0.5, 0, 0); // dark red
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case START:
				glColor3d(0.5, 0.5, 1); // light blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case BLACK:
				glColor3d(1, 1, 0); // yellow
				break;
			case GRAY:
				glColor3d(1, 0, 1); // magenta
				break;
			case TARGET_GRAY:
				glColor3d(0.2, 0.2, 0.2); // dark gray
				break;
			case PATH:
				glColor3d(0, 0.7, 0); // green
				break;
			}
			// now show the cell as plygon (square)
			glBegin(GL_POLYGON);
			glVertex2d(j, i); // left-bottom corner
			glVertex2d(j, i + 1); // left-top corner
			glVertex2d(j + 1, i + 1); // right-top corner
			glVertex2d(j + 1, i); // right-bottom corner
			glEnd();
		}
	}
}

void restoreBFSPath(Cell* ps)
{
	while (ps != nullptr)
	{
		maze[ps->getRow()][ps->getCol()] = PATH;
		ps = ps->getParent();
	}
}

void restorePath(Cell* ps, vector<Cell*> &collisionGrays, int row, int col)
{
	Cell* pt = nullptr;

	restoreBFSPath(ps);

	do
	{
		pt = *(collisionGrays.begin());
		collisionGrays.erase(collisionGrays.begin());
	} while (pt != NULL && (pt->getRow() != row || pt->getCol() != col));

	restoreBFSPath(pt);
}

// gets the pointer to the current Cell and the coordinates of its neighbor
void checkNeighbor(Cell* pCurrent, vector<Cell*> &grays, vector<Cell*> &collisionGrays, int row, int col, int color, int target)
{
	// check if the niegbor is not a TARGET
	if (maze[row][col] == target)
	{
		startBidirectionalSearch = false;
		cout << "The solution has been found\n";
		restorePath(pCurrent, collisionGrays, row, col);
	}
	else // paint this neighbor GRAY
	{
		grays.push_back(new Cell(row, col, pCurrent));
		maze[row][col] = color;
	}
}

void BFSIteration(vector<Cell*> &grays, vector<Cell*> &collisionGrays, int color, int target)
{
	Cell* pCurrent = nullptr;
	int row, col; // current row and col

	// check the grays queue
	if (grays.empty()) // nothing to do
	{
		startBidirectionalSearch = false;
		cout << "There is no solution\n";
	}
	else
	{
		// pick and remove from grays the first Cell and check its neighbors
		pCurrent = *(grays.begin());
		grays.erase(grays.begin());

		// paint it black
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		maze[row][col] = BLACK;

		// now scan all the white [or target] neighbors and add them (if it's not a target) to Grays
		// check UP
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == target)
			checkNeighbor(pCurrent, grays, collisionGrays, row + 1, col, color, target);
		// check DOWN
		if (startBidirectionalSearch && (maze[row - 1][col] == SPACE || maze[row - 1][col] == target))
			checkNeighbor(pCurrent, grays, collisionGrays, row - 1, col, color, target);
		// check LEFT
		if (startBidirectionalSearch && (maze[row][col - 1] == SPACE || maze[row][col - 1] == target))
			checkNeighbor(pCurrent, grays, collisionGrays, row, col - 1, color, target);
		// check RIGHT
		if (startBidirectionalSearch && (maze[row][col + 1] == SPACE || maze[row][col + 1] == target))
			checkNeighbor(pCurrent, grays, collisionGrays, row, col + 1, color, target);
	}
}

void bidirectionalSearch()
{
	BFSIteration(startGrays, targetGrays, GRAY, TARGET_GRAY);
	BFSIteration(targetGrays, startGrays, TARGET_GRAY, GRAY);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	
	showMaze();

	glutSwapBuffers(); // show all
}

void idle()
{
	if (startBidirectionalSearch)
		bidirectionalSearch();

	glutPostRedisplay(); // indirect call to refresh function (display)
}

void menu(int choice)
{
	if (choice == BIDIRECTIONAL_SEARCH)
		startBidirectionalSearch = true;
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // double buffering for animation
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Bidirectional Search");

	glutDisplayFunc(display); // sets display function as window refresh function
	glutIdleFunc(idle); // runs all the time when nothing happens

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Run Bidirectional Search", BIDIRECTIONAL_SEARCH);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop(); // starts window queue of events
}