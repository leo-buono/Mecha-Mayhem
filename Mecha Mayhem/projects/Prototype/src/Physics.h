#pragma once
#include "BackEndThings/BackEnd.h"
class Physics
{
public:
	void setMass(float mass);
	void setForce(glm::vec3 force);
	void addForce(glm::vec3 forceAdd);
	glm::vec3 getForce();
	//finds accel through force and mass, sets it, and returns
	glm::vec3 getAccelMath();
	//returns the current velocity
	glm::vec3 getVel();
	//sets velocity
	void setVel(glm::vec3 vel);
	//returns the current accel
	glm::vec3 getAccel();
	void setAccel(glm::vec3 accel);

	glm::vec3 getFriction();
	void setFriction(glm::vec3 frictionForce);

	Physics::Physics();

private:
	float m_mass;
	glm::vec3 m_force;
	glm::vec3 m_vel;
	glm::vec3 m_accel;
	glm::vec3 m_frictionForce;
};


