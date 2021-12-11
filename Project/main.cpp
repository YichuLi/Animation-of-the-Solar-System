#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
#include "Sphere.h"
using namespace std;

#define numVAOs 1
#define numVBOs 3
//length from sun
#define RATE_OF_LENGTH 1.5
#define mercuryLength 2.387 * RATE_OF_LENGTH
#define venusLength 3.723 * RATE_OF_LENGTH
#define earthLength 5.000 * RATE_OF_LENGTH
#define marsLength 6.524 * RATE_OF_LENGTH
#define jupiterLength 8.205 * RATE_OF_LENGTH
#define saturnLength 9.576 * RATE_OF_LENGTH
#define uranusLength 15.218 * RATE_OF_LENGTH
#define neptuneLength 20.130 * RATE_OF_LENGTH
#define moonLength 1.200 //length from earth
//Orbital speed
#define mercuryOrbital_speed 1/0.49
#define venusOrbital_speed 1/0.785
#define earthOrbital_speed 1.00
#define marsOrbital_speed 1/1.38
#define jupiterOrbital_speed 1/3.435
#define saturnOrbital_speed 1/5.43
#define uranusOrbital_speed 1/9.165
#define neptuneOrbital_speed 1/12.84
#define moonOrbital_speed 1/0.272
//self-rotate speed
#define mercurySelf_rotate_speed 1/7.655
#define venusSelf_rotate_speed 1/15.59
#define earthSelf_rotate_speed 1.00
#define marsSelf_rotate_speed 1/1.0257
#define jupiterSelf_rotate_speed 1/0.41
#define saturnSelf_rotate_speed 1/0.4264
#define uranusSelf_rotate_speed 1/0.718
#define neptuneSelf_rotate_speed 1/0.6646
//self-rotate degree
#define mercuryRotateX sin(glm::radians(0.0f))
#define mercuryRotateY cos(glm::radians(0.0f))
#define mercuryRotateZ 0.0
#define venusRotateX sin(glm::radians(117.3f))
#define venusRotateY cos(glm::radians(117.3f))
#define venusRotateZ 0.0
#define earthRotateX sin(glm::radians(23.25f))
#define earthRotateY cos(glm::radians(23.25f))
#define earthRotateZ 0.0
#define marsRotateX sin(glm::radians(25.2f))
#define marsRotateY cos(glm::radians(25.2f))
#define marsRotateZ 0.0
#define jupiterRotateX sin(glm::radians(3.1f))
#define jupiterRotateY cos(glm::radians(3.1f))
#define jupiterRotateZ 0.0
#define saturnRotateX sin(glm::radians(26.7f))
#define saturnRotateY cos(glm::radians(26.7f))
#define saturnRotateZ 0.0
#define uranusRotateX sin(glm::radians(97.8f))
#define uranusRotateY cos(glm::radians(97.8f))
#define uranusRotateZ 0.0
#define neptuneRotateX sin(glm::radians(28.3f))
#define neptuneRotateY cos(glm::radians(28.3f))
#define neptuneRotateZ 0.0
//size of planet
#define RATE 1.5
#define sunSize 1.00
#define mercurySize 0.697/RATE
#define venusSize 0.981/RATE
#define earthSize 1.000/RATE
#define marsSize 0.79/RATE
#define jupiterSize 2.45/RATE
#define saturnSize 2.286/RATE
#define uranusSize 1.685/RATE
#define neptuneSize 1.658/RATE
#define moonSize 0.250/RATE

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  25.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float sphLocX, sphLocY, sphLocZ;
float lastTime = 0.0f;
GLuint skyboxTexture;
GLuint earthTexture;
GLuint venusTexture;
GLuint sunTexture;
GLuint moonTexture;
GLuint marsTexture;
GLuint jupiterTexture;
GLuint mercuryTexture;
GLuint neptuneTexture;
GLuint plutoTexture;
GLuint saturnTexture;
GLuint uranusTexture;
GLuint renderingProgram;
GLuint backgroundProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, projLoc, nLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
Sphere mySphere = Sphere(48);
stack<glm::mat4> mvStack;
bool firstMouse = true;
GLfloat lastX;
GLfloat lastY;
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;

GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
glm::vec3 lightLoc = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 currentLightPos, lightPosV;
float lightPos[3];
// sun light
float globalAmbient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// gold material
float* matAmb = Utils::goldAmbient();
float* matDif = Utils::goldDiffuse();
float* matSpe = Utils::goldSpecular();
float matShi = Utils::goldShininess();
void keys_callback(GLFWwindow* window, double currentTime);
void planet(double currentTime, float length, float Orbital_speed,
	float self_rotate_speed, float rotateX, float rotateY, float rotateZ, float size, GLuint texture)
{
	mvStack.push(mvStack.top());
	if (texture != sunTexture) {
		if (texture != moonTexture) {
			mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime * Orbital_speed) * length, 0.0f, cos((float)currentTime * Orbital_speed) * length));
		}
		else {
			mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime * Orbital_speed) * length, cos((float)currentTime * Orbital_speed) * length));
		}
		if (texture == earthTexture) {
			mvStack.push(mvStack.top());
		}
		if (texture != moonTexture) {	//moon has no self-rotation
			mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime * self_rotate_speed, glm::vec3(rotateX, rotateY, rotateZ));
		}
		mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(size, size, size));
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	}
	else {
		mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(size, size, size));
		mvStack.push(mvStack.top());
		/*mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(size, size, size));*/
		invTrMat = glm::transpose(glm::inverse(mvStack.top()));
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
		glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	if (texture == sunTexture) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);				//normal
		glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(2);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	//if (texture != sunTexture){
	mvStack.pop();
	//}
	if (texture == moonTexture) {
		mvStack.pop();
	}
}
//void sun(double currentTime) {
//	mvStack.push(mvStack.top());
//	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
//	invTrMat = glm::transpose(glm::inverse(mvStack.top()));
//	
//	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));//
//	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));	//
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);	//pos
//	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);	//tex
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(1);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);				//normal
//	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);	
//	glEnableVertexAttribArray(2);						
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, sunTexture);
//
//	glEnable(GL_CULL_FACE);
//	//glFrontFace(GL_CCW);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//
//	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
//	//mvStack.pop();
//}
void skybox(double currentTime)
{
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime / 15, glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyboxTexture);

	glEnable(GL_CULL_FACE); //for skybox
	glCullFace(GL_FRONT);   //for skybox
	glFrontFace(GL_CCW);	// not CW, because we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	glEnable(GL_DEPTH_TEST);

	mvStack.pop();
}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	cameraPos.z += yoffset;
}

