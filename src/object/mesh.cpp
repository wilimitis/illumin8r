#define TINYOBJLOADER_IMPLEMENTATION
#include <glm/glm.hpp>
#include <iostream>
#include "../accelerationStructure/bvh.h"
#include "mesh.h"

Box Mesh::Triangle::computeBox() {
	Box b;
	b.min = glm::vec3(
		std::min({vertices[0].x, vertices[1].x, vertices[2].x}),
		std::min({vertices[0].y, vertices[1].y, vertices[2].y}),
		std::min({vertices[0].z, vertices[1].z, vertices[2].z}));
	b.max = glm::vec3(
		std::max({vertices[0].x, vertices[1].x, vertices[2].x}),
		std::max({vertices[0].y, vertices[1].y, vertices[2].y}),
		std::max({vertices[0].z, vertices[1].z, vertices[2].z}));
	return b;
}

Hit Mesh::Triangle::intersects(const Ray &ray) {
  Hit h;

	// http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
	float eta = 0.000001;
	glm::vec3 e1 = vertices[1] - vertices[0];
	glm::vec3 e2 = vertices[2] - vertices[0];

	glm::vec3 p = glm::cross(ray.direction, e2);
	float d = glm::dot(e1, p);

	float u;
	float v;
	float z;
	if (cull) {
		if (d < eta) {
			return h;
		}

		glm::vec3 t = ray.position - vertices[0];
		u = glm::dot(t, p);
		if (u < 0 || u > d) {
			return h;
		}

		glm::vec3 q = glm::cross(t, e1);
		v = glm::dot(ray.direction, q);
		if (v < 0 || u + v > d) {
			return h;
		}

		z = glm::dot(e2, q);
		float di = 1.0 / d;
		z *= di;
		u *= di;
		v *= di;
	} else {
		if (d > -eta && d < eta) {
			return h;
		}

		float di = 1.0 / d;
		glm::vec3 t = ray.position - vertices[0];
		u = glm::dot(t, p) * di;
		if (u < 0 || u > 1) {
			return h;
		}

		glm::vec3 q = glm::cross(t, e1);
		v = glm::dot(ray.direction, q) * di;
		if (v < 0 || u + v > 1) {
			return h;
		}

		z = glm::dot(e2, q) * di;
	}

	if (z < 0) {
		return h;
	}

	h.distance = z;
	h.position = ray.position + h.distance * ray.direction;
	h.normal = (1 - u - v) * normals[0] + u * normals[1] + v * normals[2];
  h.isEmpty = false;
	return h;
}

Box Mesh::computeBox() {
	Box b;
	// TODO
	return b;
}

Hit Mesh::intersects(const Ray &ray) {
	if (accelerationStructure) {
		return accelerationStructure->intersects(ray);
	}
	Hit result;
	for (int i = 0; i < objects.size(); i++) {
		Hit hit = objects.at(i)->intersects(ray);
		if (result.isEmpty || hit.distance < result.distance) {
			result = hit;
		}
	}
	return result;
}

void Mesh::init(const char* file) {
	tinyobj::attrib_t attrib;
  std::vector<tinyobj::material_t> materials;
  std::vector<tinyobj::shape_t> shapes;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file);
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	for (int shapeIndex = 0; shapeIndex < shapes.size(); shapeIndex++) {
		int vertexIndexOffset = 0;
		int faceCount = shapes[shapeIndex].mesh.num_face_vertices.size();
		for (int faceIndex = 0; faceIndex < faceCount; faceIndex++) {
			Triangle* triangle = new Triangle();
			// Assumed to equal 3 as the object loader triangulates by default.
			// TODO: Manually compute normals if not provided.
			int vertexCount = Triangle::vertexCount;
			for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
				tinyobj::index_t index = shapes[shapeIndex].mesh.indices[vertexIndexOffset + vertexIndex];
				triangle->vertices[vertexIndex] = glm::vec3(
					attrib.vertices[vertexCount * index.vertex_index],
					attrib.vertices[vertexCount * index.vertex_index + 1],
					attrib.vertices[vertexCount * index.vertex_index + 2]);
				triangle->normals[vertexIndex] = glm::vec3(
					attrib.normals[vertexCount * index.normal_index],
					attrib.normals[vertexCount * index.normal_index + 1],
					attrib.normals[vertexCount * index.normal_index + 2]);
			}
			objects.push_back(triangle);
			vertexIndexOffset += vertexCount;
		}
	}

	accelerationStructure = new BVH(objects);
}
