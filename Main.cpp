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
#include"Spring.h"

const int screenWidth = 800;
const int screenHeight = 800;
#define GRAVITY 10
#define FRICTION 0.0
#define BOUNCE 0.0
GLFWwindow* window;
Camera camera;
const int fps = 60;
Spring fr, fl, br, bl, tr, tl;

void update(Car& car, std::vector<Block>& map);
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	Model model = Model("Models/mesh.obj", "blank.png");
	
	std::vector<Model> models = Model::loadModels("Models/track.obj", "blank.png");
	
	
	
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
	Vertex p1(glm::vec3(0.5f, -0.2f, 0.75f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));
	Vertex p2(glm::vec3(-0.5f, -0.2f, 0.75f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	Vertex p3(glm::vec3(-0.5f, -0.2f, -0.75f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	Vertex p4(glm::vec3(0.5f, -0.2f, -0.75f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	Vertex p5(glm::vec3(0.5f, 0.2f, -0.75f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	Vertex p6(glm::vec3(-0.5f, 0.2f, -0.75f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	Vertex p7(glm::vec3(-0.5f, 0.2f, 0.75f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	Vertex p8(glm::vec3(0.5f, 0.2f, 0.75f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));

	std::vector<Vertex> carV;

	carV.push_back(p1);
	carV.push_back(p2);
	carV.push_back(p3);
	carV.push_back(p4);
	carV.push_back(p5);
	carV.push_back(p6);
	carV.push_back(p7);
	carV.push_back(p8);
	
	Car car = Car("grid.png", carV, indices);
	
	
	//car.addImpulse(glm::vec3(0.2, 0, 0), glm::vec3(-0.0, -0.4, 0.0));
	car.setDirection(glm::vec3(1, 0, 0), 0.0f);
	//car.inverseMass = 1.0f / 10.0f;
	//car.center = glm::vec3(7, 0, 0);
	
	//set point for block
	static Vertex b1(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));
	static Vertex b2(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	static Vertex b3(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	static Vertex b4(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	static Vertex b5(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f));
	static Vertex b6(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f));
	static Vertex b7(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f));
	static Vertex b8(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f));
	
	std::vector<Vertex> blockV;
	blockV.push_back(b1);
	blockV.push_back(b2);
	blockV.push_back(b3);
	blockV.push_back(b4);
	blockV.push_back(b5);
	blockV.push_back(b6);
	blockV.push_back(b7);
	blockV.push_back(b8);

	//create map
	std::vector<Block> map;

	Block block = Block("brick_wall.png", blockV, indices, glm::vec3(0, -1.25f, 0.0), 10, 0.5, 20);
	//map.push_back(block);
	for(int i=1; i < models.size(); i++)
	{
		map.push_back(Block("blank.png", models[i].Vertices, models[i].Indices, models[i].points, glm::vec3(0, -2,5)));
		map[i-1].setDirection(glm::vec3(0,1,0), 180);
	}
	Block block2 = Block("blank.png", blockV, indices, glm::vec3(0, 0.5f, 25.0), 3, 2.5, 0.5);
	//map.push_back(block2);
	
	//create spring
	fr = Spring(0.4f, 200, 0, &car, NULL);
	fl = Spring(0.4f, 200, 0, &car, NULL);
	br = Spring(0.4f, 200, 0, &car, NULL);
	bl = Spring(0.4f, 200, 0, &car, NULL);
	tr = Spring(0.4f, 200, 0, &car, NULL);
	tl = Spring(0.4f, 200, 0, &car, NULL);

	fl.setMap(&map); fr.setP1(&car, 0);
	fr.setMap(&map); fl.setP1(&car, 1);
	br.setMap(&map); bl.setP1(&car, 2);
	bl.setMap(&map); br.setP1(&car, 3);
	tr.setMap(&map); tr.setP1(&car, 6);
	tl.setMap(&map); tl.setP1(&car, 7);
	
	
	

	double lastTime = 0;
	double currentTime = 0;

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		currentTime = glfwGetTime();

		if(currentTime - lastTime > 1.0/fps && !glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			update(car, map);

			draw(renderer3D, car, map);
			
			lastTime = currentTime;
		}
		// Take care of all GLFW events
		glfwPollEvents();
	}

	//close window and glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return -1;
}
void update(Car& car, std::vector<Block>& map)
{
	camera.updateMovement(window);

	car.movement(window, 1.0 / fps);

	//gravity
	car.RigidBody::addForce(glm::vec3(0, -GRAVITY, 0) / car.inverseMass);
	
	//spring force
	fr.applyForce();
	fl.applyForce();
	bl.applyForce();
	br.applyForce();
	tr.applyForce();
	tl.applyForce();
	
	car.onGround = Spring::onGround(fr, fl, br, bl);
	
	

	//friction
	car.applyFriction();

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		//car.applyImpulse(car.center , glm::vec3(0,-0.15,0));
		car.center = glm::vec3(0.0, 20, 0);
	}
	
	//collision
	std::vector<Contact> contacts; 
	/*for (int i = 1; i < map.size(); i++)
	{
		std::vector<Contact> miniContacts = Collision::getContacts(car, map[i]);
		contacts.insert(contacts.end(), miniContacts.begin(), miniContacts.end());
	}*/
	
	fl.makeContact(contacts);
	fr.makeContact(contacts);
	bl.makeContact(contacts);
	br.makeContact(contacts);
	tr.makeContact(contacts);
	tl.makeContact(contacts);

	
	
	Collision::velocityResolver(contacts, BOUNCE, FRICTION);
	//Collision::penetrationResolver(contacts);
	

	//integrate car
	car.Integrate(1.0 / fps);

	// Take care of all GLFW events
	glfwPollEvents();
}

void draw(Shader& shaderProgram, Car& car, std::vector<Block>& map)
{
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw all blocks
	for (auto block : map)
	{
		block.draw(shaderProgram, camera);
	}
	car.draw(shaderProgram, camera);
	
	
	

	// display what was drawn
	glfwSwapBuffers(window);
}