/**
 * @file IRenderable.hpp
 * @author King Attalus II
 * @brief This file contains the interface for any object that can be rendered in the scene.
 * @version 1.0
 * @date 2025
 *
 */
#ifndef IRenderable_HPP
#define IRenderable_HPP
#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Light.hpp"
#include "Settings.hpp"

/**
 * @brief This class is an interface for any object that can be rendered within the scene.
 *
 */
class IRenderable {
public:
    virtual void render(
        glm::mat4 view,
        glm::mat4 projection,
        std::vector<std::shared_ptr<Light>> lights,
        glm::vec3 viewPos,
        bool isWaterPass,
        bool isShadowPass,
        glm::vec4 plane
    ) = 0;
    virtual void setupData() = 0;
    virtual void updateData(bool regenerate) = 0;
};

#endif // IRenderable_HPP
