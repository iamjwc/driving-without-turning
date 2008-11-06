/*************************************************************/
/* Filename: DrivingWithoutTurning.cpp                       */
/*                                                           */
/* Using OpenGL transformations and keyboard callbacks, this */
/* program renders a first-person traversal of a city        */
/* environment, with the ability to alter the scene being    */
/* viewed (including the weather and the time of day), as    */
/* well as the viewer's speed and the incline of the road    */
/* being traversed.  Relevant data (current speed, distance  */
/* travelled, current weather, time of day, etc.) is         */
/* displayed below the animation, illustrating the values of */
/* these user-controlled variables.                          */
/*************************************************************/

#include <windows.h>	// Header File For Windows
#include <iostream>		// For diagnostic I/O              //
#include <math.h>		// Contains math functions         //
#include <time.h>		// Accesses system time info       //
#include <stdlib.h>		// Enables random number generator //
#include "Font.h"		// Font generation routines        //
#include "glut.h"		// Standard OpenGL utility library //

#include <iomanip>

#include "Graphics.h"
#include "Graphics.Random.h"
#include "Graphics.Transformation.h"

using namespace std;
using namespace Graphics;

enum TOD {dawn,noon,dusk};			// Time-of-day enumerated type       //
enum weather {sunny,rainy,snowy};	// Weather condition enumerated type //
enum SOR {LHS,RHS};					// Side-of-road enumerated type      //

/**********************************/
/* Global constants and variables */
/**********************************/

/* Location of upper left-hand corner display window */
const GLint InitWindowPosition[2] = { 50, 50 };

/* Light source intensity and position */
const GLfloat LightIntensity[] = { 0.8, 0.8, 0.8, 1.0 };
const GLfloat LightPosition[] = { 2.0, 5.0, 2.0, 0.0 };

/* 3-D coordinate boundaries for animated display */
/* (Note: The lack of a maximum z-value indicates */
/* that potential problems could occur once any z */
/* coordinates exceed the system's FLT_MAX value) */
const GLfloat Xmin = -3.5;
const GLfloat Xmax =  3.5;
const GLfloat Ymin = -3.0;
const GLfloat Ymax =  3.0;
const GLfloat Zmin = -4.0;

/* The initial window and viewport sizes (in pixels), along */
/* with the unit aspect ratio for the viewport, assumed to  */
/* be a constant.  When the window is resized, the viewport */
/* will be adjusted to preserve this aspect ratio.          */
GLint currWindowSize[2] = { 700, 600 };
GLint currViewportSize[2] = { 600, 600 };
const GLfloat AspectRatio = 1.0;

/* Initial position of the viewer, along with the position   */
/* increment used to propel the viewer forward with each     */
/* cycle.  This increment has a "delta" that is used to      */
/* increase or decrease the viewer's speed (via keyboard     */
/* operations).  The viewer's speed also has an upper bound. */
GLfloat viewPosition[] = {0.0, 0.0, -5.0};
GLfloat viewIncrement[] = {0.0, 0.0, 0.5};
GLfloat viewIncrementDelta[] = {0.0, 0.0, 0.075};
GLfloat maxViewIncrement[] = {0.0, 0.0, 2.0};

/* The viewIncrement's z-value is multiplied by the SpeedScale */
/* to obtain the viewer's "speed" for display purposes, as     */
/* well for "calculating" the distance travelled.              */
const float SpeedScale = 13.7;
float currentSpeed = 0.5;
float distanceTravelled = 0.0;

/* The incline of the path is simulated by altering    */
/* the direction in which the viewer is looking.  When */
/* the incline is flat, the viewer looks straight      */
/* ahead; when the incline is downhill, the viewer     */
/* looks up from the vertical (from a slightly lower   */
/* position); and when the incline is uphill, the      */
/* viewer looks down from the vertical.                */
float incline = 0.0;
float lookAtYDelta = 0.0;
float lookAtYDeltaIncrement = 0.03;
float maxLookAtYDelta = 0.3;
float positionYDelta = 0.12;

/* Initially, the scene is a clear dawn in the city. */
TOD timeOfDay = dawn;
weather weatherCondition = sunny;

/* When precipitation is active, each drop or flake   */
/* is repositioned according to a specific increment, */
/* and that increment is changed as time progresses,  */
/* with different changes for rain and for snow.      */
float precipIncrement[]    = {0.01, -0.01, 0.0};
float snowIncrementDelta[] = {0.01, -0.1, -0.2};
float rainIncrementDelta[] = {0.01, -0.05, 0.0};

/* Early morning fog is set up to be lightly colored     */
/* and very dense; late evening fog is set up to be      */
/* darker and less dense; and midday fog is nonexistent. */
const GLfloat LightFogColor[] = { 0.9, 0.9, 0.9, 1.0 };
const GLfloat DarkFogColor[] = { 0.6, 0.75, 0.85, 0.9 };
GLfloat fogColor[] = {LightFogColor[0],LightFogColor[1],
                      LightFogColor[2],LightFogColor[3]};
const GLfloat DawnFogDensity = 0.05;
const GLfloat NoonFogDensity = 0.0;
const GLfloat DuskFogDensity = 0.025;
GLfloat fogDensity = DawnFogDensity;

///////////////////////////////////////////////////
// Constants & variables for the cityscape scene //
///////////////////////////////////////////////////
const int     NbrOfRoadIterations      = 10;
const int     NbrOfLinesPerRoadBlock   = 4;
const GLfloat RoadBlockLength          = 20.0;
const GLfloat RoadColor[]              = { 0.1,  0.1,  0.1  };
const GLfloat RoadLineColor[]          = { 1.0,  1.0,  1.0  };
const GLfloat SidewalkColor[]          = { 0.4,  0.4,  0.4  };
const GLfloat LamppostColor[]          = { 0.2,  0.2,  0.2  };
const GLfloat BusStopSignColor[]       = { 0.0,  0.4,  0.6  };
const GLfloat LampOffColor[]           = { 0.5,  0.5,  0.1  };
const GLfloat LampOnColor[]            = { 1.0,  1.0,  0.1  };
const GLfloat TrashcanBarrelColor[]    = { 0.0,  0.2,  0.05 };
const GLfloat TrashcanLidColor[]       = { 1.0,  1.0,  0.9  };
const GLfloat MailboxBarrelColor[]     = { 0.0,  0.05, 0.25 };
const GLfloat MailboxLidColor[]        = { 0.25, 0.05, 0.0  };
const GLfloat NewsstandCaseColor[]     = { 1.0,  1.0,  0.9  };
const GLfloat NewsstandDoorColor[]     = { 0.0,  0.0,  0.0  };

const GLfloat IntersectionBlockScale[] = { 2.0,  0.02,  1.0   };
const GLfloat VerticalCrosswalkScale[] = { 0.02, 0.022, 1.0   };
const GLfloat HorizCrosswalkScale[]    = { 0.22, 0.022, 0.02  };
const GLfloat RoadBlockScale[]         = { 0.2,  0.02,  1.0   };
const GLfloat RoadLineScale[]          = { 0.01, 0.022, 0.125 };
const GLfloat SidewalkScale[]          = { 1.54, 0.05,  1.0   };

