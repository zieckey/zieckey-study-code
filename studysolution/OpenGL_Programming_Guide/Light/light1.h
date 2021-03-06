#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")


void init()
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0, 0, 0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };

	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel( GL_SMOOTH );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
	glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, white_light );
	glLightfv( GL_LIGHT0, GL_SPECULAR, white_light );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lmodel_ambient );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_DEPTH_TEST );
}

void display()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glutSolidSphere( 1.0, 20, 16 );
	glFlush();
}

void reshape( int w, int h )
{
	glViewport( 0, 0, ( GLsizei )w, ( GLsizei )h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	GLfloat factor = 1.0;

	if ( w <= h )
	{
		factor = ( GLfloat )h / ( GLfloat )w;
	}
	else
	{
		factor = ( GLfloat )w / ( GLfloat )h;
	}

	glOrtho( -1.5, 1.5, -1.5 * factor, 1.5 * factor, -10.0, 10.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

};