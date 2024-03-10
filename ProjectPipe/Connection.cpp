#pragma once

#include "Node.hpp"
#include "Connection.hpp"	
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <string>
#include <math.h>
#include <sstream>
#include <iostream>

Connection::Connection(Node* Node1in, Node* Node2in, int Id) :Node1(Node1in), Node2(Node2in), ConnectionId(Id){
    m_vertices.setPrimitiveType(sf::TrianglesStrip);
    Node1in->connectionsAmount += 1;
    Node2in->connectionsAmount += 1;
    float xfactor = 0.f;
    float yfactor = 0.f;
   
    if (Node2->location.x - Node1->location.x == 0) {
        xfactor = TEXTURE_SIZE;
    }
    else if (Node2->location.y - Node1->location.y == 0) {
        yfactor = TEXTURE_SIZE;
    }
    else {
        xfactor = (abs(Node2->location.x - Node1->location.x) / (Node2->location.x - Node1->location.x)) * TEXTURE_SIZE;
        yfactor = (abs(Node2->location.y - Node1->location.y) / (Node2->location.y - Node1->location.y)) * TEXTURE_SIZE;
    }
    std::cout << xfactor;
    std::cout << yfactor;

    sf::Vertex node1left = sf::Vertex(sf::Vector2f(Node1->location.x - xfactor, Node1->location.y + yfactor), sf::Color::White);
    sf::Vertex node1right = sf::Vertex(sf::Vector2f(Node1->location.x + xfactor, Node1->location.y - yfactor), sf::Color::White);
    sf::Vertex node2left = sf::Vertex(sf::Vector2f(Node2->location.x - xfactor , Node2->location.y + yfactor), sf::Color::White);
    sf::Vertex node2right = sf::Vertex(sf::Vector2f(Node2->location.x + xfactor, Node2->location.y - yfactor), sf::Color::White);
    float xleft = node2left.position.x + 0.5f * (node1left.position.x - node2left.position.x);
    float yleft = node2left.position.y + 0.5f * (node1left.position.y - node2left.position.y);
    float xright = node2right.position.x + 0.5f * (node1right.position.x - node2right.position.x);
    float yright = node2right.position.y + 0.5f * (node1right.position.y - node2right.position.y);
    sf::Vertex leftextra = sf::Vertex(sf::Vector2f(xleft, yleft), sf::Color::White);
    sf::Vertex rightextra = sf::Vertex(sf::Vector2f(xright,yright), sf::Color::White);
    rotationDegrees = atanf(abs(xleft - xright) / abs(yleft - yright));
    node1left.color = sf::Color::White;
    node1right.color = sf::Color::White;
    node2left.color = sf::Color::White;
    node2right.color = sf::Color::White;

    m_vertices.append(node1left);
    m_vertices.append(node1right);
    m_vertices.append(node2left);
    m_vertices.append(node2right);
    m_vertices.append(leftextra);
    m_vertices.append(rightextra);
    

    //Font and text stuff
    font.loadFromFile("Roboto-Medium.ttf");
    idText.setFont(font);
    idText.setCharacterSize(20);
    idText.setString(std::to_string(ConnectionId));
    sf::Vector2f midpoint = Node1->location + 0.5f * (Node2->location - Node1->location);
    midpoint.y += 10; // making sure the text wont be inside the pipe(moves it down)
    midpoint.x -= 10; // same as above but for vertical pipes(moves it left)
    idText.setPosition(midpoint);
    
}

void Connection::drawPopup()
{
    std::stringstream pipeText;
    pipeText << "Pipe: " << ConnectionId;
    ImGui::Begin("Window", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    ImGui::Text(pipeText.str().c_str());
    ImGui::InputDouble("Diameter", &diameter);
    ImGui::InputDouble("Length", &length);
    ImGui::InputDouble("Roughness", &roughness);
    std::stringstream flowText;
    std::stringstream velText;
    std::stringstream preassureText;
    ImGui::Text("Results:");
    flowText << "Flow: " << flow;
    velText << "Velocity: " << velocity;
    preassureText << "Preassureloss: " << preassure_loss;
    ImGui::Text(flowText.str().c_str());
    ImGui::Text(velText.str().c_str());
    ImGui::Text(preassureText.str().c_str());
   
    ImGui::End();
}



void Connection::storeResults(double inFlow, double inPreassure1, double inPreassure2)
{
    //Storing results, later will be passed through unit converter
    if (inFlow < 0) {
        //Flow from node 2 to node 1
    }
    else if (inFlow > 0) {
        //Flow from node1 to node 2
    }
    flow = abs(inFlow);
    preassure_loss = abs(inPreassure1 - inPreassure2) / (9.81 * 1000);
    velocity = flow / (pow(diameter, 2) / 4 * 3.1415);
}


void Connection::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();
    target.draw(idText, states);
    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
    

}