const GLfloat CrosswalkDisplacement    = 2.0;
const GLfloat CrosswalkWidth           = 1.5;
const GLfloat SidewalkDisplacement     = 17.0;
const GLfloat LamppostDisplacement     = 1.84;
const GLfloat LamppostPoleHeight       = 5.0;
const GLfloat TrashcanDisplacement     = 1.84;
const GLfloat MailboxDisplacement      = 1.8;
const GLfloat NewsstandDisplacement    = 1.9;
const GLfloat SkyscraperDisplacement   = 13.0;
const GLfloat WindowDisplacement       = 2.9;
const GLfloat StoryHeight              = 20.0;


/////////////////////////////////////////////////
// Constants & variables for the display panel //
/////////////////////////////////////////////////

/* Fonts for use in the display panel. */
GLFONT *TextFont;
GLFONT *SmallTextFont;
GLFONT *MediumTextFont;
GLFONT *LargeTextFont;


/***********************/
/* Function prototypes */
/***********************/
void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition);
void TimerFunction(int value);
void Display();
void ResizeWindow(GLsizei w, GLsizei h);
void DrawCityElements();
void DrawIntersection(GLfloat firstZ);
void DrawRoadCube(GLfloat firstZ, int index);
void DrawSidewalkCubePair(GLfloat firstZ, int index);
void DrawStreetlight(GLfloat firstZ, int index, SOR roadside);
void DrawCityProp(GLfloat firstZ, int index);
void DrawSkyscraper(GLfloat firstZ, int index, SOR roadside);
void DrawCityFarPlaneCube();
void RenderPrecipitation();
void DrawDisplayPanel();
void UpdateFog();
float GenerateRandomNumber(float lowerBound, float upperBound);


/************************************************/
/* The main function sets up the data and the   */
/* environment to display the textured objects. */
/************************************************/
void main()
{
  

    /* Set up the display window. */
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowPosition( InitWindowPosition[0], InitWindowPosition[1] );
    glutInitWindowSize( currWindowSize[0], currWindowSize[1] );
    glutCreateWindow( "Speed: +/-; Time: T/t; Weather: W/w; Incline: I/i" );

    /* Specify the resizing and refreshing routines. */
    glutReshapeFunc( ResizeWindow );
    glutKeyboardFunc( KeyboardPress );
    glutDisplayFunc( Display );
    glutTimerFunc( 100, TimerFunction, 1 );

    /* Set up standard lighting, shading, and depth testing. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.2, 0.7, 0.9, 0.0);
    glViewport(0, 0, currWindowSize[0], currWindowSize[1]);

    /* Enable alpha test for transparency. */
    glAlphaFunc( GL_GREATER, 0.5 );
    glEnable( GL_ALPHA_TEST );
    
    /* Set up all fonts, initializing to medium size. */
    SmallTextFont  = FontCreate(wglGetCurrentDC(), "Arial", 8, 100, 1);
    MediumTextFont = FontCreate(wglGetCurrentDC(), "Arial", 16, 600, 1);
    LargeTextFont  = FontCreate(wglGetCurrentDC(), "Arial", 24, 900, 1);
    TextFont       = MediumTextFont;

    glutMainLoop();
}


/***********************************************************/
/* Function to react to keyboard keys pressed by the user. */
/***********************************************************/
void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition)
{
    int i;
    switch(pressedKey)
    {
    /* Plus sign: Viewer acceleration */
    case '+':	{
                    for (i = 0; i < 3; i++)
                    {
                    viewIncrement[i] += viewIncrementDelta[i];
                    if (viewIncrement[i] > maxViewIncrement[i])
                        viewIncrement[i] = maxViewIncrement[i];
                    }
                    currentSpeed = SpeedScale*viewIncrement[2];
                    if (viewIncrement[2] < maxViewIncrement[2])
                        snowIncrementDelta[2] -= 0.03;
                    break;
                }
    /* Minus sign: Viewer deceleration */
    case '-':	{ 
                    for (i = 0; i < 3; i++)
                    {
                        viewIncrement[i] -= viewIncrementDelta[i];
                        if (viewIncrement[i] < 0.0)
                            viewIncrement[i] = 0.0;
                    }
                    currentSpeed = SpeedScale*viewIncrement[2];
                    if (viewIncrement[2] > 0.0)
                        snowIncrementDelta[2] += 0.03;
                    break;
                }
    /* Lower-case i: Decrease scene inclination */
    case 'i':	{ 
                    if (lookAtYDelta < maxLookAtYDelta)
                    {
                        lookAtYDelta += lookAtYDeltaIncrement;
                        if (lookAtYDelta >= 0.0)
                            viewPosition[1] -= positionYDelta;
                        incline = -66.7*lookAtYDelta;
                    }
                    break; 
                }
    /* Upper-case I: Increase scene inclination */
    case 'I':	{ 
                    if (lookAtYDelta > -maxLookAtYDelta)
                    {
                        lookAtYDelta -= lookAtYDeltaIncrement;
                        if (lookAtYDelta > 0.0)
                            viewPosition[1] += positionYDelta;
                        incline = -66.7*lookAtYDelta;
                    }
                    break; 
                }
    /* Lower-case t: Move time-of-day backwards */
    case 't':	{ 
                    if (timeOfDay == dawn)
                        timeOfDay = dusk;
                    else
                        timeOfDay = TOD(timeOfDay - 1);
                    UpdateFog();
                    break;
                }
    /* Upper-case T: Move time-of-day forwards */
    case 'T':	{ 
                    if (timeOfDay == dusk)
                        timeOfDay = dawn;
                    else
                        timeOfDay = TOD(timeOfDay + 1);
                    UpdateFog();
                    break; 
                }
    /* Lower-case w: "Decrease" weather condition (normal order: sunny, rainy, snowy) */
    case 'w':	{ 
                    if (weatherCondition == sunny)
                        weatherCondition = snowy;
                    else
                        weatherCondition = weather(weatherCondition - 1);
                    break; 
                }
    /* Upper-case W: "Increase" weather condition (normal order: sunny, rainy, snowy) */
    case 'W':	{ 
                    if (weatherCondition == snowy)
                        weatherCondition = sunny;
                    else
                        weatherCondition = weather(weatherCondition + 1);
                    break; 
                }
    }
}


/********************************************************************/
/* Function to update the viewer's position, the far wall location, */
/* and the current position of each element of precipitation.       */
/********************************************************************/
void TimerFunction(int value)
{
    int i;

    for (i = 0; i < 3; i++)
        viewPosition[i] += viewIncrement[i];

    for (i = 0; i < 3; i++)
        if (weatherCondition == snowy)
            precipIncrement[i] += snowIncrementDelta[i];
        else if (weatherCondition == rainy)
            precipIncrement[i] += rainIncrementDelta[i];

    glutPostRedisplay();
    glutTimerFunc(100, TimerFunction, 1);
}


