#pragma once

#include "precomp.h"

namespace Tmpl8 {

struct Inputs{
	bool W = false;
	bool A = false;
	bool S = false;
	bool D = false;
	bool k1 = false;
	bool k2 = false;
	bool mouseClick = false;
};

class Game
{
public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime, float t2 );
	void MouseUp( int button );
	void MouseDown( int button );
	void MouseMove( int x, int y );
	void KeyUp( int key );
	void KeyDown( int key );
	void MouseScroll(int y);
	void processInput(float deltaTIme);
	void loadObject(string objectName, mat4 cubeMat);
private:
	Surface* screen;
	Camera camera;
	PathTracer pathTracer;
	
	int objectCount = 0;
	Object* objects[700000];

	int areaLightCount = 0;
	Object* areaLights[700000];


	Inputs inputs;
};

}; // namespace Tmpl8