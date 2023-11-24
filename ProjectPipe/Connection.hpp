#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "Node.hpp"

class Connection : public sf::Drawable, public sf::Transformable {
public:
    Node* Node1;
    Node* Node2;
    float roughness = 0.005;
    float diameter = 0.1;
    float length = 50.f;
    float TEXTURE_SIZE = 3.f;

    Connection(Node* Node1in, Node* Node2in, int Id);
   // virtual void drawPopup();


private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    int ConnectionId;

};