/**********************************************************/
/* Principal display routine: sets up material, lighting, */
/* and camera properties, clears the frame buffer, and    */
/* draws all texture-mapped objects within the window.    */
/**********************************************************/
void Display()
{
    /* Set up the properties of the light source. */
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightIntensity);
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

    /* Limit the animation to above the "control panel". */
    if (AspectRatio > currWindowSize[0]/currWindowSize[1])
    {
        glViewport(0, 0.5*(currWindowSize[1]-currViewportSize[1])+currViewportSize[1]/6, 
                        currViewportSize[0], currViewportSize[1]);
        glScissor(0, 0.5*(currWindowSize[1]-currViewportSize[1])+currViewportSize[1]/6, 
                        currViewportSize[0], currViewportSize[1]);
    }
    else
    {
        glViewport(0.5*(currWindowSize[0]-currViewportSize[0]), currViewportSize[1]/6, 
                        currViewportSize[0], currViewportSize[1]);
        glScissor(0.5*(currWindowSize[0]-currViewportSize[0]), currViewportSize[1]/6, 
                        currViewportSize[0], currViewportSize[1]);
    }
    glEnable(GL_SCISSOR_TEST);

    /* Set up the current fog properties. */
    glEnable(GL_FOG);
    glFogf(GL_FOG_DENSITY, fogDensity);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fogColor);

    /* Set up the properties of the viewing camera. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, AspectRatio, 0.1, 300.0);

    /* ??? When scissor is disabled AFTER rendering the animation    ??? */
    /* ??? (where it logically SHOULD be disabled, instead of here), ??? */
    /* ??? the control panel is still clipped from the screen.  WHY? ??? */
    glDisable(GL_SCISSOR_TEST);

    /* Position the camera and draw the texture-mapped environment. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

        /* Position camera to always be aimed down the z-axis */
        /* (modified slightly to accommodate any incline).    */
        gluLookAt(viewPosition[0], viewPosition[1], viewPosition[2],
                  viewPosition[0], viewPosition[1]+lookAtYDelta, viewPosition[2]+1,
                  0.0, 1.0, 0.0);

        /* Draw the objects comprising the city scene. */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawCityElements();
        RenderPrecipitation();

        glPopMatrix();

    /* Expand viewport so display panel can be drawn. */
    glDisable(GL_LIGHTING);
    glViewport(0,0,currWindowSize[0],currWindowSize[1]);
    DrawDisplayPanel();

    /* Exchange old and new display buffers (i.e., animate). */
    glutSwapBuffers();
    glFlush();
}


