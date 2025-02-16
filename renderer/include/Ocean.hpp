#ifndef OCEAN_HPP
#define OCEAN_HPP
/*
    This class will hold all of the information for a subchunk of ocean that will be rendered at
    the sea level of the world. This will be used to render the ocean in the world.
*/

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include <Object.hpp>
#include <IRenderable.hpp>
#include <Vertex.hpp>
#include <SubChunk.hpp>

using namespace std;

class Ocean : public Object, public IRenderable{

private:
    int size; // The size of the ocean chunk
    float seaLevel; // The sea level of the ocean as a fraction of the maximum height
    vector<float> oceanQuadOrigin; // The origin of the ocean quad in world space
    vector<Vertex> vertices; // The vertices of the ocean quad
    vector<unsigned int> indices; // The indices of the ocean quad

public:
    Ocean(
        int inSize,
        float inSeaLevel,
        vector<float> inOceanQuadOrigin
    ): Object(), size(inSize), seaLevel(inSeaLevel), oceanQuadOrigin(inOceanQuadOrigin){
        // Initialise the parent object
        vertices = vector<Vertex>();
        indices = vector<unsigned int>();
    }

    ~Ocean(){};

    int getSize(){return size;}
    float getSeaLevel(){return seaLevel;}
    vector<float> getOceanQuadOrigin(){return oceanQuadOrigin;}
    vector<Vertex> getVertices(){return vertices;}
    vector<unsigned int> getIndices(){return indices;}

    void setSize(int inSize){size = inSize;}
    void setSeaLevel(float inSeaLevel){seaLevel = inSeaLevel;}
    void setOceanQuadOrigin(vector<float> inOceanQuadOrigin){oceanQuadOrigin = inOceanQuadOrigin;}
    void setVertices(vector<Vertex> inVertices){vertices = inVertices;}
    void setIndices(vector<unsigned int> inIndices){indices = inIndices;}
    void addVertex(Vertex inVertex){vertices.push_back(inVertex);}
    void addIndex(unsigned int inIndex){indices.push_back(inIndex);}

    void render(glm::mat4 view, glm::mat4 projection) override;
    void setupData() override;
    void updateData() override;

};


#endif