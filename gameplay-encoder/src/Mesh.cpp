#include "Mesh.h"
#include <cmath>

namespace gameplay
{

Mesh::Mesh(void)
{
}

Mesh::~Mesh(void)
{
}

unsigned int Mesh::getTypeId(void) const
{
    return MESH_ID;
}

const char* Mesh::getElementName(void) const
{
    return "Mesh";
}

void Mesh::writeBinary(FILE* file)
{
    Object::writeBinary(file);
    // vertex formats
    write(_vertexFormats.size(), file);
    for (std::vector<VertexElement>::iterator i = _vertexFormats.begin(); i != _vertexFormats.end(); i++)
    {
        i->writeBinary(file);
    }
    // vertices
    writeBinaryVertices(file);
    // parts
    writeBinaryObjects(parts, file);
}

void Mesh::writeBinaryVertices(FILE* file)
{
    if (vertices.size() > 0)
    {
        // Assumes that all vertices are the same size.
        // Write the number of bytes for the vertex data
        const Vertex& vertex = vertices.front();
        write(vertices.size() * vertex.byteSize(), file); // (vertex count) * (vertex size)

        // for each vertex
        for (std::vector<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); i++)
        {
            // Write this vertex
            i->writeBinary(file);
        }
    }
    else
    {
        // No vertex data
        write((unsigned int)0, file);
    }

    // Write bounds
    computeBounds();
    write(&bounds.min.x, 3, file);
    write(&bounds.max.x, 3, file);
    write(&bounds.center.x, 3, file);
    write(bounds.radius, file);
}

void Mesh::writeText(FILE* file)
{
    fprintElementStart(file);

    // for each VertexFormat
    if (vertices.size() > 0 )
    {
        for (std::vector<VertexElement>::iterator i = _vertexFormats.begin(); i != _vertexFormats.end(); i++)
        {
            i->writeText(file);
        }
    }

    // for each Vertex
    fprintf(file, "<vertices count=\"%u\">\n", vertices.size());
    for (std::vector<Vertex>::iterator i = vertices.begin(); i != vertices.end(); i++)
    {
        i->writeText(file);
    }
    fprintf(file, "</vertices>\n");

    // write bounds
    computeBounds();
    fprintf(file, "<bounds>\n");
    fprintf(file, "<min>\n");
    bounds.min.writeText(file);
    fprintf(file, "</min>\n");
    fprintf(file, "<max>\n");
    bounds.max.writeText(file);
    fprintf(file, "</max>\n");
    fprintf(file, "<center>\n");
    bounds.center.writeText(file);
    fprintf(file, "</center>\n");
    fprintf(file, "<radius>%f</radius>\n", bounds.radius);
    fprintf(file, "</bounds>\n");

    // for each MeshPart
    for (std::vector<MeshPart*>::iterator i = parts.begin(); i != parts.end(); i++)
    {
        (*i)->writeText(file);
    }

    fprintElementEnd(file);
}

void Mesh::addMeshPart(MeshPart* part)
{
    parts.push_back(part);
}

void Mesh::addMeshPart(Vertex* vertex)
{
    vertices.push_back(*vertex);
}

void Mesh::addVetexAttribute(unsigned int usage, unsigned int count)
{
    _vertexFormats.push_back(VertexElement(usage, count));
}

size_t Mesh::getVertexCount() const
{
    return vertices.size();
}

bool Mesh::contains(const Vertex& vertex) const
{
    return vertexLookupTable.count(vertex) > 0;
}

unsigned int Mesh::addVertex(const Vertex& vertex)
{
    unsigned int index = getVertexCount();
    vertices.push_back(vertex);
    vertexLookupTable[vertex] = index;
    return index;
}

unsigned int Mesh::getVertexIndex(const Vertex& vertex)
{
    std::map<Vertex,unsigned int>::iterator it;
    it = vertexLookupTable.find(vertex);
    // TODO: Remove it from the map because we are going to delete it
    return it->second;
}

void Mesh::computeBounds()
{
    bounds.min.x = bounds.min.y = bounds.min.z = FLT_MAX;
    bounds.max.x = bounds.max.y = bounds.max.z = FLT_MIN;
    bounds.center.x = bounds.center.y = bounds.center.z = 0.0f;
    bounds.radius = 0.0f;
    
    // for each vertex
    Vector3 avgPos;
    for (std::vector<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); i++)
    {
        // Update min/max for this vertex
        if (i->position.x < bounds.min.x)
            bounds.min.x = i->position.x;
        if (i->position.y < bounds.min.y)
            bounds.min.y = i->position.y;
        if (i->position.z < bounds.min.z)
            bounds.min.z = i->position.z;
        if (i->position.x > bounds.max.x)
            bounds.max.x = i->position.x;
        if (i->position.y > bounds.max.y)
            bounds.max.y = i->position.y;
        if (i->position.z > bounds.max.z)
            bounds.max.z = i->position.z;

        avgPos.x += i->position.x;
        avgPos.y += i->position.y;
        avgPos.z += i->position.z;
    }

    // Compute center point
    bounds.center.x = avgPos.x / (float)vertices.size();
    bounds.center.y = avgPos.y / (float)vertices.size();
    bounds.center.z = avgPos.z / (float)vertices.size();

    // Compute radius by looping through all points again and finding the max
    // distance between the center point and each vertex position
    for (std::vector<Vertex>::const_iterator i = vertices.begin(); i != vertices.end(); i++)
    {
        float d = Vector3::distanceSquared(bounds.center, i->position);
        if (d > bounds.radius)
        {
            bounds.radius = d;
        }
    }

    // Convert squared distance to distance for radius
    bounds.radius = std::sqrtf(bounds.radius);
}

}
