#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "object.h"

Object::Object()
{
  // Creates the identity matrix.
  localTransformation = glm::mat4(1);
}

void Object::rotate(float angle, const glm::vec3 &axis)
{
  localTransformation = glm::rotate(localTransformation, angle, axis);
}

void Object::scale(const glm::vec3 &scale)
{
  localTransformation = glm::scale(localTransformation, scale);
}

void Object::translate(const glm::vec3 &translation)
{
  localTransformation = glm::translate(localTransformation, translation);
}

Ray Object::getLocal(const Ray &ray)
{
  glm::mat4 inverseTransformation = glm::inverse(localTransformation);
  glm::vec3 position = glm::vec3(localTransformation[3]);

  // Ignore the translation component for directions.
  // Shirley 6.3
  glm::vec3 localPosition = inverseTransformation * glm::vec4(ray.position, 1);
  glm::vec3 localDirection = inverseTransformation * glm::vec4(ray.direction, 0);

  return Ray(localDirection, localPosition);
}

Ray Object::getWorld(const Ray &ray)
{
  // TODO
}