void installLights(glm::mat4 vMatrix) {
	lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = lightPosV.x;
	lightPos[1] = lightPosV.y;
	lightPos[2] = lightPosV.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}

void setupVertices(void) {
	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}	

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	backgroundProgram = Utils::createShaderProgram("vertShader2.glsl", "fragShader2.glsl");;
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	//cameraX = 0.0f; cameraY = 0.0f; cameraZ = 25.0f;
	//sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = -1.0f;
	setupVertices();
	earthTexture = Utils::loadTexture("earth.bmp");
	sunTexture = Utils::loadTexture("sun.jpg");
	moonTexture = Utils::loadTexture("moon.bmp");
	venusTexture = Utils::loadTexture("venus.bmp");
	mercuryTexture = Utils::loadTexture("mercury.bmp");
	marsTexture = Utils::loadTexture("mars.bmp");
	jupiterTexture = Utils::loadTexture("jupiter.bmp");
	saturnTexture = Utils::loadTexture("saturn.bmp");
	uranusTexture = Utils::loadTexture("uranus.bmp");
	neptuneTexture = Utils::loadTexture("neptune.bmp");
	skyboxTexture = Utils::loadTexture("sky_final.jpg");
}

void display(GLFWwindow* window, double currentTime) {
	keys_callback(window, currentTime);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(backgroundProgram);

	mvLoc = glGetUniformLocation(backgroundProgram, "mv_matrix");
	projLoc = glGetUniformLocation(backgroundProgram, "proj_matrix");
	nLoc = glGetUniformLocation(backgroundProgram, "norm_matrix");

	vMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	installLights(vMat);
	mvStack.push(vMat);

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	skybox(currentTime);	//skybox

	glUseProgram(renderingProgram);
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	//Sun
	planet(currentTime, 0, 0, 0, 0, 0, 0, sunSize, sunTexture);
	//Mercury
	planet(currentTime, mercuryLength, mercuryOrbital_speed, mercurySelf_rotate_speed,
		mercuryRotateX, mercuryRotateY, mercuryRotateZ, mercurySize, mercuryTexture);
	//Venus
	planet(currentTime, venusLength, venusOrbital_speed, venusSelf_rotate_speed, 
		venusRotateX, venusRotateY, venusRotateZ, venusSize, venusTexture);
	//Earth
	planet(currentTime, earthLength, earthOrbital_speed, earthSelf_rotate_speed,
		earthRotateX, earthRotateY, earthRotateZ, earthSize, earthTexture);
	//Moon
	planet(currentTime, moonLength, moonOrbital_speed, 0, 0, 0, 0, moonSize, moonTexture);
	//Mars
	planet(currentTime, marsLength, marsOrbital_speed, marsSelf_rotate_speed,
		marsRotateX, marsRotateY, marsRotateZ, marsSize, marsTexture);
	//Jupiter
	planet(currentTime, jupiterLength, jupiterOrbital_speed, jupiterSelf_rotate_speed,
		jupiterRotateX, jupiterRotateY, jupiterRotateZ, jupiterSize, jupiterTexture);
	//Saturn
	planet(currentTime, saturnLength, saturnOrbital_speed, saturnSelf_rotate_speed,
		saturnRotateX, saturnRotateY, saturnRotateZ, saturnSize, saturnTexture);
	//Uranus
	planet(currentTime, uranusLength, uranusOrbital_speed, uranusSelf_rotate_speed,
		uranusRotateX, uranusRotateY, uranusRotateZ, uranusSize, uranusTexture);
	//-----------neptune
	planet(currentTime, neptuneLength, neptuneOrbital_speed, neptuneSelf_rotate_speed,
		neptuneRotateX, neptuneRotateY, neptuneRotateZ, neptuneSize, neptuneTexture);
	
	mvStack.pop();
	mvStack.pop();	// the final pop is for the view matrix
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
		lastX = xpos;
		lastY = ypos;

		GLfloat sensitivity = 0.05;	// Change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
}

void keys_callback(GLFWwindow* window, double currentTime)
{
	float deltaTime = float(currentTime - lastTime);
	lastTime = currentTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos.y += 1.0f * deltaTime * 10;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos.y -= 1.0f * deltaTime * 10;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos.x -= 1.0f * deltaTime * 10;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos.x += 1.0f * deltaTime * 10;
	}
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1000, 1000, "Our Galaxy", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);
	glfwSetScrollCallback(window, ScrollCallback);
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}