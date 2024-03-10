#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "Node.hpp"

class Connection : public sf::Drawable, public sf::Transformable {
public:
    Node* Node1;
    Node* Node2;
    double roughness = 0.005;
    float rotationDegrees;
    double diameter = 0.15;
    double length = 10.f;
    float TEXTURE_SIZE = 3.f;
    sf::VertexArray m_vertices;

    Connection(Node* Node1in, Node* Node2in, int Id);
    virtual void drawPopup();

    virtual void storeResults(double inFlow, double inPreassure1, double inPreassure2);

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;


    sf::Texture m_tileset;
    int ConnectionId;
    sf::Text idText;
    sf::Font font;
    //Results storage
    double flow;
    double velocity;
    double preassure_loss;
    

};