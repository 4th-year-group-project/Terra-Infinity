#ifndef IRenderable_HPP
#define IRenderable_HPP

/*
    This is a pure virtual class that will be used to define the interface that all renderable
    objects will need to implement. This will allow for the renderer to render the class provided
    it has the data set up correctly.
*/

class IRenderable{
public:
    virtual void render(glm::mat4 view, glm::mat4 projection) = 0;
    virtual void setupData() = 0;
    virtual void updateData() = 0;
};

#endif