

#include <cmath> // atan sqrt
#include <cstdlib> // exit
#include <iostream> // cout
#include <fstream> // file io
#include <iomanip>
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#endif
#include <vector>
#include "Esfera.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
using namespace std;

//------------------------------------------------------------
// variables globales y defaults
vector<Esfera> VESFERA;                           ///*****************BUSCAR LAS LINEAS RESALTADAS*********************///
float aang=0; // orientacion
float ax=0,ay=0; // posicion en la pista
float aacel=0; // acelerador (eje y del mouse)
float aspeed=0; // velocidad actual
float topspeed=60; // velocidad maxima
float rang=0; // direccion de las ruedas delanteras respecto al auto (eje x del mouse) 
float rang2=0; // giro de las ruedas sobre su eje, cuando el auto avanza 

int
	w=640,h=480, // tamaño de la ventana
	boton=-1, // boton del mouse clickeado
	xclick,yclick, // x e y cuando clickeo un boton
	lod=10; // nivel de detalle (subdivisiones de lineas y superficies parametricas)
float // luces y colores en float
	lpos[]={2,10,5,0}, // posicion luz, l[4]: 0 => direccional -- 1 => posicional
	escala=100,escala0, // escala de los objetos window/modelo pixeles/unidad
	dist_cam=4, // distancia del ojo al origen de coordenadas en la manipulación
	eye[]={0,0,1}, target[]={0,0,0}, up[]={0,1,0}, // camara, mirando hacia y vertical
	znear=2, zfar=50, //clipping planes cercano y alejado de la camara (en 5 => veo de 3 a -3)
	amy,amy0, // angulo del modelo alrededor del eje y
	ac0=1,rc0, // angulo resp x y distancia al target de la camara al clickear
	sky_color[]={.4,.4,.8}; // color del fondo y de la niebla (azul)

bool // variables de estado de este programa
	luz_camara=true,  // luz fija a la camara o al espacio
	perspectiva=true, // perspectiva u ortogonal
	animado=false,    // el auto se mueve por la pista
	dibuja=true,      // false si esta minimizado
	relleno=true,     // dibuja relleno o no
	cl_info=false,     // informa por la linea de comandos
	top_view=false,    // vista superior o vista trasera del auto cuando animate=true
	fog=false;

// tamaño de la pista sobre
int text_w = 200; 
int text_h = 200;

short modifiers=0;  // ctrl, alt, shift (de GLUT)
inline short get_modifiers() {return modifiers=(short)glutGetModifiers();}

// temporizador:
static int msecs=20; // milisegundos por frame

// para saber qué teclas hay apretadas cuando se calcula el movimiento del auto
static bool keys[4]; // se modifica Special_cb y SpecialUp_cb, se usa en Idle_cb

static const double R2G=45/atan(1.0);
static const double G2R=atan(1.0)/45;

GLuint texid;

// Load a PPM Image
bool mipmap_ppm(const char *ifile) {
	char dummy; int maxc,wt,ht;
	ifstream fileinput(ifile, ios::binary);
	if (!fileinput.is_open()) { cerr<<"Not found"<<endl; return false; }
	fileinput.get(dummy);
	if (dummy!='P') {cerr<<"Not P6 PPM file"<<endl; return false;}
	fileinput.get(dummy);
	if (dummy!='6') {cerr<<"Not P6 PPM file"<<endl; return false;}
	fileinput.get(dummy);
	dummy=fileinput.peek();
	if (dummy=='#') do {
		fileinput.get(dummy);
	} while (dummy!=10);
	fileinput >> wt >> ht;
	fileinput >> maxc;
	fileinput.get(dummy);
	unsigned char *img=new unsigned char[3*wt*ht];
	fileinput.read((char *)img, 3*wt*ht);
	fileinput.close();
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, wt, ht,  GL_RGB, GL_UNSIGNED_BYTE, img);
	delete[] img;
	return true;
}

