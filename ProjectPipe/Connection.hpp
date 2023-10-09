#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "Node.hpp"

class Connection : public sf::Drawable, public sf::Transformable {
public:
    Node* Node1;
    Node* Node2;
    float TEXTURE_SIZE = 5.f;

    Connection(Node* Node1in, Node* Node2in, int Id);
   // virtual void drawPopup();


private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    int ConnectionId;

};