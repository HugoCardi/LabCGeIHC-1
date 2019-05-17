#ifndef PANTALLA_H
#define PANTALLA_H
#include "Abstractmodel.h"

class Pantalla : public AbstractModel
{
public:
	Pantalla();
	~Pantalla();
	void setUVS(std::vector<glm::vec2> uvs);
	virtual bool rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection);
	AABB aabb;
};

#endif // Pantalla_H