//------------------------------------------------------------
// redibuja los objetos
extern void drawObjects();
// Cada vez que hace un redisplay
void Display_cb() { // Este tiene que estar
	if (!dibuja) return;
	
	// borra los buffers de pantalla y z
	if (animado) glClearColor(sky_color[0],sky_color[1],sky_color[2],1);
	else         glClearColor(0,0,0,1);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK,relleno?GL_FILL:GL_LINE);
	
	glPushMatrix();
	if (animado) {
		if (top_view) 
			gluLookAt(ax,ay,15,ax,ay,0,0,1,0);
		else {
			gluLookAt(ax-5*cos(aang*G2R),ay-5*sin(aang*G2R),3,ax,ay,0,0,0,1); ///(POSICION OJO; HACIA DONDE MIRA; VECTOR ORIENTACION)
			//gluLookAt(ax-5,ay,3,ax,ay,0,1,0,0); ///(POSICION OJO; HACIA DONDE MIRA; VECTOR ORIENTACION)
			
			// @@@@@ Ubicar correctamente la camara con gluLookAt, (y ver qué pasa con la luz cuando el auto se meuve)
		}
	} else {
		gluLookAt(dist_cam*eye[0],dist_cam*eye[1],dist_cam*eye[2],0,0,0,up[0],up[1],up[2]);
	}
	
	glLightfv(GL_LIGHT0,GL_POSITION,lpos);  // ubica la luz
	drawObjects();
	glPopMatrix();
	
	glutSwapBuffers();
	
#ifdef _DEBUG
	// chequea errores
	int errornum=glGetError();
	while(errornum!=GL_NO_ERROR){
		if (cl_info){
			if(errornum==GL_INVALID_ENUM)
				cout << "GL_INVALID_ENUM" << endl;
			else if(errornum==GL_INVALID_VALUE)
				cout << "GL_INVALID_VALUE" << endl;
			else if (errornum==GL_INVALID_OPERATION)
				cout << "GL_INVALID_OPERATION" << endl;
			else if (errornum==GL_STACK_OVERFLOW)
				cout << "GL_STACK_OVERFLOW" << endl;
			else if (errornum==GL_STACK_UNDERFLOW)
				cout << "GL_STACK_UNDERFLOW" << endl;
			else if (errornum==GL_OUT_OF_MEMORY)
				cout << "GL_OUT_OF_MEMORY" << endl;
		}
		errornum=glGetError();
	}
#endif // _DEBUG
}

//------------------------------------------------------------
// Regenera la matriz de proyeccion
// cuando cambia algun parametro de la vista
void regen() {
	//  if (cl_info) cout << "regen" << endl;
	if (!dibuja) return;
	
	// matriz de proyeccion
	glMatrixMode(GL_PROJECTION);  glLoadIdentity();
	
	double w0=(double)w/2/escala,h0=(double)h/2/escala; // semiancho y semialto en el target
	
	// frustum, pal eye pero con los z positivos (delante del ojo)
	if (perspectiva){ // perspectiva
		w0*=znear/dist_cam,h0*=znear/dist_cam;  // w0 y h0 en el near
		glFrustum(-w0,w0,-h0,h0,znear,zfar);
	}
	else { // proyeccion ortogonal
		glOrtho(-w0,w0,-h0,h0,znear,zfar);
	}
	
	glMatrixMode(GL_MODELVIEW); glLoadIdentity(); // matriz del modelo->view
	
	glutPostRedisplay(); // avisa que hay que redibujar
}

//------------------------------------------------------------
// Animacion

