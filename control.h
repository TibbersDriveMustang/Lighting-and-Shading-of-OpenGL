#ifndef CONTROL_H
#define CONTROL_H
#include "glm/glm.hpp"
void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::mat3 getRotationMatrix();
glm::vec3 getCameraPosition();
glm::vec3 getDirection();
glm::vec2 getlightButtons();
#endif // CONTROL_H
