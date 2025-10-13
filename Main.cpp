#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include"Shader.h"
#include"Mesh.h"
#include"Car.h"
#include"Camera.h"
#include"Wall.h"
#include"Math.h"
#include"RigidBody.h"
#include"Quaternion.h"
#include"Cube.h"
#include"Collision.h"
#include"spring.h"
#include"Block.h"

const int screenWidth = 800;
const int screenHeight = 800;
GLFWwindow* window;
Camera camera;
const int fps = 60;


void update(Car& car, std::vector<Block>& map, spring& spring);
void draw(Shader& shaderProgram, Car& car, std::vector<Block>& map);

int main()
{
	//initialize glfw
	glfwInit();
	//specify the version of opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//tel opengl we are using only core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//create a glfwWindow with size and name
	window = glfwCreateWindow(screenWidth, screenHeight, "openGl", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "falied to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	//openGL settings
	glViewport(0, 0, screenWidth, screenHeight); 
	glClearColor(0.3f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	camera = Camera(glm::vec3(0.0f, 1.0f, -2.0f), glm::vec3(0.0f, -0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), screenWidth, screenHeight);

	Shader Default = Shader("default.vert", "default.frag");
	Shader renderer3D = Shader("render3d.vert", "render3d.geom", "render3d.frag");

	Vertex p1(glm::vec3(0.25f, -0.2f, 0.25f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));
	Vertex p2(glm::vec3(-0.25f, -0.2f, 0.25f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	Vertex p3(glm::vec3(-0.25f, -0.2f, -0.25f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	Vertex p4(glm::vec3(0.25f, -0.2f, -0.25f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	Vertex p5(glm::vec3(0.25f, 0.2f, -0.25f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	Vertex p6(glm::vec3(-0.25f, 0.2f, -0.25f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	Vertex p7(glm::vec3(-0.25f, 0.2f, 0.25f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	Vertex p8(glm::vec3(0.25f, 0.2f, 0.25f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));

	Vertex w1(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));
	Vertex w2(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	Vertex w3(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	Vertex w4(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));

	Texture blank = Texture("blank.png");
	Texture grid = Texture("grid.png");
	Texture brickWall = Texture("brick_wall.png");

	Car car = Car(grid, p1, p2, p3, p4, p5, p6, p7, p8);
	//car.addImpulse(glm::vec3(0.0, 0, 0), glm::vec3(-0.0, -0.4, 0.0));
	//car.setDirection(glm::vec3(0, 1, 0), 90.0f);

	spring spring(&car, nullptr, 0.2f, 10);

	std::vector<Block> map;

	Block block = Block(brickWall, glm::vec3(0.0, -1.25f, 0),10, 0.5, 20);
	map.push_back(block);
	Block block2 = Block(blank, glm::vec3(0, -1.2f, 1.5), 1, 2, 1);
	block2.setDirection(glm::vec3(1, 0, 0), 70.0f);
	map.push_back(block2);

	

	double lastTime = 0;
	double currentTime = 0;

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();
		
		if(currentTime - lastTime > 1.0/fps)
		{
			update(car, map, spring);

			draw(renderer3D, car, map);

			lastTime = currentTime;
		}

	}

	//close window and glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return -1;
}
void update(Car& car, std::vector<Block>& map, spring& spring)
{
	camera.updateMovement(window);

	car.movement(window);
	//gravity
	car.addForce(glm::vec3(0, -2, 0));
	car.Integrate(1.0f / fps);
	
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		car.addImpulse(car.center, glm::vec3(0,-0.2,0));
	}
	
	std::vector<Contact> contacts; 
	for (int i = 0; i < map.size(); i++)
	{
		std::vector<Contact> miniContacts = Collision::getContacts(car, map[i]);
		contacts.insert(contacts.end(), miniContacts.begin(), miniContacts.end());
		//apply spring force
		//spring.applyForce(car.worldSpace(car.p1.position), Collision::wheelContact(car.worldSpace(car.frontLeft) , -car.worldSpace(car.frontLeft) + car.worldSpace(car.p1.position), map[i]));
		//spring.applyForce(car.worldSpace(car.p2.position), Collision::wheelContact(car.worldSpace(car.frontRight), -car.worldSpace(car.frontRight) + car.worldSpace(car.p2.position), map[i]));
		//spring.applyForce(car.worldSpace(car.p3.position), Collision::wheelContact(car.worldSpace(car.backRight), -car.worldSpace(car.backRight) + car.worldSpace(car.p3.position), map[i]));
		//spring.applyForce(car.worldSpace(car.p4.position), Collision::wheelContact(car.worldSpace(car.backLeft), -car.worldSpace(car.backLeft) + car.worldSpace(car.p4.position), map[i]));
	}

	Collision::velocityResolver(contacts, 0, 0.1);
	Collision::penetrationResolver(contacts);
	

	// Take care of all GLFW events
	glfwPollEvents();
}

void draw(Shader& shaderProgram, Car& car, std::vector<Block>& map)
{
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	car.draw(shaderProgram, camera);
	for(int i=0; i<map.size(); i++)
		map[i].draw(shaderProgram, camera);
	
	

	// display what was drawn
	glfwSwapBuffers(window);
}