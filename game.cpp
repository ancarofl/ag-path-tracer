#include "precomp.h" // include (only) this in every .cpp file

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

ctpl::thread_pool plotting_pool(THREAD_NO);
vector<future<void>> plotting_results(THREAD_NO);
Pixel* buffer;
vec3 prevColours[SCRWIDTH*SCRHEIGHT];
vec3 colours[SCRWIDTH*SCRHEIGHT];
int totalSamples = 0;
static Font font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");
vector<FIBITMAP*> textures;
int skyboxIndex;
BVH bvh;
float totalLightArea = 0;
unsigned int pixels[SCRWIDTH * SCRHEIGHT];
bool newFrame = true;
static int frame = 0;

//create mats
Material mdiffuse;
Material mreflective;
Material mlight;
Material mphong;


// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init() {
	unsigned int seed1 = rand();
	unsigned int seed2 = rand();

	//initialize basics
	buffer = screen->GetBuffer();
	camera = Camera();
	pathTracer = PathTracer();
	pathTracer.useNEE = NEE;
	pathTracer.useIS = IS;
	pathTracer.useRR = RR;
	pathTracer.useDOF = DOF;
	pathTracer.hasSKYBOX = SKYBOX;
	pathTracer.useFILTERING = FILTERING;

	//load basic textures
	textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/texture4.bmp", 0), "assets/texture4.bmp")); //0
	textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/texture3.png", 0), "assets/texture3.png")); //1
	textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/texture5.bmp", 0), "assets/texture5.bmp")); //2

	//create mats
	mdiffuse = Material(diffuse);
	mreflective = Material(reflective);
	mlight = Material(light);
	Material msnowtexture = Material(textured, 3);
	mphong = Material(phong);

	//define colors
	vec3 white = vec3(1.0f, 1.0f, 1.0f);
	vec3 black = vec3(0.0f, 0.0f, 0.0f);

	//load object
	if (SCENE == 10) {
		//load snowmen textures
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/snow2.jpg", 0), "assets/snow2.jpg")); //3
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/smiling_snowman.jpg", 0), "assets/smiling_snowman.jpg")); //4
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/neutral_snowman.png", 0), "assets/neutral_snowman.png")); //5
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/buttons_snowman.png", 0), "assets/buttons_snowman.png")); //6
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/bloody_snowman.jpg", 0), "assets/bloody_snowman.jpg")); //7
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/sad_snowman.png", 0), "assets/sad_snowman.png")); //8
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/alien_snowman.png", 0), "assets/alien_snowman.png")); //9
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/suit_snowman.png", 0), "assets/suit_snowman.png")); //10
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/australia_flag.png", 0), "assets/australia_flag.png")); //11
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/king_snowman.png", 0), "assets/king_snowman.png")); //12

		//define colors
		vec3 alien_green = vec3(0.11f, 0.46f, 0.13f);
		vec3 flags_blue = vec3(0.0f, 0.13f, 0.41f);

		//floor
		objects[objectCount] = new Floor(-9.0f, 380.0f, 380.0f, 0.0f, -20.0f, msnowtexture); objectCount++;

		//snowman 1 - reg
		objects[objectCount] = new Sphere(vec3(-5.0f, -6.0f, -15.0f), 2.5f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-5.0f, -1.5f, -15.0f), 2.0f, white, Material(textured, 6)); objectCount++;
		objects[objectCount] = new Sphere(vec3(-5.0f, 1.8f, -15.0f), 1.3f, white, Material(textured, 5)); objectCount++;

		//snowman 2 - scaryish face
		objects[objectCount] = new Sphere(vec3(4.0f, -6.0f, -25.0f), 2.5f, white, msnowtexture); objectCount++;
		objects[objectCount] = new Sphere(vec3(4.0f, -1.5f, -25.0f), 2.0f, white, msnowtexture); objectCount++;
		objects[objectCount] = new Sphere(vec3(4.0f, 1.8f, -25.0f), 1.3f, white, Material(textured, 4)); objectCount++;

		//snowman 3 - bloody
		objects[objectCount] = new Sphere(vec3(-14.0f, -6.0f, -25.0f), 2.0f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-14.0f, -2.98f, -25.0f), 1.4f, white, Material(textured, 7)); objectCount++;
		objects[objectCount] = new Sphere(vec3(-14.0f, -0.9f, -25.0f), 0.8f, white, Material(textured, 8)); objectCount++;

		//snowman 4 - alien
		objects[objectCount] = new Sphere(vec3(7.0f, -6.0f, -7.0f), 3.5f, alien_green, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(7.0f, -1.2f, -7.0f), 2.0f, alien_green, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(7.0f, 2.0f, -7.0f), 1.3f, alien_green, Material(textured, 9)); objectCount++;

		//snowman 5 - suit
		objects[objectCount] = new Sphere(vec3(-17.0f, -6.0f, -11.0f), 3.5f, black, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-17.0f, -1.2f, -11.0f), 2.0f, white, Material(textured, 10)); objectCount++;
		objects[objectCount] = new Sphere(vec3(-17.0f, 2.1f, -11.0f), 1.3f, white, mdiffuse); objectCount++;

		//snowman 6 - RO flag
		objects[objectCount] = new Sphere(vec3(-37.0f, -6.0f, -21.0f), 3.5f, vec3(0.0f, 0.0f, 1.0f), mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-37.0f, -1.1f, -21.0f), 2.0f, vec3(0.9f, 1.0f, 0.3f), mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-37.0f, 2.0f, -21.0f), 1.3f, vec3(1.0f, 0.0f, 0.0f), mdiffuse); objectCount++;

		//snowman 7 - Australia flag
		objects[objectCount] = new Sphere(vec3(-29.0f, -6.0f, -23.0f), 3.5f, flags_blue, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-29.0f, -1.1f, -23.0f), 2.0f, flags_blue, Material(textured, 11)); objectCount++;
		objects[objectCount] = new Sphere(vec3(-29.0f, 2.0f, -23.0f), 1.3f, flags_blue, mdiffuse); objectCount++;

		//snowman 8 - IT flag
		objects[objectCount] = new Sphere(vec3(-44.0f, -6.0f, -16.0f), 3.5f, alien_green, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-44.0f, -1.1f, -16.0f), 2.0f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-44.0f, 2.0f, -16.0f), 1.3f, vec3(1.0f, 0.0f, 0.0f), mdiffuse); objectCount++;

		//snowman 9 - Dutch flag
		objects[objectCount] = new Sphere(vec3(-35.0f, -6.0f, -15.0f), 3.5f, vec3(1.0f, 0.0f, 0.0f), mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-35.0f, -1.1f, -15.0f), 2.0f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-35.0f, 2.0f, -15.0f), 1.3f, flags_blue, mdiffuse); objectCount++;

		//other snowmen follow
		objects[objectCount] = new Sphere(vec3(-15.0f, -6.0f, -55.0f), 2.5f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-15.0f, -1.5f, -55.0f), 2.0f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-15.0f, 1.8f, -55.0f), 1.3f, white, mdiffuse); objectCount++;

		objects[objectCount] = new Sphere(vec3(-25.0f, -6.0f, -45.0f), 2.5f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-25.0f, -1.5f, -45.0f), 2.0f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-25.0f, 1.8f, -45.0f), 1.3f, white, mdiffuse); objectCount++;

		objects[objectCount] = new Sphere(vec3(15.0f, -6.0f, -20.0f), 2.5f, white, mreflective); objectCount++;
		objects[objectCount] = new Sphere(vec3(15.0f, -1.5f, -20.0f), 2.0f, white, mreflective); objectCount++;
		objects[objectCount] = new Sphere(vec3(15.0f, 1.8f, -20.0f), 1.3f, white, mreflective); objectCount++;

		objects[objectCount] = new Sphere(vec3(-5.0f, -6.0f, -15.0f), 2.5f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-5.0f, -1.5f, -15.0f), 2.0f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-5.0f, 1.8f, -15.0f), 1.3f, white, mdiffuse); objectCount++;

		objects[objectCount] = new Sphere(vec3(-5.0f, -6.0f, -15.0f), 2.5f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-5.0f, -1.5f, -15.0f), 2.0f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(-5.0f, 1.8f, -15.0f), 1.3f, white, mdiffuse); objectCount++;

		//snowman 10 - king
		objects[objectCount] = new Sphere(vec3(0.0f, -6.0f, -50.0f), 2.5f, white, mdiffuse); objectCount++;
		objects[objectCount] = new Sphere(vec3(0.0f, -1.5f, -50.0f), 2.0f, vec3(1.0f, 0.0f, 0.0f), Material(textured, 12)); objectCount++;
		objects[objectCount] = new Sphere(vec3(0.0f, 1.8f, -50.0f), 1.3f, vec3(0.9f, 0.9f, 0.0f), mdiffuse); objectCount++;

		//generate army
		for (int i = 0; i < 9; i += 4) {
			for (int j = 0; j < 11; j += 5) {
				objects[objectCount] = new Sphere(vec3(-5.0f + j, -6.0f, -70.0f + i), 2.5f, white, mdiffuse); objectCount++;
				objects[objectCount] = new Sphere(vec3(-5.0f + j, -1.5f, -70.0f + i), 2.0f, white, mdiffuse); objectCount++;
				objects[objectCount] = new Sphere(vec3(-5.0f + j, 1.8f, -70.0f + i), 1.3f, white, mdiffuse); objectCount++;
			}
		}

		mlight.setEmission(75.0f);
		areaLights[0] = new Floor(45.0f, 14.5f, 8.5f, -20.0f, -21.0f, mlight);
		areaLightCount = 1;

		camera.setSpeed(1.2f);
	}
	else if (SCENE == 9) {
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/skybox9.jpg", 0), "assets/skybox9.jpg"));//3
		skyboxIndex = 3;
		textures.push_back(FreeImage_Load(FreeImage_GetFileType("assets/carpet.jpeg", 0), "assets/carpet.jpeg"));//4

		objects[0] = new Sphere(vec3(0.0f, 2.5f, -10.0f), 3.0f, vec3(1.0f, 1.0f, 1.0f), reflective);
		objects[1] = new Floor(-4.0f, 48.0f, 40.0f, 0.0f, -10.0f, Material(textured, 4));
		objectCount = 2;

		//areaLights[0] = new Floor(20.0f, 5.0f, 5.0f, -10.0f, -11.0f, mlight);
		//areaLights[1] = new Floor(20.0f, 5.0f, 5.0f, 10.0f, -11.0f, mlight);
		mlight.setEmission(75.0f);
		areaLights[0] = new Sphere(vec3(0.0f, 10.0f, -7.0f), 1.5f, vec3(0.0f, 0.0f, 0.5f), mlight);
		areaLightCount = 1;

		//0.745586 - 0.194297 0.637454 11.6339 0.0221327 0.963245 0.267712 8.87359 - 0.66604 - 0.185493 0.722483 0.689376 0 0 0 1
		mat4 matrix;
		matrix.cell[3] = 5.5f;
		matrix.cell[7] = 11.0f;
		matrix.cell[15] = -4.0f;
		camera.setMatrix(matrix);

		pathTracer.useFILTERING = false;
		pathTracer.hasSKYBOX = true;
	}
	// Cornell Box
	else if (SCENE == 8) {
		objects[0] = new Floor(-2.0f, 5.0f, 5.5f, 0.0f, -4.0f, mdiffuse);
		objects[1] = new Floor(2.001f, 5.0f, 5.5f, 0.0f, -4.0f, mdiffuse);
		objects[2] = new Wall(2.5f, 5.0f, 5.5f, 0.5f, -4.0f, mdiffuse, vec3(0, 0.5f, 0));
		objects[3] = new Wall(-2.5f, 5.0f, 5.5f, 0.5f, -4.0f, mdiffuse, vec3(0.5f, 0, 0));
		objects[4] = new Wall(-6.75f, 5.0f, 5.0f, 0.0f, 0.0f, mdiffuse, vec3(1, 1, 1), 'z');
		objects[5] = new Sphere(vec3(1.6f, -1.0f, -5.0f), 0.5f, vec3(1.0f, 1.0f, 1.0f), mdiffuse);
		objects[6] = new Sphere(vec3(-1.4f, -1.2f, -4.5f), 0.5f, vec3(1.0f, 1.0f, 0.0f), mdiffuse);
		objects[7] = new Sphere(vec3(0.2f, -1.2f, -4.25f), 0.5f, vec3(1.0f, 1.0f, 0.0f), mreflective);
		objectCount = 8;

		//mlight.setEmission(40.0f);
		areaLights[0] = new Floor(2.0f, 1.0f, 1.0f, 0.0f, -5.0f, mlight);
		//areaLights[0] = new Sphere(vec3(0.0f, 3.15f, -5.0f), 1.2f, vec3(1.0f, 1.0f, 1.0f), mlight);
		areaLightCount = 1;

		mat4 matrix;
		matrix.cell[11] = -1.5f;
		camera.setMatrix(matrix);

		pathTracer.useFILTERING = false;
	}
	else if (SCENE == 7) {
		string path = "assets/Crate1.obj";
		mat4 cubeMat;
		//cubeMat.cell[0] = 0.1f;
		//cubeMat.cell[5] = 0.1f;
		//cubeMat.cell[10] = 0.1f;
		// cubeMat.cell[7] = -4.0f;
		cubeMat.cell[11] = -6.0f;
		loadObject(path, cubeMat);

		objects[objectCount] = new Floor(-4.0f, 28.0f, 28.0f, 0.0f, 0.0f, Material(textured, 1));
		objectCount++;

		areaLights[0] = new Floor(20.0f, 10.0f, 10.0f, -10.0f, -3.0f, mlight);
		areaLights[1] = new Floor(20.0f, 5.0f, 5.0f, 10.0f, -3.0f, mlight);
		areaLightCount = 2;
		
		//pathTracer.useFILTERING = false;
	}
	else if (SCENE == 4) {
		objects[0] = new Sphere(vec3(0.0f, -0.5f, -10.0f), 1.0f, vec3(1.0f, 1.0f, 1.0f), mdiffuse);
		objects[1] = new Sphere(vec3(3.5f, -0.5f, -10.0f), 1.3f, vec3(1.0f, 0.4f, 0.4f), mphong);
		Material mphong2 = Material(phong);
		mphong2.setPhongFactor(60);
		objects[2] = new Sphere(vec3(-4.0f, 2.5f, -10.0f), 1.3f, vec3(1.0f, 0.8f, 0.0f), mphong2);
		Material mphong3 = Material(phong);
		mphong3.setPhongFactor(125);
		objects[3] = new Sphere(vec3(-7.5f, 0.0f, -10.0f), 1.3f, vec3(0.52f, 0.53f, 0.54f), mphong3);
		Material mphong4 = Material(phong);
		mphong4.setPhongFactor(4);
		objects[4] = new Sphere(vec3(5.5f, 0.0f, -8.0f), 1.3f, vec3(0.7f, 0.5f, 0.0f), mphong4);
		objects[5] = new Floor(-4.0f, 28.0f, 20.0f, 0.0f, -10.0f, Material(textured, 0));
		objectCount = 6;

		mlight.setEmission(40);
		areaLights[0] = new Floor(15.0f, 5.0f, 5.0f, 12.0f, -5.0f, mlight);
		areaLights[1] = new Floor(20.0f, 3.0f, 3.0f, -20.0f, -11.0f, mlight);
		areaLightCount = 2;

		pathTracer.useNEE = false;
	}
	else if (SCENE == 3) {
		objects[0] = new Sphere(vec3(0.0f, -0.5f, -10.0f), 2.0f, vec3(1.0f, 1.0f, 1.0f), mdiffuse);
		objects[1] = new Sphere(vec3(4.5f, -0.5f, -10.0f), 2.0f, vec3(0.0f, 1.0f, 0.0f), mdiffuse);
		objects[2] = new Sphere(vec3(-4.5f, -0.5, -10.0f), 2.0f, vec3(0.0f, 0.0f, 0.0f), mreflective);
		objects[3] = new Floor(-4.0f, 28.0f, 20.0f, 0.0f, -10.0f, Material(textured, 2));
		objectCount = 4;

		//areaLights[0] = new Floor(20.0f, 5.0f, 5.0f, -10.0f, -11.0f, mlight);
		//areaLights[1] = new Floor(20.0f, 5.0f, 5.0f, 10.0f, -11.0f, mlight);
		mlight.setEmission(60.0f);
		areaLights[0] = new Sphere(vec3(-4.0f, 16.5f, -10.0f), 3.5f, vec3(1.0f, 1.0f, 1.0f), mlight);
		areaLights[1] = new Sphere(vec3(18.5f, 19.5f, 6.0f), 0.9f, vec3(1.0f, 1.0f, 1.0f), mlight);
		areaLightCount = 2;

		/*ref pt*/
		pathTracer.useNEE = false;
		pathTracer.useRR = false;
		pathTracer.useIS = false;
		pathTracer.useDOF = false;
		pathTracer.hasSKYBOX = false;
		pathTracer.useFILTERING = false;
	}
	else if (SCENE == 2) {
		objects[0] = new Sphere(vec3(0.0f, -0.5f, -10.0f), 2.0f, vec3(1.0f, 1.0f, 1.0f), mdiffuse);
		objects[1] = new Sphere(vec3(4.5f, -0.5f, -10.0f), 2.0f, vec3(0.0f, 0.0f, 1.0f), mdiffuse);
		objects[2] = new Sphere(vec3(-4.5f, -0.5, -10.0f), 2.0f, vec3(0.0f, 0.0f, 0.0f), Material(beer, 1.5f, vec3(0, 0, 0)));
		objects[3] = new Floor(-4.0f, 28.0f, 20.0f, 0.0f, -10.0f, Material(textured, 1));
		objectCount = 4;

		areaLights[0] = new Floor(20.0f, 5.0f, 5.0f, -10.0f, -11.0f, mlight);
		areaLights[1] = new Floor(20.0f, 5.0f, 5.0f, 10.0f, -11.0f, mlight);
		areaLightCount = 2;

		mat4 matrix;
		//0.438372 0 0.898796 6.59906 0 1 0 0 -0.898796 0 0.438372 -4.54817 0 0 0 1
		matrix.cell[0] = 0.438372f;
		matrix.cell[1] = 0.0f;
		matrix.cell[2] = 0.898796f;
		matrix.cell[3] = 6.59906f;
		matrix.cell[4] = 0.0f;
		matrix.cell[5] = 1.0;
		matrix.cell[6] = 0.0f;
		matrix.cell[7] = 0.0f;
		matrix.cell[8] = -0.898796f;
		matrix.cell[9] = 0.0f;
		matrix.cell[10] = 0.438372f;
		matrix.cell[11] = -4.54817f;
		camera.setMatrix(matrix);
	}
	// SCENE 1
	else {
		objects[0] = new Sphere(vec3(0.0f, -0.5f, -10.0f), 2.0f, vec3(1.0f, 1.0f, 1.0f), mdiffuse);
		objects[1] = new Sphere(vec3(4.5f, -0.5f, -10.0f), 2.0f, vec3(0.0f, 1.0f, 0.6f), mdiffuse);
		objects[2] = new Sphere(vec3(-4.5f, -0.5, -10.0f), 2.0f, vec3(0.0f, 0.0f, 0.0f), mreflective);
		objects[3] = new Floor(-4.0f, 28.0f, 20.0f, 0.0f, -10.0f, Material(textured, 0));
		objectCount = 4;

		//mlight.setEmission(30.0f);
		areaLights[0] = new Floor(20.0f, 5.0f, 5.0f, -10.0f, -11.0f, mlight);
		areaLights[1] = new Sphere(vec3(7.5f, 14.5f, -10.0f), 2.6f, vec3(0.0f, 1.0f, 0.6f), mlight);
		areaLightCount = 2;

		camera.setSpeed(0.4f);
		//0.992041 0.082767 0.0948841 - 0.0388017 - 0.106819 0.1543 0.982232 4.78136 0.0666559 - 0.984551 0.161913 - 4.12488 0 0 0 1
		mat4 matrix;
		matrix.cell[0] = 0.992041f;
		matrix.cell[1] = 0.082767f;
		matrix.cell[2] = 0.0948841f;
		matrix.cell[3] = -0.0388017f;
		matrix.cell[4] = -0.106819f;
		matrix.cell[5] = 0.1543f;
		matrix.cell[6] = 0.982232f;
		matrix.cell[7] = 4.78136f;
		matrix.cell[8] = 0.0666559f;
		matrix.cell[9] = -0.984551f;
		matrix.cell[10] = 0.161913f;
		matrix.cell[11] = -4.12488f;
		camera.setMatrix(matrix);

		pathTracer.useDOF = true;
	}

	for (int i = 0; i < areaLightCount; i++) {
		totalLightArea += areaLights[i]->getArea();
	}
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
	for (int i = 0; i < objectCount; i++) {
		delete objects[i];
	}


	for (int i = 0; i < areaLightCount; i++) {
		delete areaLights[i];
	}
}


// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick(float deltaTime, float t2)
{
	// clear the graphics window
	processInput(deltaTime);
	if (newFrame) {
		totalSamples = 0;
		screen->Clear(0);
	}
	totalSamples++;

	pathTracer.seed1 = rand();
	pathTracer.seed2 = rand();
	pathTracer.calculateRays(screen, camera);
	vec4 cameraOrigin = vec4(0, 0, 0, 1);
	vec4 cameraWorld = camera.getMatrix()*cameraOrigin;

	const int plotting_per_thread = SCRHEIGHT / THREAD_NO;
	for (int i = 0; i < THREAD_NO; ++i) {
		plotting_results[i] = plotting_pool.push([i, plotting_per_thread, this, cameraWorld](int) {
			for (int k = i * plotting_per_thread; k < (i + 1) * plotting_per_thread; ++k)
			{
				int temp = k * SCRWIDTH;
				for (int j = 0; j < SCRWIDTH; j++) {
					vec3 result = 0; // initialize the result we'll keep adding to

					//calc pixel point and ray direction
					vec3 topLeft = vec3(pathTracer.topLeft.x, pathTracer.topLeft.y, pathTracer.topLeft.z);
					vec3 pixelPoint = topLeft + (pathTracer.horizontalStep*(j + 0.5)) + (pathTracer.verticalStep*(k + 0.5));
					vec3 rayDirection = vec3(pixelPoint.x - cameraWorld.x, pixelPoint.y - cameraWorld.y, pixelPoint.z - cameraWorld.z);
					rayDirection.normalize();

					//create new ray with origin and direction
					vec3 rayOrigin = vec3(cameraWorld.x, cameraWorld.y, cameraWorld.z);
					Ray ray = Ray(rayOrigin, rayDirection);

					vec3 color = pathTracer.trace(objects, objectCount, areaLights, areaLightCount, totalLightArea, ray, 0, result, false);


					//cout << color.x << " " << color.y << " " << color.z << endl;
					//cout << result.x << " " << result.y << " " << result.z << endl << "-----------" << endl;

					if (newFrame) {
						prevColours[j + temp] = vec3(0, 0, 0);
					}
					else {
						prevColours[j + temp] += color;
						color.x = (prevColours[j + temp].x / totalSamples);
						color.y = (prevColours[j + temp].y / totalSamples);
						color.z = (prevColours[j + temp].z / totalSamples);
					}

					int r = 255 * color.x;
					if (r > 255) {
						r = 255;
					}
					int g = 255 * color.y;
					if (g > 255) {
						g = 255;
					}
					int b = 255 * color.z;
					if (b > 255) {
						b = 255;
					}
					colours[j + temp] = vec3(r, g, b);
					if (!pathTracer.useFILTERING) {
						unsigned long p = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
						buffer[j + temp] = Pixel(p);
					}
				}
			}
		});
	}

	if (pathTracer.useFILTERING) {
		int halfWidth = 1;
		float sigma = 1.0f;
		float colourSigma = 700.0f;
		for (int i = 0; i < SCRWIDTH * SCRHEIGHT; i++) {
			vec3 sum = 0;
			float totalWeight = 0;

			int ix = i % SCRWIDTH;
			int iy = i / SCRWIDTH;

			for (int jx = ix - halfWidth; jx <= ix + halfWidth; jx++) {
				for (int jy = iy - halfWidth; jy <= iy + halfWidth; jy++) {
					if (jy >= 0 && jy < SCRHEIGHT && jx >= 0 && jx < SCRWIDTH) {
						float x = jx - ix;
						float y = jy - iy;
						vec3 colourDifference = colours[jy*SCRWIDTH + jx] - colours[iy*SCRWIDTH + ix];
						float colourWeight = exp(-(colourDifference.sqrLentgh()) / (2 * colourSigma * colourSigma));
						float distanceWeight = exp(-(pow(abs(x - y), 2)) / (2 * sigma * sigma));

						float weight = colourWeight * distanceWeight;
						sum += colours[jy * SCRWIDTH + jx] * weight;
						totalWeight += weight;
					}
				}
			}
			sum = sum * (1 / totalWeight);
			int r = sum.x;
			int g = sum.y;
			int b = sum.z;
			unsigned long p = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
			buffer[i] = Pixel(p);
		}
	}

	for (auto &r : plotting_results)
	{
		r.get();
	}
	if (SHOWFPS) {
		char b[40];
		frame++;
		sprintf(b, " %d", (int)(frame / t2 * 1000));
		font.Print(screen, b, 10, 10);
	}
	newFrame = false;
}