// Cuando no hay ningún otro evento se invoca   a glutIdleFunc 
// El "framerate" lo determina msec, a menos que la complejidad 
// del modelo (lod) y la no aceleracion por hardware lo bajen
void Idle_cb() {
	static int anterior=glutGet(GLUT_ELAPSED_TIME); // milisegundos desde que arranco
	
	if (msecs!=1){ // esperar msec antes de pasar al próximo cuadro, si msecs es 1 no pierdo tiempo
		int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
		if (lapso<msecs) return;
		anterior=tiempo;
	}
	
	// aplicar los controles
	if (keys[2]!=keys[3])
		rang=(8*rang+(keys[2]?.5:-.5)*float(10*aspeed+60*(topspeed-aspeed))/topspeed)/9;
	else
		rang=3*rang/4;
	if (keys[1]) aacel=-1;
	else if (keys[0]) aacel=1;
	else aacel=-.2;
	// mover el auto
	aspeed+=aacel*.75-.25;
	if (aspeed<0) aspeed=0;
	else if (aspeed>topspeed) aspeed=topspeed;
	if (animado) {
		ax+=aspeed*cos(aang*G2R)/100;
		ay+=aspeed*sin(aang*G2R)/100;
		
		// la pista es ciclica
		if (ax<-text_w) ax+=text_w*2;
		else if (ax>text_w) ax-=text_w*2;
		if (ay<-text_h) ay+=text_h*2;
		else if (ay>text_h) ay-=text_h*2;
	} else {
		ax=ay=0;
	}
	aang-=rang*aspeed/150;
	rang2+=aspeed;
	
	
	glutPostRedisplay();
}

//------------------------------------------------------------
// Maneja cambios de ancho y alto de la ventana
void Reshape_cb(int width, int height){
	h=height; w=width;
	if (cl_info) cout << "reshape: " << w << "x" << h << endl;
	
	if (w==0||h==0) {// minimiza
		dibuja=false; // no dibuja mas
		glutIdleFunc(0); // no llama a cada rato a esa funcion
		return;
	}
	else if (!dibuja&&w&&h){// des-minimiza
		dibuja=true; // ahora si dibuja
		glutIdleFunc(Idle_cb); // registra de nuevo el callback
	}
	
	glViewport(0,0,w,h); // region donde se dibuja
	
	regen(); //regenera la matriz de proyeccion
}

//------------------------------------------------------------
// Movimientos del mouse

// Rotacion del punto (x,y,z) sobre el eje (u,v,w), formula de Rodriguez
void Rotate(float &x, float &y ,float &z, float u, float v, float w, float a) {
	float ux=u*x, uy=u*y, uz=u*z;
	float vx=v*x, vy=v*y, vz=v*z;
	float wx=w*x, wy=w*y, wz=w*z;
	float sa=sin(a), ca=cos(a);
	x=u*(ux+vy+wz)+(x*(v*v+w*w)-u*(vy+wz))*ca+(-wy+vz)*sa;
	y=v*(ux+vy+wz)+(y*(u*u+w*w)-v*(ux+wz))*ca+(wx-uz)*sa;
	z=w*(ux+vy+wz)+(z*(u*u+v*v)-w*(ux+vy))*ca+(-vx+uy)*sa;
}

void Normalize(float x[]) {
	float m=sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
	x[0]/=m; x[1]/=m; x[2]/=m;
}

// girar la camara o hacer zoom
void Motion_cb(int xm, int ym){ // drag
	if (modifiers==GLUT_ACTIVE_SHIFT) { // cambio de escala
		escala=escala0*exp((yclick-ym)/100.0);
		regen();
	} else  { // manipulacion
		float angulox=double(xclick-xm)/100; // respecto a up
		Rotate(eye[0],eye[1],eye[2],up[0],up[1],up[2],angulox);
		float anguloy=double(ym-yclick)/100; // respecto a up vectorial eye
		float // up vectorial eye normalizado
			vx=((up[2]*eye[1])-(up[1]*eye[2])),
			vy=((up[0]*eye[2])-(up[2]*eye[0])),
			vz=((up[1]*eye[0])-(up[0]*eye[1]));
		Rotate(eye[0],eye[1],eye[2],vx,vy,vz,anguloy);
		Rotate(up[0],up[1],up[2],vx,vy,vz,anguloy);
		Normalize(eye); Normalize(up); // mata el error numérico
		xclick=xm; yclick=ym;
		glutPostRedisplay();
	}
}

