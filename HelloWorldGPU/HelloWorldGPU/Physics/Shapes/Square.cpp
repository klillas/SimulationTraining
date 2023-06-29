#include "Square.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Physics::Shapes;

Square::Square(float size, glm::vec2 position, float rotation):
    m_color(1.0f, 0.0f, 0.0f),
    m_position(position),
    m_size(size),
    m_rotation(rotation)
{
}

Square::Square() :
    m_color(1.0f, 0.0f, 0.0f),
    m_position(0.0f, 0.0f),
    m_size(0.1f),
    m_rotation(0.0f)
{
}

std::vector<VulkanInit::Vertex> Square::GetVertices()
{
    glm::vec2 center(m_position.x, m_position.y);
    float halfSize = m_size / 2.0f;

    // Define the corner positions relative to the center
    glm::vec2 topLeft(center.x - halfSize, center.y - halfSize);
    glm::vec2 topRight(center.x + halfSize, center.y - halfSize);
    glm::vec2 bottomRight(center.x + halfSize, center.y + halfSize);
    glm::vec2 bottomLeft(center.x - halfSize, center.y + halfSize);

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

glm::vec2 Square::GetPosition()
{
    return m_position;
}

void Square::SetPosition(glm::vec2 newPos)
{
    m_position.x = newPos.x;
    m_position.y = newPos.y;
}

float Square::GetRotation()
{
    return m_rotation;
}

void Square::SetRotation(float newRotation)
{
    m_rotation = newRotation;
}

glm::vec3 Square::GetColor()
{
    return m_color;
}

void Square::SetColor(glm::vec3 newColor)
{
    m_color.r = newColor.r;
    m_color.g = newColor.g;
    m_color.b = newColor.b;
}
