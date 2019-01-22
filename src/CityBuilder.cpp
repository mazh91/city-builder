/*******************************************************************
	       Interactive 3D City Modelling Program
********************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "VECTOR3D.h"
#include "cube.h"
#include "TerrainGrid.h"
#include <fstream>
#include <iostream>

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
// Milad's Prototypes
void selectCube(Cube **cubes, int index, const bool is_multi=false);
void cycleCube(Cube **cubes, int *cycle_i, const bool is_multi=false, const bool do_deselect=false);
void deselectMulti(Cube **cubes, int index);
int getSelectedCount(Cube **cubes);
void reinitiateCubes(Cube **cubes);

static int currentButton;

GLfloat light_position0[] = {12.0, 6.0,-12.0,1.0};
GLfloat light_position1[] = {-12.0, 6.0,12.0,1.0};
GLfloat light_diffuse[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_ambient[]   = {0.2, 0.2, 0.2, 1.0};

GLfloat NEAR_PLANE = 0.2;
GLfloat FAR_PLANE = 40.0; //0.2,40.0

// Cubes
// you may want to create a fixed size array of cubes or a fixed size array of pointers to cubes
// and other variables to keep track of the number of current cubes etc.
// see cube.h createCube function
Cube *cubes[25];
int cube_i;				// counter for cubes
int mode = 0;		// transformation mode
int cubeCount = 0;
// Terrain Grid
TerrainGrid *terrainGrid = NULL;

int gridSize = 16;
const long CUBES_CAP = (unsigned)sizeof(cubes)/sizeof(long);
int cycle_i;
int start_i = 0;
Cube *myCube; //[CUBES_CAP]
std::ofstream out;
std::ifstream in;
int in_i = 0, out_i = 0;

GLint viewport[4];
GLdouble modelView[16],
		 projection[16];

GLfloat winX, winY, winZ;
GLdouble posX0, posY0, posZ0;
GLdouble posX, posY, posZ;
bool doGetCoordinateValues;
bool enableDragBounds = false;
/**
 * override iostream operators to export/import cube objects
 */
std::istream& operator>>(std::istream& is, Cube& cube)
{
	is >> cube.angle;
	is >> cube.is_selected;
	is >> cube.sfx;
	is >> cube.sfy;
	is >> cube.sfz;
	is >> cube.tx;
	is >> cube.ty;
	is >> cube.tz;
	return is;
}
std::ostream& operator<<(std::ostream& os, Cube& cube)
{
	os << cube.angle << " " << cube.is_selected << " " << cube.sfx << " "
		<< cube.sfy << " " << cube.sfz << " " << cube.tx << " " << cube.ty
		<< " " << cube.tz;
	if (!cube.is_last)
		os << std::endl;
	return os;
}

int main(int argc, char **argv)
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(750, 750);
  glutInitWindowPosition(100, 0);
  glutCreateWindow("City Builder");
  initOpenGL(750,750);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotionHandler);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(functionKeys);
  glutMainLoop();
  return 0;
}

// Setup openGL */
void initOpenGL(int w, int h)
{
  // Set up viewport, projection, then change to modelview matrix mode - 
  // display function will then set up camera and modeling transforms
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  gluPerspective(60.0,1.0,0.2,80.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set up and enable lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_REPLACE);

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.6, 0.6, 0.6, 0.0);  
  glClearDepth(1.0f);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);
  glEnable(GL_NORMALIZE);
  
  // Set up Terrain Grid
  VECTOR3D origin = VECTOR3D(-8.0f,0.0f,8.0f);
  terrainGrid = new TerrainGrid(gridSize, 16.0);	// size of terrain is therefore 16.0
  terrainGrid->InitGrid(gridSize, origin, 16.0, 16.0);
  // Milad
  doGetCoordinateValues = true;
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  // Set up fixed camera position and where it is looking, it's orientation
  gluLookAt(-12.0,6.0,12.0,0.0,0.0,0.0,0.0,1.0,0.0);
  // Add code to draw all cubes currently in your array
  // see cube.h drawCube()
  for (int i=0; i<(int)CUBES_CAP; i++)
  {
	  if(cubes[i])
	  {
		  glPushMatrix();
		  drawCube(cubes[i]);
		  glPopMatrix();
	  }
  }
  glDisable(GL_COLOR_MATERIAL);
  // Draw Terrain
  	  terrainGrid->DrawGrid(gridSize);
  glutSwapBuffers();
}


