#include "GasMolecule.h"
#include "./Physics/PhysicsConfiguration.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Physics::Shapes;

GasMolecule::GasMolecule(glm::vec2 position):
    m_color(1.0f, 0.0f, 0.0f),
    m_position(position)
{
}

GasMolecule::GasMolecule() :
    m_color(1.0f, 0.0f, 0.0f),
    m_position(0.0f, 0.0f)
{
}

bool GasMolecule::Intersects(IShape* shape)
{
    static const float sumRadii = PhysicsConfiguration::GasMoleculeDiameter;

    glm::vec2 shapePos = shape->GetPosition();

    // Calculate the distance between the centers of the circles
    float distance = glm::distance(m_position, shapePos);

    // Check if the circles intersect
    if (distance <= sumRadii) 
    {
        return true;  // Circles intersect
    }
    else 
    {
        return false; // Circles do not intersect
    }
}

std::vector<VulkanInit::Vertex> GasMolecule::GetVertices()
{
    glm::vec2 center(m_position.x, m_position.y);
    static const float radius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;

    // Define the corner positions relative to the center
    glm::vec2 topLeft(center.x - radius, center.y - radius);
    glm::vec2 topRight(center.x + radius, center.y - radius);
    glm::vec2 bottomRight(center.x + radius, center.y + radius);
    glm::vec2 bottomLeft(center.x - radius, center.y + radius);

    std::vector<VulkanInit::Vertex> vertices;
    VulkanInit::Vertex vertex;
    vertex.pos = topLeft;
    vertex.color = m_color;
    vertices.push_back(vertex);

    VulkanInit::Vertex vertex2;
    vertex2.pos = topRight;
    vertex2.color = m_color;
    vertices.push_back(vertex2);

    VulkanInit::Vertex vertex3;
    vertex3.pos = bottomRight;
    vertex3.color = m_color;
    vertices.push_back(vertex3);

    VulkanInit::Vertex vertex4;
    vertex4.pos = bottomLeft;
    vertex4.color = m_color;
    vertices.push_back(vertex4);

    VulkanInit::Vertex vertex5;
    vertex5.pos = topLeft;
    vertex5.color = m_color;
    vertices.push_back(vertex5);

    VulkanInit::Vertex vertex6;
    vertex6.pos = bottomRight;
    vertex6.color = m_color;
    vertices.push_back(vertex6);

    return vertices;
}

glm::vec2 GasMolecule::GetPosition()
{
    return m_position;
}

void GasMolecule::SetPosition(glm::vec2 newPos)
{
    m_position.x = newPos.x;
    m_position.y = newPos.y;
}

void GasMolecule::AddPosition(glm::vec2 positionIncrement)
{
    m_position += positionIncrement;
}

glm::vec3 GasMolecule::GetColor()
{
    return m_color;
}

void GasMolecule::SetColor(glm::vec3 newColor)
{
    m_color.r = newColor.r;
    m_color.g = newColor.g;
    m_color.b = newColor.b;
}
