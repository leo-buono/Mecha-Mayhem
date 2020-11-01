#pragma once
#include "BackEndThings/BackEnd.h"
#include "ECS.h"
#include "ObjLoader.h"

namespace Rendering
{
	void Update(entt::registry* reg);

	extern glm::vec4 BackColour;
	extern glm::vec3 LightPos;
	extern glm::vec3 LightColour;
	extern glm::vec3 DefaultColour;
};