// Called at initialization and whenever user resizes the window */
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,NEAR_PLANE, FAR_PLANE); // ,0.2,40.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

VECTOR3D pos = VECTOR3D(0,0,0);
/**
 * Obtain mouse position unprojected onto the world coordinates [2]
 */
void mouse(int button, int state, int x, int y)
{
  currentButton = button;

  switch(button)
  {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN)
	{
    	// get window coordinates
    	// Get viewport coordinates
    	if (doGetCoordinateValues)
    	{
			glGetIntegerv(GL_VIEWPORT, viewport);
			// Get model view coordinates
			glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
			// Get projection matrix
			glGetDoublev(GL_PROJECTION_MATRIX, projection);
    	}
    	winX = (float)x;
    	winY = (float)viewport[3] - (float)y;
    	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    	gluUnProject(winX, winY, winZ, modelView, projection, viewport,&posX0, &posY0, &posZ0);

    	for (int i=0; i<CUBES_CAP; i++)
    	{
			if (cubes[i] && cubes[i]->is_selected)
			{
				myCube = cubes[i];
			}
    	}
    	doGetCoordinateValues = false;
	}
	break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN)
	{
	  
	}
	break;
  default:
	break;
  }
  glutPostRedisplay();
}


void mouseMotionHandler(int xMouse, int yMouse)
{
  if (currentButton == GLUT_LEFT_BUTTON)
  {
	winX = (float)xMouse;
	winY = (float)viewport[3] - (float)yMouse;
	glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject(winX, winY, winZ, modelView, projection, viewport,&posX, &posY, &posZ);
	// debug
    //std::cout << posX - posX0 << " " << posY - posY0 << " " << posZ - posZ0 << std::endl;
	if (myCube)
	{
		if (enableDragBounds)
		{
			if (labs(posX-posX0) < 7.f &&  labs(posZ-posZ0) < 7.f)
			{
				myCube->tx = posX - posX0;
				myCube->ty = posY - posY0;
				myCube->tz = posZ - posZ0;
			}
		}
		else
		{
			myCube->tx = posX - posX0;
			myCube->ty = posY - posY0;
			myCube->tz = posZ - posZ0;
		}
	}
  }
  glutPostRedisplay();
}


/* Handles input from the keyboard, non-arrow keys */
void keyboard(unsigned char key, int x, int y)
{
  switch (key) 
  {
  case 't':
      mode = 1;
	  break;
  case 's':
	  mode = 2;
	  break;
  case 'r':
	  mode = 3;
	  break;
  case 'h':
	  mode = 4;
	  break;
  case 'c':		// cycle
	  std::cout << cycle_i << std::endl;
	  cycleCube(cubes, &cycle_i);
	  break;
  case 'q':
	  std::cout << "Quit" << std::endl;
	  exit(0);
	  break;
  case '=':
  case '+':
	  cycleCube(cubes, &cycle_i, true);
	  break;
  case '_':
  case '-':
	  cycleCube(cubes, &cycle_i, true, true);
	  break;
  case 'l':
	  // Import cube objects [1]
	  in.open("obj");
	  // Discard current state
	  reinitiateCubes(cubes);
	  while (!in.fail() && !in.eof())
	  {
		  cubes[cube_i] = createCube();
		  in >> *(cubes[cube_i++]);
	  }
	  // Remove dummy cube
	  cube_i --;
	  cubes[cube_i] = NULL;
	  cycle_i = getCubeCount(cubes)-1;		// add?
	  break;
  case 'w':
	  // Export cube Objects [1]
	  std::cout << "Writing object states.." << std::endl;
	  out.open("obj");
	  cubeCount = getCubeCount(cubes);
	  while (out_i < cubeCount)
	  {
		  if (out_i >= cubeCount)
			  cubes[out_i]->is_last = true;
		  out << *(cubes[out_i++]);
	  }
	  out.close();
	  std::cout << "Done" << std::endl;
	  break;
  default: break;
  }
  glutPostRedisplay();
}

