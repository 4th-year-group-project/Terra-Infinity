#ifndef IRenderable_HPP
#define IRenderable_HPP

/*
    This is a pure virtual class that will be used to define the interface that all renderable
    objects will need to implement. This will allow for the renderer to render the class provided
    it has the data set up correctly.
*/

#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Light.hpp"
#include "Settings.hpp"

using namespace std;

class IRenderable {
public:
    virtual void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos
    ) = 0;
    virtual void setupData() = 0;
    virtual void updateData(bool regenerate) = 0;
};

#endif // IRenderable_HPP