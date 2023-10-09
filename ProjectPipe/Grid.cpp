#pragma once
#include "Grid.hpp"




    bool Grid::load(float gridsize, unsigned int rows, unsigned int columns)
    {

        m_vertices.setPrimitiveType(sf::Lines);

        for (int x = 0; x <= columns; x++) {
            sf::Vertex top(gridsize * sf::Vector2f(x, 0), sf::Color::White);
            sf::Vertex bottom(gridsize * sf::Vector2f(x, rows),
                sf::Color::White);
            m_vertices.append(top);
            m_vertices.append(bottom);
        }
        for (int y = 0; y <= rows; y++) {
            sf::Vertex left(gridsize * sf::Vector2f(0, y), sf::Color::White);
            sf::Vertex right(gridsize * sf::Vector2f(columns, y),
                sf::Color::White);
            m_vertices.append(left);
            m_vertices.append(right);
        }


        return true;
    }



    void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }
