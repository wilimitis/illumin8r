#include <glm/ext.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_query.hpp>
#include "object.h"

Object::Object() {
  // Creates the identity matrix.
  localTransformation = glm::mat4(1);
}

Ray Object::getLocal(const Ray &ray) {
  glm::mat4 worldTransformation = glm::inverse(localTransformation);

  // Ignore the translation component for directions.
  // Shirley 6.3
  glm::vec3 localPosition = worldTransformation * glm::vec4(ray.position, 1);
  glm::vec3 localDirection = worldTransformation * glm::vec4(ray.direction, 0);

  Ray local = Ray(glm::normalize(localDirection), localPosition);
  assert(glm::isNormalized(local.direction, 0.1f));
  return local;
}

Ray Object::getWorld(const Ray &ray) {
  // Ignore the translation component for directions.
  // Shirley 6.3
  glm::vec3 worldPosition = localTransformation * glm::vec4(ray.position, 1);
  glm::vec3 worldDirection = localTransformation * glm::vec4(ray.direction, 0);

  Ray world = Ray(glm::normalize(worldDirection), worldPosition);
  assert(glm::isNormalized(world.direction, 0.1f));
  return world;
}

void Object::rotate(float angle, const glm::vec3 &axis) {
  localTransformation = glm::rotate(localTransformation, glm::radians(angle), axis);
}

void Object::scale(const glm::vec3 &scale) {
  // NOTE: Assumed to be uniform.
  localTransformation = glm::scale(localTransformation, scale);
}

void Object::translate(const glm::vec3 &translation) {
  localTransformation = glm::translate(localTransformation, translation);
}
