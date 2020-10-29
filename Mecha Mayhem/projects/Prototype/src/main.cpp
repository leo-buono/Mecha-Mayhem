#include "BackEndThings/Rendering.h"

GLFWwindow* window;

int main() {
	srand(time(0));
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	if (!(window = BackEnd::Init("Mecha Mayhem")))	return 1;

	int width = 720, height = 480;

	glfwSetWindowSize(window, width, height);
	//glfwGetWindowSize(window, &width, &height);
	//if (height + width == 0)	return 1;

	ObjLoader::Init();

	entt::registry reg;

	ECS::AttachRegistry(&reg);
	
	unsigned cameraEnt = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt);

	auto playerBlue = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(playerBlue, ObjLoader("blue_paddle.obj", true));
	ECS::GetComponent<Transform>(playerBlue).SetPosition(glm::vec3(0, 0, 0));


	auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);

	float lastClock = glfwGetTime();

	constexpr float pi = glm::half_pi<float>() - 0.01f;
	glm::quat startQuat = glm::rotation(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
	bool change = true;
	glm::vec2 rot = glm::vec2(0.f);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float deltaTime = glfwGetTime() - lastClock;
		lastClock = glfwGetTime();

		/// Start of loop
		glfwGetWindowSize(window, &width, &height);
		camCam.SetAspect(float(width) / height);


		if (glfwGetKey(window, GLFW_KEY_UP)) {
			rot.x += 2.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			rot.x -= 2.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			rot.y -= 2.f * deltaTime;
			change = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			rot.y += 2.f * deltaTime;
			change = true;
		}
		if (rot.x > pi) {
			rot.x = pi;
		}
		else if (rot.x < -pi) {
			rot.x = -pi;
		}
		if (change) {
			glm::quat rotf = glm::rotate(startQuat, rot.x, glm::vec3(1, 0, 0));
			rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
			camTrans.SetRotation(rotf);
			change = false;
		}
		/*glm::mat4 rotf = glm::rotate(glm::mat4(1.f), rot.x, glm::vec3(1, 0, 0));
		rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
		camTrans.SetRotation(glm::mat3(rotf));*/

		
		glm::vec3 pos = glm::vec3(0.f);
		if (glfwGetKey(window, GLFW_KEY_W)) {
			pos.z += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			pos.z -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			pos.x += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			pos.x -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
			pos.y -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			pos.y += 5 * deltaTime;
		}
		if (pos.x != 0 || pos.y != 0 || pos.z != 0) {
			pos = glm::vec4(pos, 1) * glm::rotate(glm::mat4(1.f), rot.y, glm::vec3(0, 1, 0));
			camTrans.SetPosition(camTrans.GetPosition() + pos);
		}

		/// End of loop

		Rendering::Update(&reg);

		glfwSwapBuffers(window);
	}

	Shader::UnBind();
	VertexArrayObject::UnBind();


	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
} 
