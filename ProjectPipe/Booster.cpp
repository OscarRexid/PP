#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <string>
#include <sstream>
#include "Booster.hpp"

Booster::Booster(sf::Vector2u locG, sf::Vector2f loc) :locationGrid(locG), location(loc) {
    m_vertices.setPrimitiveType(sf::Quads);
    sf::Vertex topleft = sf::Vertex(sf::Vector2f(location.x - TEXTURE_SIZE, location.y + TEXTURE_SIZE), sf::Color::White);
    sf::Vertex bottomright = sf::Vertex(sf::Vector2f(location.x + TEXTURE_SIZE, location.y - TEXTURE_SIZE), sf::Color::White);
    sf::Vertex topright = sf::Vertex(sf::Vector2f(location.x + TEXTURE_SIZE, location.y + TEXTURE_SIZE), sf::Color::White);
    sf::Vertex bottomleft = sf::Vertex(sf::Vector2f(location.x - TEXTURE_SIZE, location.y - TEXTURE_SIZE), sf::Color::White);

    if (m_tileset.loadFromFile("flow.png")) {
        sf::Vector2u size = m_tileset.getSize();
        topleft.texCoords = sf::Vector2f(0, 0);
        bottomleft.texCoords = sf::Vector2f(0, size.y - 1);
        bottomright.texCoords = sf::Vector2f(size.x - 1, size.y - 1);
        topright.texCoords = sf::Vector2f(size.x - 1, 0);
    }
    else {
        topleft.color = sf::Color::Blue;
        topright.color = sf::Color::Blue;
        bottomright.color = sf::Color::Blue;
        bottomleft.color = sf::Color::Blue;
    }
    m_vertices.append(topleft);
    m_vertices.append(topright);
    m_vertices.append(bottomright);
    m_vertices.append(bottomleft);
}

void Booster::updateoutletPipe(Connection* newPipe,int node)
{
    if (hasPipe) {
        outletPipe->afterBooster = false;
        outletPipe->boosterNode = 0;
    }
    outletPipe = newPipe;
    outletPipe->afterBooster = true;
    outletPipe->boosterNode = node;
}