// Clicks del mouse
// GLUT LEFT BUTTON, GLUT MIDDLE BUTTON, or GLUT RIGHT BUTTON
// The state parameter is either GLUT UP or GLUT DOWN
// glutGetModifiers may be called to determine the state of modifier keys
///-----------------------------------------///
int idEsferaGuardada=0;
int esferaCercana = 0;
///----------------------------------------///
string mode="";
void Mouse_cb(int button, int state, int x, int y){
	
//	VESFERA[int(s)].restablecer();
	if (button==GLUT_LEFT_BUTTON){
		if (state==GLUT_DOWN) {
			xclick=x; yclick=y;
			boton=button;
			get_modifiers();
			glutMotionFunc(Motion_cb);
			if (modifiers==GLUT_ACTIVE_SHIFT) escala0=escala; // escala      
			else if (modifiers==(GLUT_ACTIVE_ALT|GLUT_ACTIVE_CTRL)) return; // origen textura
			else { // manipulacion
				double yc=eye[1]-target[1],zc=eye[2]-target[2];
				rc0=sqrt(yc*yc+zc*zc); ac0=atan2(yc,zc);
				amy0=amy;
			}
		}
		else if (state==GLUT_UP){
			boton=-1;
			glutMotionFunc(NULL);
		}
	}
	if (button==GLUT_RIGHT_BUTTON){
	if(mode=="unproject"){       ///Picking con gluUnProject*************
		double MOD=99999;
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		
		glPushMatrix();
		gluLookAt(dist_cam*eye[0],dist_cam*eye[1],dist_cam*eye[2],0,0,0,up[0],up[1],up[2]);
		
		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );
		glPopMatrix();
		///----------------------///
		float winZ;
		
		GLdouble posX, posY, posZ;
		glReadPixels( x, h-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );                 ///Leo la componente en z del z-buffer en el punto donde se hace click
		gluUnProject( x, h-y, winZ, modelview, projection, viewport, &posX, &posY, &posZ); ///Calculo con gluUnProject las coordenadas x,y,z del espacio modelo (posX,posY,posZ) relativas a la ventana

		cout<<"X="<<posX<< "    "<<"Y="<<posY<<"    "<<"Z="<<posZ<<endl;
		
		VESFERA[esferaCercana].restablecer();
		
		for(int i=0;i<VESFERA.size();i++) { 
			double distX = (posX - VESFERA[i].coordX)*(posX - VESFERA[i].coordX);
			double distY = (posY - VESFERA[i].coordY)*(posY - VESFERA[i].coordY);         ///calculo de la distancia del punto picado al centro de cada esfera
			double distZ = (posZ - VESFERA[i].coordZ)*(posZ - VESFERA[i].coordZ);
			double modulo = sqrt(distX+distY+distZ)-VESFERA[i].radio;
			
			if (MOD>modulo && modulo<10){
				MOD = modulo;
				esferaCercana = i;                 ///nos quedamos con la esfera que tenga la menor distancia
			}
		}
		cout<<"MOD: "<<MOD<<endl;
		cout<<"ESFERA CERCANA: "<<esferaCercana<<endl;
		cout<<endl;
		if(MOD<10){
			VESFERA[esferaCercana].resaltar();         ///la resaltamos(seleccionamos)
		}
	}
	
	
	if(mode=="stencil"){       ///Picking con Stencil*************
		unsigned char s;      
		glReadPixels(x,h-y,1,1,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,&s);  ///leo el valor del Stencil Buffer en el punto donde se hace click
		if(int(s)!=0){
			VESFERA[idEsferaGuardada].restablecer();
		}
		if(int(s)!=0){    ///Stencil=0 es el fondo, no queremos que titile el fondo si picamos sobre el
			idEsferaGuardada=int(s);  ///guardo la esfera picada para restablecerla la proxima vez que clickee otra esfera
			VESFERA[int(s)].resaltar();
		}
		cout<<"X="<<x<< "    "<<"Y="<<y<<endl;
		cout <<"Stencil buffer: "<<int(s)<<"        "<<endl; cout.flush();
		cout<<endl;
	}
	}
}
//------------------------------------------------------------
// Teclado
// Special keys (non-ASCII)
// aca es int key
void Special_cb(int key,int xm=0,int ym=0) {
	if (key==GLUT_KEY_F4){ // alt+f4 => exit
		get_modifiers();
		if (modifiers==GLUT_ACTIVE_ALT)
			exit(EXIT_SUCCESS);
	}
	else if (key==GLUT_KEY_UP) keys[0]=true;
	else if (key==GLUT_KEY_DOWN) keys[1]=true;
	else if (key==GLUT_KEY_RIGHT) keys[2]=true;
	else if (key==GLUT_KEY_LEFT) keys[3]=true;
}

