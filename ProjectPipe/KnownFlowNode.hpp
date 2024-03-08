#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"

class KnownFlowNode{
public:
	sf::Vector2u locationGrid;
	sf::Vector2f location;
	double flow = 0;
	int TEXTURE_SIZE = 10;
	KnownFlowNode(sf::Vector2u locG, sf::Vector2f loc);
	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};