/**************************************************/
/* Window-reshaping callback, adjusting the view- */
/* port to be as large as possible within the     */
/* window, without changing its aspect ratio.     */
/**************************************************/
void ResizeWindow(GLsizei w, GLsizei h)
{
    currWindowSize[0] = w;
    currWindowSize[1] = h;
    if (AspectRatio > w/h)
    {
        currViewportSize[0] = w;
        currViewportSize[1] = w/AspectRatio;
    }
    else
    {
        currViewportSize[1] = h;
        currViewportSize[0] = h*AspectRatio;
    }

    /* Center the image within the resized window. */
    glViewport(0.5*(w-currWindowSize[0]), 0, currWindowSize[0], currWindowSize[1]);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**************************************************************/
/* Render the primitives comprising the downtown skyscrapers, */
/* sidewalks, streets, etc. for the cityscape environment.    */
/**************************************************************/
void DrawCityElements()
{	
    static bool firstTime = true;
    static time_t randomNumberSeed;

    if (firstTime)
    {
        time(&randomNumberSeed);
        firstTime = false;
    }
    srand(randomNumberSeed);

    GLfloat firstZ = 0.0;				// z-position of first recycled cube
    while (firstZ < viewPosition[2])
        firstZ += 200.0;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    DrawIntersection(firstZ);
    for (int i = 1; i < NbrOfRoadIterations; i++)
    {
        DrawRoadCube(firstZ, i);
        DrawSidewalkCubePair(firstZ,i);
        DrawStreetlight(firstZ, i, RHS);
        DrawStreetlight(firstZ, i, LHS);
        DrawCityProp(firstZ, i);
        DrawSkyscraper(firstZ, i, RHS);
        DrawSkyscraper(firstZ, i, LHS);
    }
    DrawCityFarPlaneCube();
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}

/**************************************************/
/* Render the primitives comprising the street    */
/* intersection, including the crosswalk stripes. */
/**************************************************/
void DrawIntersection(GLfloat firstZ)
{
    int j;

    GLfloat matAmbient[]   = { RoadColor[0], RoadColor[1], RoadColor[2], 1.0 };
    GLfloat matDiffuse[]   = { RoadColor[0], RoadColor[1], RoadColor[2], 1.0 };
    GLfloat matSpecular[]  = { RoadColor[0], RoadColor[1], RoadColor[2], 1.0 };
    GLfloat matEmission[]  = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat matShininess[] = { 0.3 };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    GLfloat tranZ = firstZ-(0.75*RoadBlockLength);
    if (firstZ > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
        tranZ -= NbrOfRoadIterations*RoadBlockLength;

    /* Intersecting road cube */
    glPushMatrix();
        glColor3f( RoadColor[0], RoadColor[1], RoadColor[2] );
        glTranslatef( 0.0, Ymin, tranZ );
        glScalef( IntersectionBlockScale[0], IntersectionBlockScale[1], IntersectionBlockScale[2] );
        glutSolidCube(RoadBlockLength);
    glPopMatrix();

    /* Vertical crosswalk cubes */
    for (j = 0; j < 3; j++)
    {
        matAmbient[j] = matDiffuse[j] = matSpecular[j] = RoadLineColor[j];
        matEmission[j] = 0.0;
    }
    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    for (j = 0; j < 4; j++)
    {
        glPushMatrix();
            glColor3f( RoadLineColor[0], RoadLineColor[1], RoadLineColor[2] );
            tranZ = firstZ-(0.75*RoadBlockLength);
            if (firstZ > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                tranZ -= NbrOfRoadIterations*RoadBlockLength;
            switch (j)
            {
            case 0: { glTranslatef(    CrosswalkDisplacement+CrosswalkWidth, Ymin+0.01, tranZ ); break; }
            case 1: { glTranslatef(                   CrosswalkDisplacement, Ymin+0.01, tranZ ); break; }
            case 2: { glTranslatef(                  -CrosswalkDisplacement, Ymin+0.01, tranZ ); break; }
            case 3: { glTranslatef( -(CrosswalkDisplacement+CrosswalkWidth), Ymin+0.01, tranZ ); break; }
            }
            glScalef( VerticalCrosswalkScale[0], VerticalCrosswalkScale[1], VerticalCrosswalkScale[2] );
            glutSolidCube(RoadBlockLength);
        glPopMatrix();
    }

    /* Horizontal crosswalk cubes (limit strobing/aliasing by  */
    /* drawing these crosswalks only if viewer is very close). */
    if (((firstZ-viewPosition[2] < 2*RoadBlockLength) && (incline < 0.0)) ||
        (firstZ-viewPosition[2] < 3*RoadBlockLength))
        for (j = 0; j < 4; j++)
        {
            glPushMatrix();
                glColor3f( RoadLineColor[0], RoadLineColor[1], RoadLineColor[2] );
                tranZ = firstZ-(0.75*RoadBlockLength);
                if (firstZ > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                    tranZ -= NbrOfRoadIterations*RoadBlockLength;
                switch (j)
                {
                case 0: { tranZ += (0.52*RoadBlockLength+CrosswalkWidth); break; }
                case 1: { tranZ += 0.52*RoadBlockLength;                  break; }
                case 2: { tranZ -= 0.52*RoadBlockLength;                  break; }
                case 3: { tranZ -= (0.52*RoadBlockLength+CrosswalkWidth); break; }
                }
                glTranslatef( 0.0, Ymin+0.01, tranZ );
                glScalef( HorizCrosswalkScale[0], HorizCrosswalkScale[1], HorizCrosswalkScale[2] );
                glutSolidCube(RoadBlockLength);
            glPopMatrix();
        }
}

/**************************************************************/
/* Draw the indexed block representing a portion of the road  */
/* for the cityscape scene, including the white lines on the  */
/* block.  The parameters represent the z-value of the set of */
/* non-repeating iterated road blocks that is closest to the  */
/* viewer and the index of the block currently being drawn.   */
/**************************************************************/
void DrawRoadCube(GLfloat firstZ, int index)
{
    int j;

    GLfloat matAmbient[]   = { RoadColor[0], RoadColor[1], RoadColor[2], 1.0 };
    GLfloat matDiffuse[]   = { RoadColor[0], RoadColor[1], RoadColor[2], 1.0 };
    GLfloat matSpecular[]  = { RoadColor[0], RoadColor[1], RoadColor[2], 1.0 };
    GLfloat matEmission[]  = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat matShininess[] = { 0.3 };
    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    GLfloat tranZ = firstZ-(0.75*RoadBlockLength)+index*RoadBlockLength;
    if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
        tranZ -= NbrOfRoadIterations*RoadBlockLength;

    /* Road cube */
    glPushMatrix();
        glColor3f( RoadColor[0], RoadColor[1], RoadColor[2] );
        glTranslatef( 0.0, Ymin, tranZ );
        glScalef( RoadBlockScale[0], RoadBlockScale[1], RoadBlockScale[2] );
        glutSolidCube(RoadBlockLength);
    glPopMatrix();

    /* Road white-line cubes */
    for (j = 0; j < 3; j++)
    {
        matAmbient[j] = matDiffuse[j] = matSpecular[j] = RoadLineColor[j];
        matEmission[j] = 0.0;
    }
    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    for (j = 0; j < NbrOfLinesPerRoadBlock; j++)
    {
        glPushMatrix();
            glColor3f( RoadLineColor[0], RoadLineColor[1], RoadLineColor[2] );
            tranZ = firstZ-(1.125*RoadBlockLength)+index*RoadBlockLength + j*RoadBlockLength/4;
            if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                tranZ -= NbrOfRoadIterations*RoadBlockLength;
            glTranslatef( 0.0, Ymin+0.01, tranZ );
            glScalef( RoadLineScale[0], RoadLineScale[1], RoadLineScale[2] );
            glutSolidCube(RoadBlockLength);
        glPopMatrix();
    }
}

/****************************************************************/
/* Draw the indexed block pair representing a portion of the    */
/* sidewalks bordering both sides of the road in the cityscape. */
/* The parameters represent the z-value of the set of non-      */
/* repeating iterated road blocks that is closest to the viewer */
/* and the index of the block pair currently being drawn.       */
/****************************************************************/
void DrawSidewalkCubePair(GLfloat firstZ, int index)
{
  GLfloat matAmbient[] = { SidewalkColor[0], SidewalkColor[1], SidewalkColor[2], 1.0 };
  GLfloat matDiffuse[] = { SidewalkColor[0], SidewalkColor[1], SidewalkColor[2], 1.0 };
  GLfloat matSpecular[] = { SidewalkColor[0], SidewalkColor[1], SidewalkColor[2], 1.0 };
  GLfloat matEmission[] = { 0.0, 0.0, 0.0, 0.0 };
  GLfloat matShininess[] = { 0.3 };
  glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
  glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  GLfloat tranZ = firstZ-(0.75*RoadBlockLength)+index*RoadBlockLength;
  if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
    tranZ -= NbrOfRoadIterations*RoadBlockLength;

  glPushMatrix();
    glColor3f( SidewalkColor[0], SidewalkColor[1], SidewalkColor[2] );

    glPushMatrix();
      glTranslatef( -SidewalkDisplacement, Ymin, tranZ );
      glScalef( SidewalkScale[0], SidewalkScale[1], SidewalkScale[2] );
      glutSolidCube( RoadBlockLength );
    glPopMatrix();
 
    glPushMatrix();
      glTranslatef( SidewalkDisplacement, Ymin, tranZ );
      glScalef( SidewalkScale[0], SidewalkScale[1], SidewalkScale[2] );
      glutSolidCube( RoadBlockLength );
    glPopMatrix();
  glPopMatrix();
}

/*********************************************************************/
/* Draw a streetlight on the designated side of the current road     */
/* block in the cityscape scene, including its base, vertical post,  */
/* lamp support beam, and lamp.  The firstZ parameter represents the */
/* z-value of the set of non-repeating iterated road blocks that is  */
/* closest to the viewer; the index parameter is the index of the    */
/* block currently being drawn; and the roadside parameter indicates */
/* the side of the street (left or right) of the streetlight.        */
/*********************************************************************/
//void DrawStreetlight(GLfloat firstZ, int index, SOR roadside)
//{
//    int i;
//    GLfloat trans[3];
//
//    GLfloat matAmbient[] = { LamppostColor[0], LamppostColor[1], LamppostColor[2], 1.0 };
//    GLfloat matDiffuse[] = { LamppostColor[0], LamppostColor[1], LamppostColor[2], 1.0 };
//    GLfloat matSpecular[] = { LamppostColor[0], LamppostColor[1], LamppostColor[2], 1.0 };
//    GLfloat matEmission[] = { 0.0, 0.0, 0.0, 0.0 };
//    GLfloat matShininess[] = { 0.3 };
//    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
//    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
//    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
//
//    /* Draw the ovoid base of the lamppost */
//    glPushMatrix();		// Base
//        glColor3f( LamppostColor[0], LamppostColor[1], LamppostColor[2] );
//        trans[0] = (roadside == RHS) ? (-LamppostDisplacement) : (LamppostDisplacement);
//        trans[1] = Ymin + 0.6;
//        trans[2] = firstZ+(index-1)*RoadBlockLength;
//        if (roadside == RHS)
//            trans[2] += 0.5*RoadBlockLength;
//        if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
//            trans[2] -= NbrOfRoadIterations*RoadBlockLength;
//        glTranslatef(trans[0],trans[1],trans[2]);
//        glScalef( 0.25, 1.0, 0.25 );
//        glutSolidSphere(1.0, 12, 12);
//    glPopMatrix();
//
//    /* Draw the vertical pole of the lamppost */
//    glPushMatrix();
//        glColor3f( LamppostColor[0], LamppostColor[1], LamppostColor[2] );
//        trans[0] = (roadside == RHS) ? (-LamppostDisplacement) : (LamppostDisplacement);
//        trans[1] = Ymin + 0.6;//LamppostPoleHeight + 0.6;
//        trans[2] = firstZ+(index-1)*RoadBlockLength;
//        if (roadside == RHS)
//            trans[2] += 0.5*RoadBlockLength;
//        if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
//            trans[2] -= NbrOfRoadIterations*RoadBlockLength;
//        glTranslatef(trans[0],trans[1],trans[2]);
//        glRotatef( -90.0, 1.0, 0.0, 0.0 );
//        glutSolidCone(0.075, LamppostPoleHeight, 12, 1);
//    glPopMatrix();
//
//    // Draw the three portions of the "curved" //
//    // beam from which the lamp is suspended   //
//    glPushMatrix();
//        glColor3f( LamppostColor[0], LamppostColor[1], LamppostColor[2] );
//        trans[0] = (roadside == RHS) ? (-LamppostDisplacement) : (LamppostDisplacement);
//        trans[1] = Ymin + 5.4;
//        trans[2] = firstZ+(index-1)*RoadBlockLength;
//        if (roadside == RHS)
//            trans[2] += 0.5*RoadBlockLength;
//        if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
//            trans[2] -= NbrOfRoadIterations*RoadBlockLength;
//        glTranslatef(trans[0],trans[1],trans[2]);
//        glRotatef((roadside == RHS) ? (115.0) : (-115.0), 0.0, 0.0, 1.0);
//        glRotatef( 90.0, 1.0, 0.0, 0.0 );
//        glutSolidCone(0.025, 0.5, 6, 1);
//    glPopMatrix();
//    glPushMatrix();
//        glColor3f( LamppostColor[0], LamppostColor[1], LamppostColor[2] );
//        trans[0] = (roadside == RHS) ? (-(LamppostDisplacement-0.44)) : (LamppostDisplacement-0.44);
//        trans[1] = Ymin + 5.6;
//        trans[2] = firstZ+(index-1)*RoadBlockLength;
//        if (roadside == RHS)
//            trans[2] += 0.5*RoadBlockLength;
//        if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
//            trans[2] -= NbrOfRoadIterations*RoadBlockLength;
//        glTranslatef(trans[0],trans[1],trans[2]);
//        glRotatef((roadside == RHS) ? (90.0) : (-90.0), 0.0, 0.0, 1.0);
//        glRotatef( 90.0, 1.0, 0.0, 0.0 );
//        glutSolidCone(0.015, 0.35, 6, 1);
//    glPopMatrix();
//    glPushMatrix();
//        glColor3f( LamppostColor[0], LamppostColor[1], LamppostColor[2] );
//        trans[0] = (roadside == RHS) ? (-(LamppostDisplacement-0.74)) : (LamppostDisplacement-0.74);
//        trans[1] = Ymin + 5.6;
//        trans[2] = firstZ+(index-1)*RoadBlockLength;
//        if (roadside == RHS)
//            trans[2] += 0.5*RoadBlockLength;
//        if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
//            trans[2] -= NbrOfRoadIterations*RoadBlockLength;
//        glTranslatef(trans[0],trans[1],trans[2]);
//        glRotatef((roadside == RHS) ? (60.0) : (-60.0), 0.0, 0.0, 1.0);
//        glRotatef( 90.0, 1.0, 0.0, 0.0 );
//        glutSolidCone( 0.015, 0.35, 6, 1);
//    glPopMatrix();
//
//    /* Generate random bus stop signs on the right side of the road */
//    if (roadside == RHS)
//    {
//        bool busStopFlag = (int(GenerateRandomNumber(0.0,7.0))==0);
//        if (busStopFlag)
//        {
//            for (i = 0; i < 3; i++)
//            {
//                matAmbient[i] = matDiffuse[i] = matSpecular[i] = BusStopSignColor[i];
//                matEmission[i] = 0.0;
//            }
//            glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
//            glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
//            glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
//            glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
//            glPushMatrix();		// Bus stop sign
//                glColor3f( BusStopSignColor[0], BusStopSignColor[1], BusStopSignColor[2] );
//                trans[0] = -(LamppostDisplacement-0.14);
//                trans[1] = Ymin + 2.8;
//                trans[2] = firstZ-0.2+(0.5*RoadBlockLength)+(index-1)*RoadBlockLength;
//                if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
//                    trans[2] -= NbrOfRoadIterations*RoadBlockLength;
//                glTranslatef(trans[0],trans[1],trans[2]);
//                glScalef( 0.25, 0.25, 0.05 );
//                glutSolidCube( 1.0 );
//            glPopMatrix();
//        }
//    }
//
//    glPushMatrix();		// Lamp
//        if (timeOfDay == noon)
//        {
//            for (i = 0; i < 3; i++)
//                matAmbient[i] = matDiffuse[i] = matSpecular[i] = matEmission[i] = LampOffColor[i];
//            glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
//            glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
//            glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
//            glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
//            glColor3f( LampOffColor[0], LampOffColor[1], LampOffColor[2] );
//        }
//        else
//        {
//            for (i = 0; i < 3; i++)
//                matAmbient[i] = matDiffuse[i] = matSpecular[i] = matEmission[i] = LampOnColor[i];
//            glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
//            glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
//            glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
//            glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
//            glColor3f( LampOnColor[0], LampOnColor[1], LampOnColor[2] );
//        }
//        glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
//        trans[0] = (roadside == RHS) ? (-(LamppostDisplacement-0.94)) : (LamppostDisplacement-0.94);
//        trans[1] = Ymin + 5.4;
//        trans[2] = firstZ+(index-1)*RoadBlockLength;
//        if (roadside == RHS)
//            trans[2] += 0.5*RoadBlockLength;
//        if (firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
//            trans[2] -= NbrOfRoadIterations*RoadBlockLength;
//        glTranslatef(trans[0],trans[1],trans[2]);
//        glutSolidSphere( 0.2, 12, 12 );
//    glPopMatrix();
//}

void DrawStreetlight(GLfloat firstZ, int index, SOR roadside)
{
    int side = (roadside == RHS) ? -1 : 1;

    Translation t;
    t.x = side * LamppostDisplacement;
    t.y = Ymin;
    t.z = firstZ + (index-1) * RoadBlockLength;

    if( roadside == RHS )
      t.z += 0.5*RoadBlockLength;

    if( firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength )
      t.z -= NbrOfRoadIterations*RoadBlockLength;

    glPushMatrix();
      glTranslateft( t );
      glRotateft( Rotation( Axis::Y, 180.0f ) );
 
      // Lamppost Bottom
      glPushMatrix();
        glTranslateft( Translation( 0.0f, 0.6f, 0.0f ) );
        glScaleft( Scalation( 0.25f, 1.0f, 0.25f ) );
        glutSolidSphere(1.0, 12, 12);
      glPopMatrix();

      // Lamppost Pole
      glPushMatrix();
        glTranslateft( Translation( 0.0f, 0.6f, 0.0f ) );
        glRotateft( Rotation( Axis::X, -90.0f ) );
        glutSolidCone(0.075, LamppostPoleHeight, 12, 1);
      glPopMatrix();

      // Lamppost Arm
      glPushMatrix();
        glTranslateft( Translation( 0.0f, 5.4f, 0.0f ) );
        glRotateft( Rotation( Axis::Z, 115.0f * side ) );
        glRotateft( Rotation( Axis::X, 90.0f ) );
        glutSolidCone(0.025, 0.5, 6, 1);
      glPopMatrix();
      glPushMatrix();
        glTranslateft( Translation( 0.44f * side, 5.6f, 0.0f ) );
        glRotateft( Rotation( Axis::Z, 90.0f * side ) );
        glRotateft( Rotation( Axis::X, 90.0f ) );
        glutSolidCone(0.015, 0.35, 6, 1);
      glPopMatrix();
      glPushMatrix();
        glTranslateft( Translation( 0.74f * side, 5.6f, 0.0f ) );
        glRotateft( Rotation( Axis::Z, 60.0f * side ) );
        glRotateft( Rotation( Axis::X, 90.0f ) );
        glutSolidCone( 0.015, 0.35, 6, 1);
      glPopMatrix();

      // Lamppost Lamp
      glPushMatrix();

        /*if (timeOfDay == noon)
        {
            glColor3f( LampOffColor[0], LampOffColor[1], LampOffColor[2] );
        }
        else
        { 
            glColor3f( LampOnColor[0], LampOnColor[1], LampOnColor[2] );
        }
        glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);*/

        glTranslateft( Translation( 0.94f * side, 5.4f, 0.0f ) );
        glutSolidSphere( 0.2, 12, 12 );
      glPopMatrix();

    glPopMatrix();


    /* Generate random bus stop signs on the right side of the road */
    if (roadside == RHS && (int)Random<>().next( 0.0f, 7.0f ) == 0)
    {
        Translation bus_stop_pos;
        bus_stop_pos.x = 0.14f - LamppostDisplacement;
        bus_stop_pos.y = Ymin + 2.8f;
        bus_stop_pos.z = firstZ-0.2+(0.5*RoadBlockLength)+(index-1)*RoadBlockLength;
                
        if(firstZ+index*RoadBlockLength > viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
          bus_stop_pos.z -= NbrOfRoadIterations*RoadBlockLength;

        glPushMatrix();		// Bus stop sign
          glColor3f( BusStopSignColor[0], BusStopSignColor[1], BusStopSignColor[2] );

          glTranslateft( bus_stop_pos );
          glScaleft( Scalation( 0.25f, 0.25f, 0.05f ) );
          glutSolidCube( 1.0 );
        glPopMatrix();
    }

   

} 
void DrawMailbox( float, int );

/********************************************************************/
/* Draw a "prop" (i.e., trashcan, mailbox, or newsstand on one side */
/* of the current road block in the cityscape scene.  The firstZ    */
/* parameter represents the z-value of the set of non-repeating     */
/* iterated road blocks that is closest to the viewer, while the    */
/* index parameter is the index of the block currently being drawn. */
/********************************************************************/
void DrawCityProp(GLfloat firstZ, int index)
{
    int i;

    GLfloat matAmbient[4]  = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matDiffuse[4]  = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matSpecular[4] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matEmission[4] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat matShininess[] = { 0.3 };

    int frill = int(GenerateRandomNumber(0.0,10.0));

    switch (frill)
    {
    case 0:
    case 1: {	// Trash can (left-hand-side of the street)
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = TrashcanBarrelColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( TrashcanBarrelColor[0], TrashcanBarrelColor[1], TrashcanBarrelColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( TrashcanDisplacement, Ymin+0.8, 
                                    firstZ-(0.75*RoadBlockLength)+index*RoadBlockLength );
                    else
                        glTranslatef( TrashcanDisplacement, Ymin+0.8, 
                                    firstZ-(NbrOfRoadIterations*RoadBlockLength+0.75*RoadBlockLength)+
                                    index*RoadBlockLength );
                    glScalef(0.45, 1.5, 0.45);
                    glutSolidCube(1.0);
                glPopMatrix();
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = TrashcanLidColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( TrashcanLidColor[0], TrashcanLidColor[1], TrashcanLidColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( TrashcanDisplacement, Ymin+1.5, firstZ-(0.75*RoadBlockLength)+
                                    index*RoadBlockLength );
                    else
                        glTranslatef( TrashcanDisplacement, Ymin+1.5, firstZ-(NbrOfRoadIterations*RoadBlockLength+
                                    0.75*RoadBlockLength)+index*RoadBlockLength );
                    glutSolidSphere( 0.225, 12, 12 );
                glPopMatrix();
                break;
            }
    case 2:
    case 3: {	// Trash can (right-hand-side of the street)
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = TrashcanBarrelColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( TrashcanBarrelColor[0], TrashcanBarrelColor[1], TrashcanBarrelColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( -TrashcanDisplacement, Ymin+0.8, firstZ-(0.75*RoadBlockLength)+
                                    index*RoadBlockLength );
                    else
                        glTranslatef( -TrashcanDisplacement, Ymin+0.8, firstZ-(NbrOfRoadIterations*RoadBlockLength+
                                    0.75*RoadBlockLength)+index*RoadBlockLength );
                    glScalef(0.45, 1.5, 0.45);
                    glutSolidCube(1.0);
                glPopMatrix();
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = TrashcanLidColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( TrashcanLidColor[0], TrashcanLidColor[1], TrashcanLidColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( -TrashcanDisplacement, Ymin+1.5, firstZ-(0.75*RoadBlockLength)+
                                    index*RoadBlockLength );
                    else
                        glTranslatef( -TrashcanDisplacement, Ymin+1.5, firstZ-(NbrOfRoadIterations*RoadBlockLength+
                                    0.75*RoadBlockLength)+index*RoadBlockLength );
                    glutSolidSphere( 0.225, 12, 12 );
                glPopMatrix();
                break;
            }
    case 4: {	// Mailbox (right-hand side of the street only)
		for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = MailboxBarrelColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
		DrawMailbox( firstZ, index );
                break;
            }
    case 5:
    case 6: {	// Newsstand (left-hand-side of the street)
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = NewsstandCaseColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( NewsstandCaseColor[0], NewsstandCaseColor[1], NewsstandCaseColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( NewsstandDisplacement, Ymin+1.0, 
                                    firstZ-(0.15*RoadBlockLength)+(index-1)*RoadBlockLength );
                    else
                        glTranslatef( NewsstandDisplacement, Ymin+1.0, firstZ-(NbrOfRoadIterations*RoadBlockLength+
                                    0.15*RoadBlockLength)+(index-1)*RoadBlockLength );
                    glScalef( 0.3, 0.8, 0.7 );
                    glutSolidCube(1.0);
                glPopMatrix();
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = NewsstandDoorColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( NewsstandDoorColor[0], NewsstandDoorColor[1], NewsstandDoorColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( NewsstandDisplacement-0.01, Ymin+1.0, 
                                    firstZ-(0.15*RoadBlockLength)+(index-1)*RoadBlockLength );
                    else
                        glTranslatef( NewsstandDisplacement-0.01, Ymin+1.0, 
                                    firstZ-(NbrOfRoadIterations*RoadBlockLength+0.15*RoadBlockLength)+
                                    (index-1)*RoadBlockLength );
                    glScalef( 0.3, 0.37, 0.67 );
                    glutSolidCube(1.0);
                glPopMatrix();
                break;
            }
    case 7:
    case 8: {	// Newsstand (right-hand-side of the street)
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = NewsstandCaseColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( NewsstandCaseColor[0], NewsstandCaseColor[1], NewsstandCaseColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( -NewsstandDisplacement, Ymin+1.0, 
                                    firstZ-(0.15*RoadBlockLength)+(index-1)*RoadBlockLength );
                    else
                        glTranslatef( -NewsstandDisplacement, Ymin+1.0, 
                                    firstZ-(NbrOfRoadIterations*RoadBlockLength+0.15*RoadBlockLength)+
                                    (index-1)*RoadBlockLength );
                    glScalef( 0.3, 0.8, 0.7 );
                    glutSolidCube(1.0);
                glPopMatrix();
                glPushMatrix();
                    for (i = 0; i < 3; i++)
                    {
                        matAmbient[i] = matDiffuse[i] = matSpecular[i] = NewsstandDoorColor[i];
                        matEmission[i] = 0.0;
                    }
                    glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                    glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                    glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                    glColor3f( NewsstandDoorColor[0], NewsstandDoorColor[1], NewsstandDoorColor[2] );
                    if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                        glTranslatef( -(NewsstandDisplacement-0.01), Ymin+1.0, 
                                    firstZ-(0.15*RoadBlockLength)+(index-1)*RoadBlockLength );
                    else
                        glTranslatef( -(NewsstandDisplacement-0.01), Ymin+1.0, 
                                    firstZ-(NbrOfRoadIterations*RoadBlockLength+0.15*RoadBlockLength)+
                                    (index-1)*RoadBlockLength );
                    glScalef( 0.3, 0.37, 0.67 );
                    glutSolidCube(1.0);
                glPopMatrix();
                break;
            }
    }
}

void DrawMailbox( float firstZ, int index )
{/*
                    */

	glPushMatrix();glColor3f( MailboxBarrelColor[0], MailboxBarrelColor[1], MailboxBarrelColor[2] );
	  if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
        glTranslatef( -MailboxDisplacement, Ymin+1.0, firstZ-(0.6*RoadBlockLength)+ index*RoadBlockLength );
      else
        glTranslatef( -MailboxDisplacement, Ymin+1.0, firstZ-(NbrOfRoadIterations*RoadBlockLength+ 0.6*RoadBlockLength)+index*RoadBlockLength );

	  glPushMatrix();
        glScaleft( Scalation( 0.5f, 1.0f, 0.5f ) );
        glutSolidCube(1.0);
	  glPopMatrix();

      glPushMatrix();
        glScaleft( Scalation( 0.475f, 0.475f, 0.475f ) );
        glutSolidCube(1.0);
      glPopMatrix();
  glPopMatrix();

}

/*********************************************************/
/* Draw a building on the designated side of the indexed */
/* block of road, including the building's windows.      */
/*********************************************************/
void DrawSkyscraper(GLfloat firstZ, int index, SOR roadside)
{
    int i;
    GLfloat buildingColor[3];
    GLfloat windowColor[3];
    GLfloat heightScale;
    GLfloat depthScale;
    GLfloat matAmbient[4]  = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matDiffuse[4]  = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matSpecular[4] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matEmission[4] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat matShininess[] = { 1.0 };

    glPushMatrix();
        for (int t = 0; t < 3; t++)
            if (timeOfDay != noon)
                buildingColor[t] = GenerateRandomNumber(0.1, 0.25);
            else
                buildingColor[t] = 0.4 + GenerateRandomNumber(0.1, 0.25);

        for (i = 0; i < 3; i++)
        {
            matAmbient[i] = matDiffuse[i] = matSpecular[i] = buildingColor[i];
            matEmission[i] = 0.0;
        }
        glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
        glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
        glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
        glColor3f( buildingColor[0], buildingColor[1], buildingColor[2] );

        // Scale the building to be between one-half and twice the //
        // "normal" height, and 60-90% of the width of a street    //
        // block (ensuring variable-sized gaps between buildings). //
        heightScale = GenerateRandomNumber(0.5,2.0);
        depthScale = GenerateRandomNumber(0.6,0.9);

        if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
            glTranslatef( (roadside == RHS) ? (-SkyscraperDisplacement) : (SkyscraperDisplacement),
                        Ymin+0.5*StoryHeight*heightScale, firstZ-(0.75*RoadBlockLength)+index*RoadBlockLength );
        else
            glTranslatef( (roadside == RHS) ? (-SkyscraperDisplacement) : (SkyscraperDisplacement),
                        Ymin+0.5*StoryHeight*heightScale, 
                                    firstZ-(NbrOfRoadIterations*RoadBlockLength+0.75*RoadBlockLength)+
                                    index*RoadBlockLength );
        glScalef( 1.0, heightScale, depthScale);
        glutSolidCube(RoadBlockLength);
    glPopMatrix();

    // 25 windows per building face, evenly spaced across the face //
    for (int row = -4; row <= 4; row += 2)
        for (int col = -4; col <= 4; col += 2)
        {
                if (timeOfDay == dusk)
                {
                    windowColor[0] = GenerateRandomNumber(0.86,0.94);
                    windowColor[1] = GenerateRandomNumber(windowColor[0]-0.02,windowColor[0]+0.02);
                    windowColor[2] = GenerateRandomNumber(windowColor[0]-0.02,windowColor[0]+0.02);
                }
                else
                {
                    windowColor[2] = GenerateRandomNumber(0.4,0.5);
                    windowColor[1] = GenerateRandomNumber(windowColor[2]-0.05,windowColor[2]+0.05);
                    windowColor[0] = GenerateRandomNumber(windowColor[2]-0.05,windowColor[2]+0.05);
                }
                for (i = 0; i < 3; i++)
                {
                    matAmbient[i] = matDiffuse[i] = matSpecular[i] = windowColor[i];
                    matEmission[i] = 0.0;
                }
                glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
                glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
                glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
                glColor3f(windowColor[0],windowColor[1],windowColor[2]);

            // Window facing road //
            glPushMatrix();
                if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                    glTranslatef((roadside == RHS) ? (-WindowDisplacement) : (WindowDisplacement),
                                Ymin+0.5*StoryHeight*heightScale+row*StoryHeight*heightScale/11, 
                                firstZ-(0.75*RoadBlockLength)+index*RoadBlockLength+
                                col*RoadBlockLength*depthScale/11);
                else
                    glTranslatef((roadside == RHS) ? (-WindowDisplacement) : (WindowDisplacement),
                                Ymin+0.5*StoryHeight*heightScale+row*StoryHeight*heightScale/11, 
                                firstZ-(NbrOfRoadIterations*RoadBlockLength+0.75*RoadBlockLength)+
                                index*RoadBlockLength+col*RoadBlockLength*depthScale/11);
                glScalef(0.05,1.5,1.5);
                glutSolidCube(1.0);
            glPopMatrix();

            // Window facing viewer //
            glPushMatrix();
                if (firstZ+index*RoadBlockLength <= viewPosition[2]+NbrOfRoadIterations*RoadBlockLength)
                    glTranslatef((roadside == RHS) ? (-(SkyscraperDisplacement+
                                col*RoadBlockLength*depthScale/11)) : (SkyscraperDisplacement+
                                col*RoadBlockLength*depthScale/11),
                                Ymin+0.5*StoryHeight*heightScale+row*StoryHeight*heightScale/11, 
                                firstZ-(0.75*RoadBlockLength)+index*RoadBlockLength);
                else
                    glTranslatef((roadside == RHS) ? (-(SkyscraperDisplacement+col*RoadBlockLength*depthScale/11)) : 
                                            (SkyscraperDisplacement+col*RoadBlockLength*depthScale/11),
                                Ymin+0.5*StoryHeight*heightScale+row*StoryHeight*heightScale/11, 
                                firstZ-(NbrOfRoadIterations*RoadBlockLength+0.75*RoadBlockLength)+
                                index*RoadBlockLength);
                glScalef(1.5,1.5,1.05*RoadBlockLength*depthScale);
                glutSolidCube(1.0);
            glPopMatrix();
        }
}

/**************************************************************/
/* Draw the large, thin block in the distance that represents */
/* the farthest visible plane in the cityscape scene.         */
/**************************************************************/
void DrawCityFarPlaneCube()
{
    int i;
    GLfloat farColor[3];
    GLfloat matAmbient[4]  = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matDiffuse[4]  = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matSpecular[4] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat matEmission[4] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat matShininess[] = { 0.3 };

    glPushMatrix();
        switch (timeOfDay)
        {
        case dawn: { farColor[0] = 0.0; farColor[1] = farColor[2] = 0.6; break; }
        case noon: { farColor[0] = 0.7; farColor[1] = farColor[2] = 1.0; break; }
        case dusk: { farColor[0] = 0.0; farColor[1] = farColor[2] = 0.1; break; }
        }
        for (i = 0; i < 3; i++)
        {
            matAmbient[i] = matDiffuse[i] = matSpecular[i] = farColor[i];
            matEmission[i] = 0.0;
        }
        glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
        glMaterialfv(GL_FRONT, GL_EMISSION,  matEmission);
        glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
        glColor3f(farColor[0], farColor[1], farColor[2]);

        glTranslatef( 0.0, Ymin+200.0, viewPosition[2]+NbrOfRoadIterations*RoadBlockLength );
        glScalef( 20.0, 20.1, 0.01 );
        glutSolidCube(RoadBlockLength);
    glPopMatrix();
}

/****************************************************************/
/* Render the weather conditions (if they exist), by generating */
/* thousands of randomly spaced snowflakes or raindrops, all    */
/* positioned within a narrow corridor, in front of the viewer. */
/****************************************************************/
void RenderPrecipitation()
{
    if (weatherCondition == snowy)
    {
        GLfloat x,y,z;
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POINTS);

            /* By repeatedly using the same seed, and then adding a */
            /* continuously updated increment to each snowflake's   */
            /* position, actual "snowfall" is simulated.            */
            srand(99);
            for (int t = 0; t < 10000; t++)
            {
                x = GenerateRandomNumber(Xmin, Xmax) + precipIncrement[0];
                while (x > Xmax)
                    x -= (Xmax-Xmin);
                while (x < Xmin)
                    x += (Xmax-Xmin);
                y = GenerateRandomNumber(Ymin, Ymax) + precipIncrement[1];
                while (y > Ymax)
                    y -= (Ymax-Ymin);
                while (y < Ymin)
                    y += (Ymax-Ymin);
                z = viewPosition[2]+GenerateRandomNumber(-10.0,10.0) + precipIncrement[2];
                while (z < viewPosition[2]-10.0)
                    z += 20.0;
                while (z > viewPosition[2]+10.0)
                    z -= 20.0;
                glVertex3f(x,y,z);
            }
        glEnd();
        glFlush();
    }
    else if (weatherCondition == rainy)
    {
        GLfloat x,y,z;
        int nbrOfDrops;

        /* Customize the number of raindrops and their  */
        /* color, according to the current time-of-day. */
        switch (timeOfDay)
        {
        case dawn:
          { nbrOfDrops = 10000; glColor3f(0.7f, 0.7f, 0.8f); break; }
        case noon: { nbrOfDrops = 10000; glColor3f(0.8f, 0.8f, 0.9f); break; }
        case dusk: { nbrOfDrops = 20000; glColor3f(0.6f, 0.6f, 0.6f); break; }
        }
        glBegin(GL_LINES);

            /* By repeatedly using the same seed, and then adding  */
            /* a continuously updated increment to each raindrop's */
            /* position, actual "rainfall" is simulated.           */
            srand(13);
            for (int t = 0; t < nbrOfDrops; t++)
            {
                x = GenerateRandomNumber(Xmin, Xmax) + precipIncrement[0];
                while (x > Xmax)
                    x -= (Xmax-Xmin);
                while (x < Xmin)
                    x += (Xmax-Xmin);
                y = GenerateRandomNumber(Ymin, Ymax) + precipIncrement[1];
                while (y > Ymax)
                    y -= (Ymax-Ymin);
                while (y < Ymin)
                    y += (Ymax-Ymin);
                z = viewPosition[2]+GenerateRandomNumber(0.0,5.0) + precipIncrement[2];
                while (z < viewPosition[2])
                    z += 5.0;
                while (z > viewPosition[2]+5.0)
                    z -= 5.0;
                glVertex3f(x,y,z);
                glVertex3f(x+0.005,y-0.02,z);
            }
        glEnd();
        glFlush();
    }
}


/*******************************************************************/
/* Draw the 2-D display panel in the bottom portion of the display */
/* window, showing relevant data about the course being traversed  */
/* (e.g., current speed and incline, distance travelled thus far), */
/* and environmental data (e.g., time-of-day, weather conditions). */
/*******************************************************************/
void DrawDisplayPanel()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (float)currWindowSize[0], 0.0f, (float)currWindowSize[1], -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    /* Customize the font size within the display panel, */
    /* according to the current window size, thus using  */
    /* a large font whenever it would be reasonably      */
    /* spaced and a small font whenever space is scarce. */
    if ((currWindowSize[0] < 300) || (currWindowSize[1] < 250))
        TextFont = SmallTextFont;
    else if ((currWindowSize[0] < 800) || (currWindowSize[1] < 600))
        TextFont = MediumTextFont;
    else
        TextFont = LargeTextFont;

    /* Draw vertical line to separate the course, and scene output. */
    glColor3f(0.0f, 0.1f, 0.5f);
    glBegin(GL_LINES);
        glVertex2i(0.5*currWindowSize[0],currWindowSize[1]/6);
        glVertex2i(0.5*currWindowSize[0],0);
    glEnd();

    /* Update the distance travelled, assuming one refresh */
    /* every 1/15th of a second at the current speed.      */
    distanceTravelled += (currentSpeed/54000.0);

    /* Output current course readouts, starting with the speed. */
    glColor3f(0.7f, 0.0f, 0.0f);
    glRasterPos2i(currWindowSize[0]/4, currWindowSize[1]/8);
    FontPrintf(TextFont, 0, "Speed = %.2f MPH", currentSpeed);

    /* Output the distance travelled. */
    glRasterPos2i(currWindowSize[0]/4, currWindowSize[1]/12);
    FontPrintf(TextFont, 0, "Distance = %.2f Miles", distanceTravelled);

    /* Output the current incline. */
    glRasterPos2i(currWindowSize[0]/4, currWindowSize[1]/24);
    FontPrintf(TextFont, 0, "Incline = %.1f Degrees", incline);

    /* Output current scene parameters, starting with the time of day. */
    glColor3f(0.3f, 0.0f, 0.4f);
    char timeString[5];
    switch (timeOfDay)
    {
    case dawn: { strcpy_s(timeString, "Dawn"); break; }
    case noon: { strcpy_s(timeString, "Noon"); break; }
    case dusk: { strcpy_s(timeString, "Dusk"); break; }
    }
    glRasterPos2i(3*currWindowSize[0]/4, currWindowSize[1]/8);
    FontPrintf(TextFont, 0, "Time Of Day: %4s", timeString);

    /* Output the weather conditions. */
    char weatherString[6];
    switch (weatherCondition)
    {
    case sunny: { strcpy_s(weatherString, "Clear"); break; }
    case rainy: { strcpy_s(weatherString, "Rainy"); break; }
    case snowy: { strcpy_s(weatherString, "Snowy"); break; }
    }
    glRasterPos2i(3*currWindowSize[0]/4, currWindowSize[1]/12);
    FontPrintf(TextFont, 0, "Weather: %5s", weatherString);
}


/*******************************************************************/
/* Update the fog color and density, according to the time-of-day. */
/*******************************************************************/
void UpdateFog()
{
    int i;
    switch (timeOfDay)
    {
    case dawn: {
                fogDensity = DawnFogDensity;
                for (i = 0; i < 4; i++)
                    fogColor[i] = LightFogColor[i];
                break;
               }
    case noon: {
                fogDensity = NoonFogDensity;
                break;
               }
    case dusk: {
                fogDensity = DuskFogDensity;
                for (i = 0; i < 4; i++)
                    fogColor[i] = DarkFogColor[i];
                break;
               }
    }
}


/******************************************/
/* Generate a random floating-point value */
/* between the two parameterized values.  */
/******************************************/
float GenerateRandomNumber(float lowerBound, float upperBound)
{
    return (lowerBound + ((upperBound - lowerBound) * (float(rand()) / RAND_MAX)));
}