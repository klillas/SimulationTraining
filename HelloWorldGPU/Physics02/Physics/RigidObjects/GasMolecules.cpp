#include "GasMolecules.h"
#include "Physics/PhysicsConfiguration.h"

using namespace Physics::RigidObjects;

void GasMolecules::PhysicsTick(float timeDelta, GasMolecule* gasMolecule)
{
    gasMolecule->position += (gasMolecule->velocity * timeDelta);
}

bool GasMolecules::Intersects(GasMolecule* moleculeOne, GasMolecule* moleculeTwo)
{
    static const float sumRadii = PhysicsConfiguration::GasMoleculeDiameter;
    static const float sumRadiiSquare = sumRadii * sumRadii;
    static float dx = 0.0f;
    static float dy = 0.0f;
    static float squaredDistance = 0.0f;

    // Calculate the distance between the centers of the circles
    // distance = sqrt((x2 - x1)^2 + (y2 - y1)^2)
    // sqrt((x2 - x1)^2 + (y2 - y1)^2) <= sumRadii
    // 
    // Optimization: ^2 to get rid of sqrt
    // (x2 - x1)^2 + (y2 - y1)^2 <= sumRadii^2
    // (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) <= sumRadii^2
    // Check if the circles intersect

    dx = moleculeOne->position.x - moleculeTwo->position.x;
    dy = moleculeOne->position.y - moleculeTwo->position.y;

    // Both dx and dy are within sumRadii. Check if we actually intersect
    squaredDistance = (dx * dx) + (dy * dy);

    return squaredDistance < sumRadiiSquare;
}

void GasMolecules::GetVertices(GasMolecule* gasMolecule, std::vector<VulkanInit::Vertex>* vertexVector)
{
    glm::vec2 center(gasMolecule->position.x, gasMolecule->position.y);
    static const float radius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;

    // Define the corner positions relative to the center
    glm::vec2 topLeft(center.x - radius, center.y - radius);
    glm::vec2 topRight(center.x + radius, center.y - radius);
    glm::vec2 bottomRight(center.x + radius, center.y + radius);
    glm::vec2 bottomLeft(center.x - radius, center.y + radius);

    VulkanInit::Vertex vertex;
    vertex.pos = topLeft;
    vertex.color = gasMolecule->color;
    vertexVector->push_back(vertex);

    VulkanInit::Vertex vertex2;
    vertex2.pos = topRight;
    vertex2.color = gasMolecule->color;
    vertexVector->push_back(vertex2);

    VulkanInit::Vertex vertex3;
    vertex3.pos = bottomRight;
    vertex3.color = gasMolecule->color;
    vertexVector->push_back(vertex3);

    VulkanInit::Vertex vertex4;
    vertex4.pos = bottomLeft;
    vertex4.color = gasMolecule->color;
    vertexVector->push_back(vertex4);

    VulkanInit::Vertex vertex5;
    vertex5.pos = topLeft;
    vertex5.color = gasMolecule->color;
    vertexVector->push_back(vertex5);

    VulkanInit::Vertex vertex6;
    vertex6.pos = bottomRight;
    vertex6.color = gasMolecule->color;
    vertexVector->push_back(vertex6);
}