void Game::KeyDown(int key) {
	switch (key) {
	case SDL_SCANCODE_W:
		inputs.W = true;
		break;
	case SDL_SCANCODE_S:
		inputs.S = true;
		break;
	case SDL_SCANCODE_A:
		inputs.A = true;
		break;
	case SDL_SCANCODE_D:
		inputs.D = true;
		break;
	case SDL_SCANCODE_1:
		inputs.k1 = true;
		break;
	case SDL_SCANCODE_2:
		inputs.k2 = true;
		break;
	default:
		break;
	}
}

void Game::KeyUp(int key) {
	switch (key) {
	case SDL_SCANCODE_W:
		inputs.W = false;
		break;
	case SDL_SCANCODE_S:
		inputs.S = false;
		break;
	case SDL_SCANCODE_A:
		inputs.A = false;
		break;
	case SDL_SCANCODE_D:
		inputs.D = false;
		break;
	case SDL_SCANCODE_SPACE:
		for (int i = 0; i < 16; i++) {
			cout << camera.getMatrix().cell[i] << " ";
		}
		cout << endl;
	default:
		break;
	}
}

void Game::processInput(float deltaTime) {
	if (this->inputs.W) {
		newFrame = true;
		this->camera.translate(vec3(0, 0, -1), camera.getSpeed());
	}
	if (this->inputs.A) {
		newFrame = true;
		this->camera.rotate('y', 2 * (PI / 180));
	}
	if (this->inputs.S) {
		newFrame = true;
		this->camera.translate(vec3(0, 0, 1), camera.getSpeed());
	}
	if (this->inputs.D) {
		newFrame = true;
		this->camera.rotate('y', -2 * (PI / 180));
	}
}

