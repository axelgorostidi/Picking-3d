#include <iostream> // cout
#include <cmath> // sin cos
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#endif
#include "Esfera.h"
#include <cstdlib>
#include <vector>
using namespace std;

#define mat_error DEBEN_ARMAR_LAS_MATRICES_MANUALMENTE
#define glRotated(a,x,y,z) mat_error
#define glRotatef(a,x,y,z) mat_error
#define glTranslatef(x,y,z) mat_error
#define glTranslated(x,y,z) mat_error
#define glScaled(x,y,z) mat_error
#define glScalef(x,y,z) mat_error
extern vector<Esfera> VESFERA;
extern int text_w; // tamaño de la textura de la pista
extern int text_h; // tamaño de la textura de la pista
extern bool animado;
//==========================================
// globales
int t=0;
extern float aang; // orientacion
extern float ax,ay; // posicion en la pista
extern float aacel; // acelerador (eje y del mouse)
extern float rang; // direccion de las ruedas delanteras respecto al auto (eje x del mouse) 
extern float rang2; // giro de las ruedas sobre su eje, cuando el auto avanza 

extern float lpos[]; // posición de la luz
extern int lod; // nivel de detalle para los graficos

//==========================================

//==========================================
// algunos objetos
//==========================================

void drawCube() {
  glColor3f(1,1,1);
  glBegin(GL_LINES);
  glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1);
  glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1);
  glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1);
  glVertex3f(1,1,1); glVertex3f(1,1,-1);
  glVertex3f(1,1,1); glVertex3f(1,-1,1);
  glVertex3f(1,1,1); glVertex3f(-1,1,1);
  glVertex3f(-1,-1,1); glVertex3f(1,-1,1);
  glVertex3f(-1,-1,1); glVertex3f(-1,1,1);
  glVertex3f(-1,1,-1); glVertex3f(-1,1,1);
  glVertex3f(-1,1,-1); glVertex3f(1,1,-1);
  glVertex3f(1,-1,-1); glVertex3f(1,1,-1);
  glVertex3f(1,-1,-1); glVertex3f(1,-1,1);
  glEnd();
}

void drawObjects() {
  if (!animado) drawCube();



///----------------------------------------------------------------------------//
///----------------------------------------------------------------------------//

  for(int i=0;i<VESFERA.size();i++) { 
	int id = VESFERA[i].ID;
	glStencilFunc(GL_ALWAYS,id,~0);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);        ///para asignarle un valor de stencil a cada esfera
	VESFERA[id].drawEsfera();
  }
  int r;
  extern int idEsferaGuardada;
  VESFERA[idEsferaGuardada].resaltar();
  
  extern int esferaCercana;
  VESFERA[esferaCercana].resaltar();
 ///----------------------------------------------------------------------------//
 ///----------------------------------------------------------------------------//

}


























