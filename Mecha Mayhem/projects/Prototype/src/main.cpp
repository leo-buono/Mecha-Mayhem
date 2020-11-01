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

	int width = 720, height = 500;

	glfwSetWindowSize(window, width, height);
	//glfwGetWindowSize(window, &width, &height);
	//if (height + width == 0)	return 1;

	ObjLoader::Init();

	entt::registry reg;

	ECS::AttachRegistry(&reg);
	
	/// Creating Entities

	unsigned cameraEnt = ECS::CreateEntity();
	ECS::AttachComponent<Camera>(cameraEnt);
	ECS::GetComponent<Camera>(cameraEnt).SetOrtho(35);
	ECS::GetComponent<Camera>(cameraEnt).ChangePerspective(CAMERA_ORTHO);
	ECS::GetComponent<Transform>(cameraEnt).SetPosition(glm::vec3(0, 10.f, 0));
	//rotation
	ECS::GetComponent<Transform>(cameraEnt).SetRotation(glm::quat(0, 1, 0, 0));




	auto playerBlue = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(playerBlue, ObjLoader("models/blue_paddle_Fix.obj", true));
	ECS::GetComponent<Transform>(playerBlue).SetPosition(glm::vec3(3.f, -1.f, 0.f));
	ECS::AttachComponent<Physics>(playerBlue);
	

	auto playerRed = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(playerRed, ObjLoader("models/red_paddle.obj", true));
	ECS::GetComponent<Transform>(playerRed).SetPosition(glm::vec3(4.f, -1.f, 0.f));
	ECS::AttachComponent<Physics>(playerRed);

	auto arena = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(arena, ObjLoader("models/airHockey_ArenaTriangulated.obj", true));
	ECS::GetComponent<Transform>(arena).SetPosition(glm::vec3(0.f, -2.f, 0));
	ECS::GetComponent<Transform>(arena).SetScale(glm::vec3(4, 4, 4));

	auto puck = ECS::CreateEntity();
	ECS::AttachComponent<ObjLoader>(puck, ObjLoader("models/airHockey_Puck.obj", true));
	//physics only holds member variables for each of the entities, instead of having arrays in the transform
	ECS::AttachComponent<Physics>(puck).setMass(1.f);
	ECS::GetComponent<Physics>(puck).setForce(glm::vec3(0.f, 0.f, 0.f));
	ECS::GetComponent<Transform>(puck).SetPosition(glm::vec3(0, -1.f, 0));
	ECS::GetComponent<Physics>(puck).setFriction(glm::vec3(1.f, 0.f, 1.f));

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
		//puck physics
		glm::vec3 puckPos = ECS::GetComponent<Transform>(puck).GetPosition();
		if (ECS::GetComponent<Physics>(puck).getForce() != glm::vec3(0.f) || ECS::GetComponent<Physics>(puck).getAccel() != glm::vec3(0.f))
		{
			glm::vec3 puckAccel = ECS::GetComponent<Physics>(puck).getAccelMath();
			ECS::GetComponent<Physics>(puck).setVel(puckAccel * deltaTime);
		}
		glm::vec3 vel = ECS::GetComponent<Physics>(puck).getVel();
		//decelleration thingy
		ECS::GetComponent<Physics>(puck).setVel(vel + ((-vel * 0.4f) * deltaTime));
		//update puck position based on velocity
		ECS::GetComponent<Transform>(puck).SetPosition(puckPos + (ECS::GetComponent<Physics>(puck).getVel() * deltaTime));

		//red
		if (ECS::GetComponent<Physics>(playerRed).getForce() != glm::vec3(0.f) || ECS::GetComponent<Physics>(playerRed).getAccel() != glm::vec3(0.f))
		{
			glm::vec3 redAccel = ECS::GetComponent<Physics>(playerRed).getAccelMath();
			ECS::GetComponent<Physics>(playerRed).setVel(redAccel * deltaTime);
		}
		ECS::GetComponent<Transform>(playerRed).SetPosition(ECS::GetComponent<Transform>(playerRed).GetPosition() + (ECS::GetComponent<Physics>(playerRed).getVel() * deltaTime));

		if (ECS::GetComponent<Physics>(playerBlue).getForce() != glm::vec3(0.f) || ECS::GetComponent<Physics>(playerBlue).getAccel() != glm::vec3(0.f))
		{
			glm::vec3 blueAccel = ECS::GetComponent<Physics>(playerBlue).getAccelMath();
			ECS::GetComponent<Physics>(playerBlue).setVel(blueAccel * deltaTime);
		}
		ECS::GetComponent<Transform>(playerBlue).SetPosition(ECS::GetComponent<Transform>(playerBlue).GetPosition() + (ECS::GetComponent<Physics>(playerBlue).getVel() * deltaTime));
		

		//circle circle collision
		float puckRad = 0.7f;
		float playerRad = 1.f;
		glm::vec3 thing = ECS::GetComponent<Transform>(playerRed).GetPosition() - ECS::GetComponent<Transform>(puck).GetPosition();
		if (puckRad + playerRad > Magnitude(thing))
		{
			glm::vec3 currentVelocity = ECS::GetComponent<Physics>(puck).getVel();
			if (ECS::GetComponent<Physics>(playerRed).getVel() != glm::vec3(0.f))
			{
				ECS::GetComponent<Physics>(puck).setVel(ECS::GetComponent<Physics>(playerRed).getVel() * 1.5f);
			}
			else 
			{
				ECS::GetComponent<Physics>(puck).setVel(-(currentVelocity) * 1.5f);
			}
		}
		thing = ECS::GetComponent<Transform>(playerBlue).GetPosition() - ECS::GetComponent<Transform>(puck).GetPosition();
		if (puckRad + playerRad > Magnitude(thing))
		{
			glm::vec3 currentVelocity = ECS::GetComponent<Physics>(puck).getVel();
			if (ECS::GetComponent<Physics>(playerBlue).getVel() != glm::vec3(0.f))
			{
				ECS::GetComponent<Physics>(puck).setVel(ECS::GetComponent<Physics>(playerBlue).getVel() * 1.5f);
			}
			else
			{
				ECS::GetComponent<Physics>(puck).setVel(-(currentVelocity) * 1.5f);
			}
		}
		thing = ECS::GetComponent<Transform>(playerBlue).GetPosition() - ECS::GetComponent<Transform>(playerRed).GetPosition();
		if (playerRad + playerRad > Magnitude(thing))
		{
			//if players collide
		}
		 

		//RED PLAYER MOVEMENT
		glm::vec3 velPlayer = ECS::GetComponent<Physics>(playerRed).getVel();
		float moveAmount = 7;
		if (glfwGetKey(window, GLFW_KEY_J))
		{
			velPlayer.x = moveAmount;
		}
		if (glfwGetKey(window, GLFW_KEY_L))
		{
			velPlayer.x = -moveAmount;
		}
		if (glfwGetKey(window, GLFW_KEY_I))
		{
			velPlayer.z = moveAmount;
		}
		if (glfwGetKey(window, GLFW_KEY_K))
		{
			velPlayer.z = -moveAmount;
		}
		if ((!(glfwGetKey(window, GLFW_KEY_K)) && !(glfwGetKey(window, GLFW_KEY_I)) && !(glfwGetKey(window, GLFW_KEY_L)) && !(glfwGetKey(window, GLFW_KEY_J))))
		{
			velPlayer = glm::vec3(0.f);
		}
		ECS::GetComponent<Physics>(playerRed).setVel(velPlayer);
		velPlayer = ECS::GetComponent<Physics>(playerBlue).getVel();
		//blue player position
		if (glfwGetKey(window, GLFW_KEY_W)) 
		{
			velPlayer.z = moveAmount;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) 
		{
			velPlayer.z = -moveAmount;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) 
		{
			velPlayer.x = moveAmount;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) 
		{
			velPlayer.x = -moveAmount;
		}
		if ((!(glfwGetKey(window, GLFW_KEY_W)) && !(glfwGetKey(window, GLFW_KEY_A)) && !(glfwGetKey(window, GLFW_KEY_S)) && !(glfwGetKey(window, GLFW_KEY_D))))
		{
			velPlayer = glm::vec3(0.f);
		}
		ECS::GetComponent<Physics>(playerBlue).setVel(velPlayer);




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
	
		if (glfwGetKey(window, GLFW_KEY_P))
		{
			ECS::GetComponent<Physics>(puck).setVel(glm::vec3(0, 0, 3.f));
		}

		glm::vec3 pos = glm::vec3(0.f);
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
