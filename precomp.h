// Add your includes to this file instead of to individual .cpp files.
// Do not include headers in header files (ever).

// Prevent expansion clashes (when using std::min and std::max).
#define NOMINMAX

#define SCRWIDTH 512
#define SCRHEIGHT 512
#define INVPI 1/PI
#define DOUBLEPI 2*PI

#define THREAD_NO 128
#define SHOWFPS false

#define SCENE 10
/*
 * SCENE list:
 * 1 - 2 diffuse spheres, reflective sphere, textured floor, 1 spherical area light, 1 rectangular area light. DOF, RR, NEE, IS, FILTERING.
 * 2 - 2 diffuse spheres, Beer sphere, textured floor, 2 rectangular area lights. RR, NEE, IS, FILTERING
 * 3 - 2 diffuse spheres, reflective sphere, textured floor, 2 spherical area lights. REFERENCE PATH TRACER.
 * 4 - 1 diffuse sphere, 4 Phong spheres, textured floor, 2 rectangular area lights. RR, IS, FILTERING.
 * 7 - Triangle mesh. RR, NEE, IS, FILTERING.
 * 8 - Cornell Box. RR, NEE, IS.
 * 9 - Trip through the Grand Canyon using Aladdin's flying carpet. RR, NEE, IS, SKYBOX.
 * 10 - Snowmen taking over winter. RR, NEE, IS, FILTERING.
 * Other values show scene 1.
 */

/* Start Features - Please note that for demo purposes some scenes have features set as active/inactive in game.cpp. "pathTracer.useIS = boolean" for example. */
#define MAXDEPTH 10
#define RR true
#define NEE true
#define IS true
#define FILTERING true

#define DOF false
#define FOCALLENGTH 10
#define APERTURE 0.5 //lens cross sectional size

#define SKYBOX false
/* End Features */

 // Glew should be included first
#include <GL/glew.h>
// Comment for autoformatters: prevent reordering these two.
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef _WIN32
// Followed by the Windows header
#include <Windows.h>

// Then import wglext: This library tries to include the Windows
// header WIN32_LEAN_AND_MEAN, unless it was already imported.
#include <GL/wglext.h>

// Extra definitions for redirectIO
#include <fcntl.h>
#include <io.h>
#endif

// External dependencies:
#include <FreeImage.h>
#include <SDL2/SDL.h>

// C++ headers
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include "ctpl_stl.h"

// Namespaced C headers:
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Header for AVX, and every technology before it.
// If your CPU does not support this, include the appropriate header instead.
// See: https://stackoverflow.com/a/11228864/2844473
#include <immintrin.h>

// clang-format off

// "Leak" common namespaces to all compilation units. This is not standard
// C++ practice but a mere simplification for this small project.
using namespace std;

#include "surface.h"
#include "template.h"
#include "game.h"
#include "pathTracer.h"
#include "camera.h"
#include "BVH.h"
#include "BVHNode.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "triangleMesh.h"
#include "floor.h"
#include "wall.h"
#include "object.h"
#include "AABB.h"
#include "ray.h"
#include "material.h"



using namespace Tmpl8;

extern int skyboxIndex;
extern vector<FIBITMAP*> textures;


//#include "game.h"
// clang-format on
