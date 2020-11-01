#include "BackEndThings/Rendering.h"

float Magnitude(const glm::vec3 input) 
{
	return(sqrtf((input.x * input.x + input.z * input.z)));
}



GLFWwindow* window;

int main() {
	srand(time(0));
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	if (!(window = BackEnd::Init("Epic")))	return 1;

	int width = 720, height = 480;

	glfwSetWindowSize(window, width, height);
	//glfwGetWindowSize(window, &width, &height);
	//if (height + width == 0)	return 1;

	ObjLoader::Init();

	entt::registry reg;

	ECS::AttachRegistry(&reg);
	
	/// Creating Entities

	unsigned cameraEnt = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt);
	ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(0, 5, 0));

	//glm::quat rotf = glm::rotate(startQuat, rot.x, glm::vec3(1, 0, 0));
	//rotf = glm::rotate(rotf, rot.y, glm::vec3(0, 1, 0));
	//camTrans.SetRotation(rotf);

	auto playerBlue = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(playerBlue, ObjLoader("blue_paddle_Fix.obj", true));
	ECS::GetComponent<Transform>(playerBlue).SetPosition(glm::vec3(1.f, 1.f, 0.f));

	auto playerRed = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(playerRed, ObjLoader("red_paddle.obj", true));
	ECS::GetComponent<Transform>(playerRed).SetPosition(glm::vec3(4.f, 1.f, 0.f));

	auto arena = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(arena, ObjLoader("airHockey_Arena.obj", true));
	ECS::GetComponent<Transform>(arena).SetPosition(glm::vec3(0.f, 0, 0));
	ECS::GetComponent<Transform>(arena).SetScale(glm::vec3(3, 3, 3));

	auto puck = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(puck, ObjLoader("airHockey_Puck.obj", true));
	//physics only holds member variables for each of the entities, instead of having arrays in the transform
	ECS::AttachComponent<Physics>(puck).setMass(1.f);
	ECS::GetComponent<Physics>(puck).setForce(glm::vec3(0.f, 0.f, 0.f));
	ECS::GetComponent<Transform>(puck).SetPosition(glm::vec3(0, 1.f, 0));

	auto& camTrans = ECS::GetComponent<Transform>(cameraEnt);

	/// End of creating entities

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
		//camCam.SetAspect(float(width) / height);

		///physics
		//f = m * a -> f / m = a
		glm::vec3 puckPos = ECS::GetComponent<Transform>(puck).GetPosition();
		if (ECS::GetComponent<Physics>(puck).getForce() != glm::vec3(0.f) || ECS::GetComponent<Physics>(puck).getAccel() != glm::vec3(0.f))
		{
			glm::vec3 puckAccel = ECS::GetComponent<Physics>(puck).getAccelMath();
			ECS::GetComponent<Physics>(puck).setVel(puckAccel * deltaTime);
		}
		//update player position based on velocity
		ECS::GetComponent<Transform>(puck).SetPosition(puckPos + (ECS::GetComponent<Physics>(puck).getVel() * deltaTime));
		float puckRad = 0.7f;
		float playerRad = 1.f;
		glm::vec3 thing = ECS::GetComponent<Transform>(playerRed).GetPosition() - ECS::GetComponent<Transform>(puck).GetPosition();

		//circle circle collision
		if (puckRad + playerRad > Magnitude(thing))
		{
			glm::vec3 currentVelocity = ECS::GetComponent<Physics>(puck).getVel();
			ECS::GetComponent<Physics>(puck).setVel(-(currentVelocity));
		}

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
		glm::vec3 posPlayer = ECS::GetComponent<Transform>(playerRed).GetPosition();
		if (glfwGetKey(window, GLFW_KEY_J))
		{
			posPlayer.x += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_L))
		{
			posPlayer.x -= 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_I))
		{
			posPlayer.z += 5 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_K))
		{
			posPlayer.z -= 5 * deltaTime;
		}
		ECS::GetComponent<Transform>(playerRed).SetPosition(posPlayer);
		if (glfwGetKey(window, GLFW_KEY_P))
		{
			ECS::GetComponent<Physics>(puck).setVel(glm::vec3(0, 0, 3.f));
		}

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
