#include "GLMenu.h"

#include "GL/glut.h"

#include "Vector3.h"


#define DEFAULT_WINDOW_HEIGHT 768
#define DEFAULT_WINDOW_WIDTH 1024

GLMenu::GLMenu(void)
{
}

GLMenu::~GLMenu(void)
{
	for (int i = 0; i < lines.size(); i++)
	{
//		delete []lines[i];
	}
}

void GLMenu::addMenuLine(string line)
{
	const char* original = line.c_str();
	char* str = new char[ strlen(original) ];
	strcpy(str, original);
	
	this->lines.push_back( str );
}

void GLMenu::draw()
{
	float startPos = 48;
	const float pixelsPerLine = 48.0;
	float pixelsizeH = 1.0/DEFAULT_WINDOW_HEIGHT;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		{
			glOrtho(0, DEFAULT_WINDOW_WIDTH, 0, DEFAULT_WINDOW_HEIGHT, -1, 1);

			std::vector<char*>::const_iterator linesIt = lines.begin();
			std::vector<char*>::const_iterator end = lines.end();
			
			for(; linesIt != end; ++linesIt, startPos+= 16)
			{
				//glRasterPos2f(-1,1.0-1*pixelsPerLine*pixelsizeH);
				glRasterPos2f(0, DEFAULT_WINDOW_HEIGHT - startPos);
				int len = strlen( *linesIt );
				for (unsigned int i = 0; i < len; i++){
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,(*linesIt)[i]);
				}   
			}
		}
		glPopMatrix();
	}
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}