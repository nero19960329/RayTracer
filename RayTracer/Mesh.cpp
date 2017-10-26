#include "mesh.h"

#include <algorithm>
#include <iostream>

#include <glm.hpp>

Mesh::Mesh(Texture * texture_, const std::string & filename) :
	Object(texture_) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());
	if (!err.empty())
		std::cerr << err << std::endl;
	if (!ret)
		return;
	this->shape = shapes[0];
	if (materials.size()) this->material = materials[0];
	this->attrib = attrib;
}

Mesh::Mesh(Texture * texture_, const tinyobj::shape_t & shape_, const tinyobj::material_t &material_, const tinyobj::attrib_t & attrib_) :
	Object(texture_), shape(shape_), material(material_), attrib(attrib_) {}

Mesh::Mesh(const tinyobj::shape_t & shape_, const tinyobj::material_t &material_, const tinyobj::attrib_t & attrib_) :
	Object(nullptr), shape(shape_), material(material_), attrib(attrib_) {
	glm::dvec3 specular(material.specular[0], material.specular[1], material.specular[2]);
	glm::dvec3 diffuse(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
	glm::dvec3 transmittance(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
	texture = new PureTexture(
		Material(DEFAULT_AMBIENT, glm::length(specular), material.shininess, 0.0, glm::length(transmittance)),
		diffuse
	);
}

std::shared_ptr<Intersect> Mesh::getTrace(const Ray & ray, double dist) const {
	std::shared_ptr<MeshIntersect> res = std::make_shared<MeshIntersect>(*this, ray);
	if (res->isIntersect() && res->getDistToInter() < dist) return res;
	return nullptr;
}

AABB Mesh::getAABB() const {
	return tree->getAABB();
}

bool Mesh::hasInside() const {
	return true;
}

void Mesh::scaleToBoundingSphere(glm::dvec3 center, double radius) {
	glm::dvec3 meshCenter = zero_vec3;
	for (auto i = 0; i < attrib.vertices.size(); i += 3) {
		glm::dvec3 vert(attrib.vertices[i + 0], attrib.vertices[i + 1], attrib.vertices[i + 2]);
		meshCenter += vert;
	}
	meshCenter /= (attrib.vertices.size() / 3);
	for (auto i = 0; i < attrib.vertices.size(); i += 3) {
		attrib.vertices[i + 0] += (center.x - meshCenter.x);
		attrib.vertices[i + 1] += (center.y - meshCenter.y);
		attrib.vertices[i + 2] += (center.z - meshCenter.z);
	}

	double maxDist = *std::max_element(attrib.vertices.begin(), attrib.vertices.end());
	for (auto i = 0; i < attrib.vertices.size(); i += 3) {
		attrib.vertices[i + 0] = (attrib.vertices[i + 0] - center.x) * (radius / maxDist) + center.x;
		attrib.vertices[i + 1] = (attrib.vertices[i + 1] - center.y) * (radius / maxDist) + center.y;
		attrib.vertices[i + 2] = (attrib.vertices[i + 2] - center.z) * (radius / maxDist) + center.z;
	}
}

void Mesh::update() {
	int index_offset = 0;
	for (auto f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
		int fv = shape.mesh.num_face_vertices[f];

		glm::dvec3 verts[3];
		glm::dvec3 normals[3];
		for (int v = 0; v < fv; ++v) {
			tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
			tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
			tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
			tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

			tinyobj::real_t nx = 0.0, ny = 0.0, nz = 0.0;
			if (3 * idx.normal_index + 2 <= attrib.normals.size()) {
				nx = attrib.normals[3 * idx.normal_index + 0];
				ny = attrib.normals[3 * idx.normal_index + 1];
				nz = attrib.normals[3 * idx.normal_index + 2];
			}

			tinyobj::real_t tx = 0.0, ty = 0.0;
			if (3 * idx.texcoord_index + 1 <= attrib.texcoords.size()) {
				tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				ty = attrib.texcoords[2 * idx.texcoord_index + 1];
			}

			verts[v] = glm::dvec3(vx, vy, vz);
		}

		for (int v = 0; v < 3; ++v) {
			if (normals[v].x == 0.0 && normals[v].y == 0.0 && normals[v].z == 0.0) {
				normals[v] = glm::normalize(glm::cross(
					glm::dvec3(verts[0] - verts[1]),
					glm::dvec3(verts[0] - verts[2])
				));
			}
		}

		Face * face = new Face(texture, verts[0], verts[1], verts[2], normals[0], normals[1], normals[2]);
		faces.push_back(face);
		index_offset += fv;
	}
	tree = new KDTree(faces);
}

std::shared_ptr<Surface> MeshIntersect::getInterPointSurfaceProp() const {
	return nullptr;
}

const Object * MeshIntersect::getObj() const {
	return &mesh;
}

MeshIntersect::MeshIntersect(const Mesh & mesh_, const Ray & ray_) : Intersect(ray_), mesh(mesh_) {
	intersect = std::make_shared<KDTreeIntersect>(*mesh.tree, ray);
}

double MeshIntersect::getDistToInter() const {
	distToInter = intersect->getDistToInter();
	return distToInter;
}

bool MeshIntersect::isIntersect() const {
	return intersect->isIntersect();
}

glm::dvec3 MeshIntersect::getNormal() const {
	return intersect->getNormal();
}