#include "Esfera.h"
#include <GL/glut.h>
#include <vector>
#include <ctime>
using namespace std;

Esfera::Esfera(vector<float> Vcoord, float _r, vector<float> Vcolor, int _ID) {
	coordX = Vcoord[0]; coordY = Vcoord[1]; coordZ = Vcoord[2]; radio = _r;
	colorR = Vcolor[0]; colorG = Vcolor[1]; colorB = Vcolor[2]; ID = _ID+1;
	
	
}
void Esfera::drawEsfera(){
	glPushMatrix();
	float MEsfera[] = { 1, 0, 0, 0,    
						0, 1, 0, 0,    
						0, 0, 1, 0,    
						coordX,coordY, coordZ, 1 };    
	glMultMatrixf(MEsfera);
	glColor3f(colorR,colorG,colorB);
	glutSolidSphere(radio,50,50);
	glPopMatrix();
}

void Esfera::resaltar(){

	colorR = rand()%10/10.0;
	colorG = 0;
	colorB = 0;
	
}

void Esfera::restablecer(){
	srand (time(NULL));
	float R = 0;
	float G = rand()%10/10.0;
	float B = rand()%10/10.0;
	
	colorR = R;
	colorG = G;
	colorB = B;
}
Esfera::~Esfera() {
	
}

