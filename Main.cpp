#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include"Shader.h"
#include"Mesh.h"
#include"Model.h"
#include"Car.h"
#include"Camera.h"
#include"Math.h"
#include"RigidBody.h"
#include"Quaternion.h"
#include"Collision.h"
#include"Block.h"
#include"Grid.h"
#include"QuadTree.h"

const int screenWidth = 800;
const int screenHeight = 800;
#define GRAVITY 10
#define FRICTION 0.0
#define BOUNCE 0.0
GLFWwindow* window;
Camera camera;
const int fps = 60;

void update(float deltaTime, Car& car, QuadTree& tree, Grid& grid, std::vector<Model>& collisionMap);
void draw(Shader& shaderProgram, Car& car, std::vector<Model>& map);

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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	RigidBody empty = RigidBody();
	Collision::empty = &empty;

	//load models
	std::vector<Model> modelMap = Model::loadModels("Models/gameTrackMap.obj", "Textures/blank.png");
	std::vector<Model> collisionMap = Model::loadModels("Models/gameTrack.obj", "Textures/wall_base_color.png");

	modelMap[0].texture = Texture("Textures/road.png");
	modelMap[1].texture = Texture("Textures/wall_base_color.png");
	modelMap[2].texture = Texture("Textures/wall_base_color.png");
	modelMap[3].texture = Texture("Textures/startLine.png");
	modelMap[4].texture = Texture("Textures/terrain.png");

	//create quadtree of map
	HitBox mapZone = HitBox(glm::vec3(-300), 600, 600, 600);

	//insert map into tree
	std::vector<HitBox> hitboxList = HitBox::convertToHitbox(collisionMap);

	QuadTree tree = QuadTree(mapZone, 0, &hitboxList);
	tree.insert(hitboxList);
	
	//tree.print(0);
	Grid grid = Grid(2, &hitboxList);
	
	
	camera = Camera(glm::vec3(0.0f, 1.0f, -2.0f), glm::vec3(0.0f, -0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), screenWidth, screenHeight);

	Shader Default = Shader("default.vert", "default.frag");
	Shader renderer3D = Shader("render3d.vert", "render3d.geom", "render3d.frag");
	//set indices for cube
	static GLuint Indices[] =
	{
		4,5,6,
		6,7,4,
		0,1,2,
		2,3,0,
		2,1,6,
		6,5,2,
		0,3,4,
		4,7,0,
		3,2,5,
		5,4,3,
		1,0,7,
		7,6,1
	};
	std::vector<GLuint> indices;
	for (int i = 0; i < std::size(Indices); i++)
	{
		indices.push_back(Indices[i]);
	}
	//set points for car
	Vertex p1(glm::vec3(0.5f, -0.2f, 1.2f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));
	Vertex p2(glm::vec3(-0.5f, -0.2f, 1.2f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	Vertex p3(glm::vec3(-0.5f, -0.2f, -1.2f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	Vertex p4(glm::vec3(0.5f, -0.2f, -1.2f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	Vertex p5(glm::vec3(0.5f, 0.2f, -1.2f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	Vertex p6(glm::vec3(-0.5f, 0.2f, -1.2f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	Vertex p7(glm::vec3(-0.5f, 0.2f, 1.2f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	Vertex p8(glm::vec3(0.5f, 0.2f, 1.2f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));

	std::vector<Vertex> carV;

	carV.push_back(p1);
	carV.push_back(p2);
	carV.push_back(p3);
	carV.push_back(p4);
	carV.push_back(p5);
	carV.push_back(p6);
	carV.push_back(p7);
	carV.push_back(p8);
	
	Car car = Car("Textures/grid.png", carV, indices);
	Model carModel = Model("Models/car.obj", "Textures/car_tex.png");
	car.carModel = &carModel;
	car.center = glm::vec3(0, 0.5, 0);
	car.setDirection(glm::vec3(0, 1, 0), -90.0f);

	

	double lastTime = 0;
	double lastFRAME = 0;
	double currentTime = 0;
	int fpsCounter = 1;
	int currentFps = fps;

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();

		//enter game tick
		if(currentTime - lastTime > 1.0/fps && !glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			update(1.0f / fps, car, tree, grid, collisionMap);

			draw(renderer3D, car, modelMap);
			
			lastTime = currentTime;

			fpsCounter++;
		}

		//fps counter
		if (currentTime - lastFRAME > 1.0f)
		{
			std::string title = "OPENGL FPS:" + std::to_string(fpsCounter);
			glfwSetWindowTitle(window, title.c_str());
			fpsCounter = 0;
			lastFRAME =  currentTime;
		}

		// Take care of all GLFW events
		glfwPollEvents();
	}

	//close window and glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return -1;
}
void update(float deltaTime, Car& car, QuadTree& tree, Grid& grid, std::vector<Model>& collisionMap)
{
	//gravity
	car.RigidBody::addForce(glm::vec3(0, -GRAVITY, 0) / car.inverseMass);

	//downforce
	car.addForce(car.rotation * glm::vec3(0,-1,0) * (float)std::pow(glm::length(car.velocity), 2) * 0.4f);

	//friction
	car.applyFriction();

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		car.velocity += glm::vec3(0, 0, 1);
		//car.center = glm::vec3(0.0, 20, 0);
	}
	
	//integrate car
	car.Integrate(deltaTime);

	//COLLISION
	
	//get data from qudatree/grid
	std::vector<int> zone = Collision::collisionDetector(tree, grid, car);

	/*for (int i = 1; i < map.size(); i++)
	{
		std::vector<Contact> miniContacts = Collision::getContacts(car, map[i]);
		contacts.insert(contacts.end(), miniContacts.begin(), miniContacts.end());
	}*/

	std::vector<Contact> groundContacts = car.generateGroundContacts(0.4f, zone, collisionMap); 
	std::vector<Contact> wallContacts = car.generateWallContacts(0.4f, zone, collisionMap);
	std::vector<Contact> contacts;
	contacts.insert(contacts.end(), groundContacts.begin(), groundContacts.end());
	contacts.insert(contacts.end(), wallContacts.begin(), wallContacts.end());

	Collision::velocityResolver(contacts, BOUNCE, FRICTION);

	Collision::penetrationResolver(contacts);

	car.movement(window, deltaTime);
	camera.updateMovement(car);
	car.updateCarModel();
	//Math::print(car.center);
	// Take care of all GLFW events
	glfwPollEvents();
}

void draw(Shader& shaderProgram, Car& car, std::vector<Model>& map)
{
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw all blocks
	for (int i=0; i< map.size(); i++)
	{
		map[i].draw(shaderProgram, camera);
	}
	car.carModel->draw(shaderProgram, camera);

	
	
	// display what was drawn
	glfwSwapBuffers(window);
}