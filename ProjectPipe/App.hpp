#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "Node.hpp"
#include "Grid.hpp"
#include "Connection.hpp"

class App {

public:
    App();
    void Run();
    bool clickedOn(int element, sf::Vector2f mousePosView);
    std::vector<std::unique_ptr<Node>>  Nodes;
    std::vector<std::unique_ptr<Connection>>  Pipes;
    double allowedDev = 0.01;
private:
    float gridSizeF = 20.f;
    unsigned gridSizeU = static_cast<unsigned>(gridSizeF);
    void deleteObject(int element, bool isPipe);
    sf::Time dt;
    sf::Vector2i mousePosScreen;
    sf::Vector2i mousePosWindow;
    sf::Vector2f mousePosView;
    sf::Vector2u mousePosGrid;
    sf::Vector2u selectPosGrid;
    sf::Vector2f selectPos;
    const int mapSize = 50;
    //int fromX,toX,fromY,toY = 0;
    Grid grid;
    
    float viewSpeed = 400.f;


};