void Game::MouseMove(int x, int y) {
	if (this->inputs.mouseClick) {
		newFrame = true;
		this->camera.rotate('y', x * -0.2f * (M_PI / 180));
		this->camera.rotate('x', y * -0.2f * (M_PI) / 180);
	}
}

void Game::MouseDown(int button) {
	this->inputs.mouseClick = true;
}

void Game::MouseUp(int button) {
	this->inputs.mouseClick = false;
}

void Game::MouseScroll(int y) {
	newFrame = true;
	camera.adjustFOV(y);
}

void Game::loadObject(string objectName, mat4 cubeMat) {
	tinyobj::attrib_t attribute;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string warn, err;
	string path = objectName;
	if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warn, &err, path.c_str())) {
		throw std::runtime_error(warn + err);
	}
	int triCount = shapes[0].mesh.num_face_vertices.size();
	vector<tinyobj::index_t> ind = shapes[0].mesh.indices;
	vector<int> indices;
	for (int i = 0; i < ind.size(); i++) {
		indices.push_back(ind[i].vertex_index);
		if (i == 3851) {
			//cout << indices[i] << endl;
		}
	}
	vector<vec3> vertices;
	for (int i = 0; i < attribute.vertices.size() / 3; i++) {
		tinyobj::real_t vx = attribute.vertices[3 * i + 0];
		tinyobj::real_t vy = attribute.vertices[3 * i + 1];
		tinyobj::real_t vz = attribute.vertices[3 * i + 2];
		vertices.push_back(vec3((float)vx, (float)vy, (float)vz));
	}

	for (int i = 0; i < vertices.size(); i++) {
		mat4 r1;
		//r1 = r1.rotatey(M_PI_2);
		cubeMat = cubeMat * r1;
		vec4 newOne = cubeMat * vec4(vertices[i].x, vertices[i].y, vertices[i].z, 1);
		vertices[i] = vec3(newOne.x, newOne.y, newOne.z);
	}

	TriangleMesh mesh = TriangleMesh(triCount, vertices, indices, vec3(1.0f, 1.0f, 1.2f), Material(diffuse, true));
	Object** obj = mesh.getTriangles();
	for (int i = 0; i < triCount; i++) {
		objects[i + objectCount] = obj[i];
	}
	objectCount += triCount;
}