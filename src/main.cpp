// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define GLEW_STATIC

#include "texturedMesh.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <string>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/quaternion.hpp>

// Defines several possible options for freeCamera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default freeCamera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract freeCamera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class freeCamera
{
public:
	// freeCamera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// freeCamera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with vectors
	freeCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	freeCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	inline glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of freeCamera defined ENUM (to abstract it from windowing systems)
	inline void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// processes rotational input. Expects the offset value in both the x and y direction.
	inline void ProcessOrientation(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}



private:
	// calculates the front vector from the freeCamera's (updated) Euler Angles
	inline void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

//initializes the camera
freeCamera Camera;

//declares the events class manager
class events
{
public:
	events() = default;
	~events() = default;
	events(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, (GLFWkeyfun)setKey);
	}


	static void setKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			events::pressedKey = key;
		}
	}

	bool checkKey(int key)
	{
		if (key == pressedKey)
		{
			pressedKey = GL_ZERO;
			return true;
		}
		return false;
	}


public:
	static int pressedKey;
};
int events::pressedKey = 0;
//initializes an event manager
events dispachter;

//function call to make camera movements
void manageEvents(float ts)
{
	if (dispachter.checkKey(GLFW_KEY_LEFT))
	{
		Camera.ProcessOrientation(ts * -3000, 0.0f);
	}
	else if (dispachter.checkKey(GLFW_KEY_RIGHT))
	{
		Camera.ProcessOrientation(ts * 3000, 0.0f);
	}

	else if (dispachter.checkKey(GLFW_KEY_UP))
	{
		Camera.ProcessKeyboard(Camera_Movement::FORWARD, ts * 10);
	}

	else if (dispachter.checkKey(GLFW_KEY_DOWN))
	{
		Camera.ProcessKeyboard(Camera_Movement::BACKWARD, ts * 10);
	}

}


// main function
// sets up window to which we'll draw
int main(int argc, char** argv)
{
	//............................................................................
	if (!glfwInit()) {
		std::cout << "initialization failed!" << std::endl;
		return 0;
	}
	//................................................................................
	GLFWwindow* m_window = glfwCreateWindow(1024, 768, "pyramid", nullptr, nullptr);

	int max, min, r;
	glfwGetVersion(&max, &min, &r);
	//..............................................................................
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, max);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, min);
	glfwWindowHint(GLFW_SAMPLES, 4);

	std::cout << "version >> " << max << "." << min << std::endl;
	//...............................................................
	glfwMakeContextCurrent(m_window);

	glfwSetErrorCallback((GLFWerrorfun)[](int error_code, const char* description) {
		fprintf(stderr, "Error: %s\n", description);
		});

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//.............................................................
	if (!(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)))
	{
		std::cout << "error opengl loader not initialized" << std::endl;
	}


	//...........................................................
	//std::cout << glGetString(GL_VERSION) << std::endl;
	dispachter = events(m_window);

	Camera.Position = glm::vec3(0.5f, 0.4f, 0.5f);
	Camera.Yaw = 0.0f;
	Camera.Pitch = 0.0f;

	//...........................................................
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f/ 3.0f, 0.1f, 1000.0f);
	glm::mat4 model = glm::mat4(1.0f), view = glm::mat4(1.0f);
	//.............................................................

	TexturedMesh walls("asset/Walls.ply", "asset/walls.bmp");
	TexturedMesh floor("asset/Floor.ply", "asset/floor.bmp");
	TexturedMesh table("asset/Table.ply", "asset/table.bmp");
	TexturedMesh wood_objects("asset/WoodObjects.ply", "asset/woodobjects.bmp");
	TexturedMesh bottles("asset/Bottles.ply", "asset/bottles.bmp");
	TexturedMesh patio("asset/Patio.ply", "asset/patio.bmp");
	TexturedMesh WindowBG("asset/WindowBG.ply", "asset/WindowBG.bmp");
	
	//transparent objects
	TexturedMesh Curtains("asset/Curtains.ply", "asset/curtains.bmp");
	TexturedMesh MetalObjects("asset/MetalObjects.ply", "asset/metalobjects.bmp");
	TexturedMesh doorBG("asset/DoorBG.ply", "asset/doorbg.bmp");

	float deltaTime, currentFrame, lastFrame = 0;

	GLcall(glEnable(GL_DEPTH_TEST));
	GLcall(glDepthFunc(GL_LESS));
	GLcall(glDepthRange(0.0f, 1.0f));
	GLcall(glDepthMask(1));

	while (!glfwWindowShouldClose(m_window))
	{
		// clear the color buffer before each drawing ////////////////////////////
		GLcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLcall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//events////////////////////////////////////////
		manageEvents(deltaTime);

		//escape the program
		if (dispachter.checkKey(GLFW_KEY_ESCAPE))
		{
			break;
		}

		//disable blending
		GLcall(glDisable(GL_BLEND));
		//sets view///////////////////////////
		view = Camera.GetViewMatrix();

		//renders models
		walls.draw(projection * view * model);
		floor.draw(projection * view * model);
		table.draw(projection * view * model);
		wood_objects.draw(projection * view * model);
		bottles.draw(projection * view * model);
		WindowBG.draw(projection * view * model);

		//enables blending
		GLcall(glEnable(GL_BLEND));
		GLcall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//draws transparent objects
		doorBG.draw(projection * view * model);
		Curtains.draw(projection * view * model);
		MetalObjects.draw(projection * view * model);

		//enables and disables wire frame mode
		//..........press u to enable it and see the lines
		//..........press y to disable it and see the complete onbjects
		if (dispachter.checkKey(GLFW_KEY_U))
		{
			GLcall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		}
		else if (dispachter.checkKey(GLFW_KEY_Y))
		{
			GLcall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		}


		// swap the buffers and hence show the buffers
		// content to the screen
		glfwSwapBuffers(m_window);
		//polls all events recorded
		glfwPollEvents();
	}

	return 0;
}