void functionKeys(int key, int x, int y)
{
  if (key == GLUT_KEY_F1)
  {
	  // Create and initialize new cube
    // add to your array see cube.h
	  cubes[cube_i] = createCube();
	  selectCube(cubes, cube_i);
	  // Set cycle index to last created cube
	  cycle_i = cube_i;
	  // Increment cubes index
	  if (cube_i < CUBES_CAP)
		 cube_i++;
  }

    // Arrow keys
    switch(key)
    {
      case GLUT_KEY_DOWN:
    	  switch(mode)
		  {
			case 1:		// translate
				for (int i=0; i<CUBES_CAP; i++)
					if (cubes[i] && cubes[i]->is_selected)
						cubes[i]->ty -= 0.25f;
				break;
			case 2:		// scale
				for (int i=0; i<CUBES_CAP; i++)
					if (cubes[i] && cubes[i]->is_selected)
					{
						cubes[i]->sfy -= 0.25f;
						cubes[i]->sfz -= 0.25f;
					}
				break;
			case 4:		// extrusion
				for (int i=0; i<CUBES_CAP; i++)
					if (cubes[i] && cubes[i]->is_selected && cubes[i]->sfy > 0.25f)
						cubes[i]->sfy -= 0.25f;
				break;
			default:
				break;
		  }
        break;
      case GLUT_KEY_UP:
      	switch(mode)
      	{
      	case 1:
			for (int i=0; i<CUBES_CAP; i++)
				if (cubes[i] && cubes[i]->is_selected)
					cubes[i]->ty += 0.25f;
      		break;
      	case 2:
			for (int i=0; i<CUBES_CAP; i++)
				if (cubes[i] && cubes[i]->is_selected)
				{
					cubes[i]->sfy += 0.25f;
					cubes[i]->sfz += 0.25f;
				}
      		break;
      	case 4:
			for (int i=0; i<CUBES_CAP; i++)
				if (cubes[i] && cubes[i]->is_selected)
					cubes[i]->sfy += 0.25f;
    		break;
      	default:
      		break;
    	}
        break;
      case GLUT_KEY_RIGHT:
		  switch(mode)
		  {
			case 1:
				for (int i=0; i<CUBES_CAP; i++)
					if (cubes[i] && cubes[i]->is_selected)
						cubes[i]->tx += 0.25f;
				break;
			case 3:		// Rotate
				for (int i=0; i<CUBES_CAP; i++)
					if (cubes[i] && cubes[i]->is_selected)
						cubes[i]->angle -= 5.f;
				break;
			default:
				break;
			}
			break;
      case GLUT_KEY_LEFT:
    	  switch(mode)
    	  {
			case 1:
				for (int i=0; i<CUBES_CAP; i++)
					if (cubes[i] && cubes[i]->is_selected)
						cubes[i]->tx -= 0.25f;
				break;
			case 3:
				for (int i=0; i<CUBES_CAP; i++)
					if (cubes[i] && cubes[i]->is_selected)
						cubes[i]->angle += 5.f;
				break;
			default:
				break;
		}
			break;
			  default:
        break;
    }
  glutPostRedisplay();
}


/**
 * Select a single cube and make sure all others are not selected
 */
void selectCube(Cube **cubes, int index, const bool is_multi)
{
	if (!is_multi)
	{
		for (int i=0; i<CUBES_CAP; i++)
			if (cubes[i])
				cubes[i]->is_selected = false;
	}
	cubes[index]->is_selected = true;
}

/**
 * Cycle through the cubes and select the next one
 */
void cycleCube(Cube **cubes, int *cycle_i, const bool is_multi,
		const bool do_deselect)
{
	if (do_deselect)
	{
		// boundary check
		if (getSelectedCount(cubes) < 1)
			return;

		deselectMulti(cubes, *cycle_i);
		(*cycle_i)++;

		if (!(*cycle_i < getCubeCount(cubes)))
			*cycle_i = 0;
	}
	else
	{
		// boundary check
		if (getSelectedCount(cubes) >= getCubeCount(cubes))
			return;
		if (*cycle_i > 0)
			(*cycle_i)--;
		else
			*cycle_i = getCubeCount(cubes) - 1;
		selectCube(cubes, *cycle_i, is_multi);
	}
}
/**
 * Deselect last multiple-selected cube
 */
void deselectMulti(Cube **cubes, int index)
{
	cubes[index]->is_selected = false;
}
/**
 * Returns count of selected cubes
 */
int getSelectedCount(Cube **cubes)
{
	int count = 0;
	for (int i=0; i<CUBES_CAP; i++)
		if (cubes[i] && cubes[i]->is_selected)
			count++;
	return count;
}

void reinitiateCubes(Cube **cubes)
{
	for (int i = 0; i < CUBES_CAP; i++)
		cubes[i] = new Cube;
}
