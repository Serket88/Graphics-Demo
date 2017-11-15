#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Material.h"
#include "Light.h"
#include "component.h"

#define PI 3.14159265358979323846

using namespace std;
using namespace glm;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from

shared_ptr<Camera> camera;
shared_ptr<Program> prog;
shared_ptr<Shape> bunny;
shared_ptr<Shape> mecha;
shared_ptr<Shape> sphere;
shared_ptr<Material> material;
shared_ptr<Light> light;

bool keyToggles[256] = {false}; // only for English keyboards!

int matIndex = 0;
vector<Material> matVec;

vector<component> compVec;
vector<component> sunVec;

// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;
		bool ctrl  = (mods & GLFW_MOD_CONTROL) != 0;
		bool alt   = (mods & GLFW_MOD_ALT) != 0;
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if(state == GLFW_PRESS) {
		camera->mouseMoved((float)xmouse, (float)ymouse);
	}
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	GLfloat cameraSpeed = 0.05f;

	switch((unsigned char) key) {
		case 'w' :
			//  move forward
			camera->pos += cameraSpeed * camera->front;
			break;
		case 's' :
			//  move backwards
			camera->pos -= cameraSpeed * camera->front;
			break;
		case 'a' :
			//  move left
			camera->pos -= cameraSpeed * normalize(cross(camera->front, camera->up));
			break;
		case 'd' :
			//  move right
			camera->pos += cameraSpeed * normalize(cross(camera->front, camera->up));
			break;
	}

}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "frag.glsl");
	prog->setVerbose(false);
	prog->init();
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->addAttribute("aTex");
	prog->addUniform("MV");
	prog->addUniform("P");
	prog->addUniform("lightPos");
	prog->addUniform("lightInt");
	prog->addUniform("ka");
	prog->addUniform("kd");
	prog->addUniform("ks");
	prog->addUniform("s");

	for (float x=-10; x<=10; x+= 2) {
		for (float z=10; z>=-10; z-= 2) {

			Material compMat;
			
			float kaR = (rand() % 256);
			kaR = kaR / 256;
			float kaG = rand() % 256;
			kaG = kaG / 256;
			float kaB = rand() % 256;
			kaB = kaB / 256;

			compMat.ka = vec3(kaR, kaG, kaB);
			compMat.kd = vec3(0.8, 0.7, 0.7);
			compMat.ks = vec3(1.0, 0.9, 0.8);
			compMat.s = 200.0f;

			component temp = component(
					vec3(x, 0, z),
					vec3(0, 0, 0),
					vec3(1, 1, 1),
					compMat
					);

			int meshChoice = rand() % 2 + 1;

			if (meshChoice == 1) {
				float randRot = (float)rand()/(RAND_MAX)*(2*PI);
				temp.rotate += vec3(0, randRot, 0);
				temp.type = "bunny";
			} else {
				temp.rotate = vec3(-PI/2, 0, PI);
				float randRot = (float)rand()/(RAND_MAX)*(2*PI);
				temp.rotate += vec3(0, 0, randRot);
				temp.type = "mecha";
			}

			compVec.push_back(temp);
		}
	}

	Material lightMat;
	lightMat.ka = vec3(1, 1, 0.19);
	lightMat.kd = vec3(0.8, 0.7, 0.7);
	lightMat.ks = vec3(1.0, 0.9, 0.8);
	lightMat.s = 200.0f;

	component sun = component(
			vec3(1, 10, 1),
			vec3(0, 0, 0),
			vec3(1, 1, 1),
			lightMat
			);

	sunVec.push_back(sun);

	camera = make_shared<Camera>();
	camera->setInitDistance(2.0f);
	
	bunny = make_shared<Shape>();
	mecha = make_shared<Shape>();
	sphere = make_shared<Shape>();

	bunny->loadMesh(RESOURCE_DIR + "bunny.obj");
	mecha->loadMesh(RESOURCE_DIR + "mecha.obj");
	sphere->loadMesh(RESOURCE_DIR + "sphere.obj");

	bunny->fitToUnitBox();
	mecha->fitToUnitBox();
	sphere->fitToUnitBox();

	bunny->init();
	mecha->init();
	sphere->init();

	light = make_shared<Light>();
	light->lightPos = sunVec[0].translate;
	light->intensity = 0.8;

	GLSL::checkError(GET_FILE_LINE);
}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	
	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	camera->applyViewMatrix(MV);

	mat4 mvTop = MV->topMatrix();
	vec4 newPos = mvTop * vec4(10, 2, 1, 1.0);
	light->lightPos = vec3(newPos[0], newPos[1], newPos[2]);

	prog->bind();

	sunVec[0].draw(MV, P, prog, sphere, light);

	//  draw component
	for (size_t i=0; i<compVec.size(); i++) {
		if (compVec[i].type == "bunny") {
			compVec[i].draw(MV, P, prog, bunny, light);
		} else {
			compVec[i].draw(MV, P, prog, mecha, light);
		}
	}
	
	prog->unbind();
	
	MV->popMatrix();
	P->popMatrix();

	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	srand(time(NULL));

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "calebp", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