void SpecialUp_cb(int key,int xm=0,int ym=0) {
	if (key==GLUT_KEY_UP) keys[0]=false;
	else if (key==GLUT_KEY_DOWN) keys[1]=false;
	else if (key==GLUT_KEY_RIGHT) keys[2]=false;
	else if (key==GLUT_KEY_LEFT) keys[3]=false;
}

// Maneja pulsaciones del teclado (ASCII keys)
void Keyboard_cb(unsigned char key,int x=0,int y=0) {
	switch (key){
	case 'f': case 'F': // relleno
		relleno=!relleno;
		if (relleno) 
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		if (cl_info) cout << ((relleno)? "Relleno" : "Sin Relleno") << endl;
		break;
	case 'p': case 'P':  // perspectiva
		perspectiva=!perspectiva;
		if (cl_info) cout << ((perspectiva)? "Perspectiva" : "Ortogonal") << endl;
		regen();
		break;	
	case 'u': case 'U': 
		if(mode!="unproject"){ mode="unproject";cout<<"PICKING UNPROJECT ACTIVADO"<<endl; VESFERA[idEsferaGuardada].restablecer(); idEsferaGuardada=205;}
		else {mode=""; cout<<"PICKING UNPROJECT DESACTIVADO"<<endl;
		VESFERA[esferaCercana].restablecer();
		esferaCercana=205;
		}
		break;
		
	case 's': case 'S': 
		
		if(mode!="stencil"){ mode="stencil";cout<<"PICKING STENCIL ACTIVADO"<<endl; VESFERA[esferaCercana].restablecer(); esferaCercana=205;}
		else {mode=""; cout<<"PICKING STENCIL DESACTIVADO"<<endl;
		VESFERA[idEsferaGuardada].restablecer();
		idEsferaGuardada=205;
		}
		break;
	case 27: // escape => exit
		get_modifiers();
		if (!modifiers)
			exit(EXIT_SUCCESS);
		break;
	regen();
	glutPostRedisplay();
	}
}

//------------------------------------------------------------
// Menu
void Menu_cb(int value)
{
	switch (value){
	case 'f':
		Keyboard_cb('f');
		return;
	case 'p':
		Keyboard_cb('p');
		return;		
	case 's':
		Keyboard_cb('s');
		return;
		
	case 't':
		Keyboard_cb('t');
		return;
	case 27: //esc
		exit(EXIT_SUCCESS);
	}
}
//------------------------------------------------------------
// pregunta a OpenGL por el valor de una variable de estado
int integerv(GLenum pname){
	int value;
	glGetIntegerv(pname,&value);
	return value;
}
#define _PRINT_INT_VALUE(pname) #pname << ": " << integerv(pname) <<endl

