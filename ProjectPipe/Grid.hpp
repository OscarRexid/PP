#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"

class Grid : public sf::Drawable, public sf::Transformable {
public:

    bool load(float gridsize, unsigned int rows, unsigned int columns);

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};