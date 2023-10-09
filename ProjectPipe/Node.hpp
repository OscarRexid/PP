#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"

class Node : public sf::Drawable, public sf::Transformable {
public:
    enum type {
        bend,
        cross,
        booster
    };
    sf::Vector2u locationGrid;
    sf::Vector2f location;
    type nodeType;
    int TEXTURE_SIZE = 5;

    Node(sf::Vector2u locG, sf::Vector2f loc, int typeint, int Id);
    virtual void drawPopup();


private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    
    int NodeId;

};