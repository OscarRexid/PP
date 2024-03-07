#pragma once

#include "Node.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <string>
#include <sstream>



    Node::Node(sf::Vector2u locG, sf::Vector2f loc, int typeint, int Id) :locationGrid(locG), location(loc), nodeType(type(typeint)), NodeId(Id) {

        //Set up the graphical icon
        m_vertices.setPrimitiveType(sf::Quads);
        sf::Vertex topleft = sf::Vertex(sf::Vector2f(location.x - TEXTURE_SIZE, location.y + TEXTURE_SIZE), sf::Color::White);
        sf::Vertex bottomright = sf::Vertex(sf::Vector2f(location.x + TEXTURE_SIZE, location.y - TEXTURE_SIZE), sf::Color::White);
        sf::Vertex topright = sf::Vertex(sf::Vector2f(location.x + TEXTURE_SIZE, location.y + TEXTURE_SIZE), sf::Color::White);
        sf::Vertex bottomleft = sf::Vertex(sf::Vector2f(location.x - TEXTURE_SIZE, location.y - TEXTURE_SIZE), sf::Color::White);

        switch (nodeType) {
        case type::bend:
            topleft.color = sf::Color::Blue;
            topright.color = sf::Color::Blue;
            bottomright.color = sf::Color::Blue;
            bottomleft.color = sf::Color::Blue;
            break;
        case type::cross:

            topleft.color = sf::Color::Red;
            topright.color = sf::Color::Red;
            bottomright.color = sf::Color::Red;
            bottomleft.color = sf::Color::Red;
            break;
        case type::booster:

            topleft.color = sf::Color::Green;
            topright.color = sf::Color::Green;
            bottomright.color = sf::Color::Green;
            bottomleft.color = sf::Color::Green;
            break;
        default:

            topleft.color = sf::Color::White;
            topright.color = sf::Color::White;
            bottomright.color = sf::Color::White;
            bottomleft.color = sf::Color::White;
        }
        m_vertices.append(topleft);
        m_vertices.append(topright);
        m_vertices.append(bottomright);
        m_vertices.append(bottomleft);

    }

    void Node::drawPopup() {
        std::stringstream nodeText;
        nodeText << "Node: " << NodeId;
        ImGui::Begin("Window", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
        ImGui::Text(nodeText.str().c_str());
        //ImGui::SetWindowPos(ImVec2(200, 200));
        //ImGui::SetWindowSize(ImVec2(100, 200));
        ImGui::End();
    }
    

    int Node::getId() {
        return NodeId;
    }


    void Node::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);

    }
   
   
