#ifndef ESFERA_H
#define ESFERA_H
#include <vector>
#include "Esfera.h"
using namespace std;
class Esfera {
private:
	
//	vector<float> VPOS;
//	vector<float> VCOLOR;
protected:
public:
	float coordX;
	float coordY;
	float coordZ;
	float colorR;
	float colorG;
	float colorB;
	float radio;
	int ID;      
	Esfera(vector<float> VPOS, float radio, vector<float> VCOLOR, int ID);
	void drawEsfera();
	void resaltar();
	void restablecer();
	~Esfera();
	
};

#endif

