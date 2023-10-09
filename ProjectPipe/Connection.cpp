#pragma once

#include "Node.hpp"
#include "Connection.hpp"	
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <string>
#include <sstream>

Connection::Connection(Node* Node1in, Node* Node2in, int Id) :Node1(Node1in), Node2(Node2in), ConnectionId(Id){
    m_vertices.setPrimitiveType(sf::Quads);
    sf::Vertex topleft = sf::Vertex(sf::Vector2f(Node2->location.x, Node1->location.y), sf::Color::White);
    sf::Vertex bottomright = sf::Vertex(sf::Vector2f(Node1->location.x, Node2->location.y), sf::Color::White);
    sf::Vertex topright = sf::Vertex(sf::Vector2f(Node1->location.x, Node1->location.y), sf::Color::White);
    sf::Vertex bottomleft = sf::Vertex(sf::Vector2f(Node2->location.x, Node2->location.y ), sf::Color::White);

    topleft.color = sf::Color::White;
    topright.color = sf::Color::White;
    bottomright.color = sf::Color::White;
    bottomleft.color = sf::Color::White;

    m_vertices.append(topleft);
    m_vertices.append(topright);
    m_vertices.append(bottomright);
    m_vertices.append(bottomleft);
}


void Connection::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);

}