#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "Connection.hpp"


class Booster {
public:
	Connection* outletPipe;
	sf::Vector2u locationGrid;
	sf::Vector2f location;
	sf::VertexArray m_vertices;
	sf::Texture m_tileset;

	double headGained = 0;
	bool hasPipe = false;
	int TEXTURE_SIZE = 10;
	Booster(sf::Vector2u locG, sf::Vector2f loc);


	void updateoutletPipe(Connection* newPipe, int node);
};