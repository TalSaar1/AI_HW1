#include "glut.h"
#include <time.h>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;

const int MSZ = 100; // maze size

enum types { WALL, SPACE, START, TARGET, BLACK, GRAY, TARGET_GRAY, PATH };

int maze[MSZ][MSZ] = { 0 };
vector<Cell*> grays;
vector<Cell*> targetGrays;
bool startBFS = false;

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
	grays.push_back(ps);

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
				glColor3d(0.5, 0, 0.5); // 
				break;
			case PATH:
				glColor3d(0, 0.7, 0); // green
				break;
			}
			// now show the cell as plygon (square)
			glBegin(GL_POLYGON);
			glVertex2d(j, i); // left-bottom corner
			glVertex2d(j, i+1); // left-top corner
			glVertex2d(j+1, i+1); // right-top corner
			glVertex2d(j+1, i); // right-bottom corner
			glEnd();
		}
	}
}

void restorePath(Cell* ps, Cell* pt)
{
	while (ps != nullptr)
	{
		maze[ps->getRow()][ps->getCol()] = PATH;
		ps = ps->getParent();
	}

	while (pt != nullptr)
	{
		maze[pt->getRow()][pt->getCol()] = PATH;
		pt = pt->getParent();
	}
}

// gets the pointer to the current Cell and the coordinates of its neighbor
void checkNeighbor(Cell* pCurrent, int row, int col)
{
	Cell* pTarget;

	// check if the niegbor is not a TARGET
	if (maze[row][col] == TARGET_GRAY)
	{
		startBFS = false;
		cout << "The solution has been found\n";
		do
		{
			pTarget = *(targetGrays.begin());
			targetGrays.erase(targetGrays.begin());
		} while (pTarget != NULL && (pTarget->getRow() != row || pTarget->getCol() != col));
		restorePath(pCurrent, pTarget);
	}
	else // paint this neighbor GRAY
	{
		Cell* ps = new Cell(row, col, pCurrent);
		grays.push_back(ps);
		maze[row][col] = GRAY;
	}
}

void checkNeighborTarget(Cell* pCurrent, int row, int col)
{
	Cell* pTarget;

	// check if the niegbor is not a TARGET
	if (maze[row][col] == GRAY)
	{
		startBFS = false;
		cout << "The solution has been found\n";
		do
		{
			pTarget = *(grays.begin());
			grays.erase(grays.begin());
		} while (pTarget != NULL && (pTarget->getRow() != row || pTarget->getCol() != col));
		restorePath(pTarget, pCurrent);
	}
	else // paint this neighbor GRAY
	{
		Cell* ps = new Cell(row, col, pCurrent);
		targetGrays.push_back(ps);
		maze[row][col] = TARGET_GRAY;
	}
}

void BFSIteration() 
{
	Cell* pCurrent;
	int row, col; // current row and col

	Cell* pTarget;
	int rowTarget, colTarget; // current row and col

	// check the grays queue
	if (grays.empty() || targetGrays.empty()) // nothing to do
	{
		startBFS = false;
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

		// pick and remove from grays the first Cell and check its neighbors
		pTarget = *(targetGrays.begin());
		targetGrays.erase(targetGrays.begin());
		// paint it black
		rowTarget = pTarget->getRow();
		colTarget = pTarget->getCol();
		maze[rowTarget][colTarget] = BLACK;

		// now scan all the white [or target] neighbors and add them (if it's not a target) to Grays
		// check UP
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET_GRAY)
			checkNeighbor(pCurrent, row + 1, col);
		// check DOWN
		if (startBFS && (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET_GRAY))
			checkNeighbor(pCurrent, row - 1, col);
		// check LEFT
		if (startBFS && (maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET_GRAY))
			checkNeighbor(pCurrent, row , col - 1);
		// check RIGHT
		if (startBFS && (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET_GRAY))
			checkNeighbor(pCurrent, row , col + 1);

		// now scan all the white [or target] neighbors and add them (if it's not a target) to Grays
		// check UP
		if (maze[rowTarget + 1][colTarget] == SPACE || maze[rowTarget + 1][colTarget] == GRAY)
			checkNeighborTarget(pTarget, rowTarget + 1, colTarget);
		// check DOWN
		if (startBFS && (maze[rowTarget - 1][colTarget] == SPACE || maze[rowTarget - 1][colTarget] == GRAY))
			checkNeighborTarget(pTarget, rowTarget - 1, colTarget);
		// check LEFT
		if (startBFS && (maze[rowTarget][colTarget - 1] == SPACE || maze[rowTarget][colTarget - 1] == GRAY))
			checkNeighborTarget(pTarget, rowTarget, colTarget - 1);
		// check RIGHT
		if (startBFS && (maze[rowTarget][colTarget + 1] == SPACE || maze[rowTarget][colTarget + 1] == GRAY))
			checkNeighborTarget(pTarget, rowTarget, colTarget + 1);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	
	showMaze();

	glutSwapBuffers(); // show all
}

void idle()
{
	if (startBFS)
		BFSIteration();

	glutPostRedisplay(); // indirect call to refresh function (display)
}

void menu(int choice)
{
	if (choice == 1) // BFS
		startBFS = true;
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
	glutAddMenuEntry("Run BFS", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop(); // starts window queue of events
}