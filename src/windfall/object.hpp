#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "glm/glm.hpp"

class Object {
friend class QTree;
protected:
	glm::vec3 position;
	glm::vec3 boundingBoxMin, boundingBoxMax;

	Object() {}
	~Object() {}
public:

	virtual void draw()=0;
};

#endif//OBJECT_HPP
