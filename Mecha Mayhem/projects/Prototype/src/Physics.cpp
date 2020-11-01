#include "Physics.h"
void Physics::setMass(float mass)
{
	m_mass = mass;
}
void Physics::setForce(glm::vec3 force)
{
	m_force = force;
}
void Physics::addForce(glm::vec3 forceAdd)
{
	m_force += forceAdd;
}
glm::vec3 Physics::getForce()
{
	return m_force;
}
//finds accel through force and mass, sets it, and returns
glm::vec3 Physics::getAccelMath()
{
	//no divide by 0 errors
	if (m_mass != 0)
	{
		m_accel = m_force / m_mass;
	}
	return (m_accel);
}
glm::vec3 Physics::getVel()
{
	return glm::vec3(m_vel);
}
void Physics::setVel(glm::vec3 vel)
{
	m_vel = vel;
}
glm::vec3 Physics::getAccel()
{
	return glm::vec3(m_accel);
}
void Physics::setAccel(glm::vec3 accel)
{
	m_accel = accel;
}
glm::vec3 Physics::getFriction()
{
	return glm::vec3(m_frictionForce);
}
void Physics::setFriction(glm::vec3 frictionForce)
{
	m_frictionForce = frictionForce; 
}
Physics::Physics()
{
	
}