//------------------------------------------------------------
//void PMotion_cb(int xm, int ym){
//	unsigned char s;
//	glReadPixels(xm,h-ym,1,1,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,&s);
//	cout <<"\rStencil buffer: "<<int(s)<<"        "; cout.flush();
//}
// Inicializa GLUT y OpenGL
void initialize() {
	srand (time(NULL));
	for(int i=0;i<200;i++) {          ///creamos 200 esferas
		
//		float R = rand()%0/10.0;
		float R = 0;
		float G = rand()%10/10.0;  ///cada esfera tendra color con componentes en azul y verde solamente para resaltarlas en rojo
		float B = rand()%10/10.0;
		
		int signoX = rand()%2;
		if(signoX == 0) signoX =-1;
		float X = rand()%10/10.0 * signoX;
		int signoY = rand()%2;
		if(signoY == 0) signoY =-1;              ///se posicionan las esferas en lugares aleatorios dentro del cubo
		float Y = rand()%10/10.0 * signoY;
		int signoZ = rand()%2;
		if(signoZ == 0) signoZ =-1;
		float Z = rand()%10/10.0 * signoZ;
		
		vector<float> POS;
		vector<float> COLOR;
		COLOR.push_back(R);
		COLOR.push_back(G);
		COLOR.push_back(B);
		POS.push_back(X);
		POS.push_back(Y);
		POS.push_back(Z);
		float rad = rand()%2/10.0 +0.1;
		Esfera c(POS,rad,COLOR,i+1);      ///notar que el id empieza de 1 (i+1) porque el id=0 corresponde al fondo
		VESFERA.push_back(c);         ///vector de esferas
		
	}
	
	
	
	// pide z-buffer, color RGB y double buffering
	glutInitDisplayMode(GLUT_DEPTH|GLUT_RGB|GLUT_ALPHA|GLUT_DOUBLE|GLUT_STENCIL);
	
	glutInitWindowSize(w,h); glutInitWindowPosition(50,50);
	
	glutCreateWindow("Picking"); // crea el main window
	
	//declara los callbacks
	//los que no se usan no se declaran
	glutDisplayFunc(Display_cb); // redisplays
	glutReshapeFunc(Reshape_cb); // cambio de alto y ancho
	glutKeyboardFunc(Keyboard_cb); // teclado
	glutSpecialFunc(Special_cb); // teclas especiales (cuando se apretan)
	glutSpecialUpFunc(SpecialUp_cb); // teclas especiales (cuando se sueltan)
	glutMouseFunc(Mouse_cb); // botones picados
	glutIdleFunc(Idle_cb); // registra el callback
	
	
	
	// ========================
	// estado normal del OpenGL
	// ========================
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); // habilita el z-buffer
	glEnable(GL_NORMALIZE); // para que el scaling no moleste
	glEnable(GL_AUTO_NORMAL); // para nurbs??
	glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset (1,1); // coplanaridad
	
	glEnable(GL_BLEND); // para usar transparencias en drawPlanito
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	// interpola normales por nodos o una normal por plano
	//  glShadeModel(GL_FLAT);
	
	// direccion de los poligonos
	glFrontFace(GL_CCW); glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glCullFace(GL_BACK); glDisable(GL_CULL_FACE);// se habilita por objeto (cerrado o abierto)
	
	// define luces
	static const float
		lambient[]={.4f,.4f,.4f,1}, // luz ambiente
		ldiffuse[]={.7f,.7f,.7f,1}, // luz difusa
		lspecular[]={1,1,1,1};      // luz especular
	glLightfv(GL_LIGHT0,GL_AMBIENT,lambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,ldiffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,lspecular);
	// caras de atras y adelante distintos (1) o iguales (0)
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	// material estandar
	static const float
		fambient[]={.4f,.2f,.1f,1},
		fdiffuse[]={.5f,.2f,.1f,.5},
		fspecular[]={1,1,1,1},
		bambient[]={.2f,.4f,.4f,1},
		bdiffuse[]={.2f,.5f,.4f,.5},
		bspecular[]={1,1,1,1};
	static const int
		fshininess=25,
		bshininess=50;
	glMaterialfv(GL_FRONT,GL_AMBIENT,fambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,fdiffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,fspecular);
	glMateriali(GL_FRONT,GL_SHININESS,fshininess);
	glMaterialfv(GL_BACK,GL_AMBIENT,bambient);
	glMaterialfv(GL_BACK,GL_DIFFUSE,bdiffuse);
	glMaterialfv(GL_BACK,GL_SPECULAR,bspecular);
	glMateriali(GL_BACK,GL_SHININESS,bshininess);
	glEnable(GL_COLOR_MATERIAL);
}

	

	


//------------------------------------------------------------
// main
int main(int argc,char** argv) {
	glutInit(&argc,argv);// inicializa glut
	initialize(); // condiciones iniciales de la ventana y OpenGL
	glutMainLoop(); // entra en loop de reconocimiento de eventos
	return 0;
}
