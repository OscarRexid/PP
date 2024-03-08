#pragma once

#include "Node.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <string>
#include <sstream>
#include<iostream>



    Node::Node(sf::Vector2u locG, sf::Vector2f loc, int typeint, int type2int, int Id) :locationGrid(locG), location(loc), flowTypeVar(flowType(typeint)), NodeId(Id) {

        //Set up the graphical icon
        std::cout << flowTypeVar;
        if (flowTypeVar == flowType::input) {
            std::unique_ptr<KnownFlowNode> newFlowNode = std::make_unique<KnownFlowNode>(locationGrid, location);
            flowNode = std::move(newFlowNode);
            m_vertices = flowNode->m_vertices;
            m_tileset = flowNode->m_tileset;
            
        }
        else if (flowTypeVar == flowType::connection) {
            connectionTypeVar = connectionType(type2int);
            m_vertices.setPrimitiveType(sf::Quads);
            sf::Vertex topleft = sf::Vertex(sf::Vector2f(location.x - TEXTURE_SIZE, location.y + TEXTURE_SIZE), sf::Color::White);
            sf::Vertex bottomright = sf::Vertex(sf::Vector2f(location.x + TEXTURE_SIZE, location.y - TEXTURE_SIZE), sf::Color::White);
            sf::Vertex topright = sf::Vertex(sf::Vector2f(location.x + TEXTURE_SIZE, location.y + TEXTURE_SIZE), sf::Color::White);
            sf::Vertex bottomleft = sf::Vertex(sf::Vector2f(location.x - TEXTURE_SIZE, location.y - TEXTURE_SIZE), sf::Color::White);
            
            switch (connectionTypeVar) {
            case connectionType::bend:
                topleft.color = sf::Color::Blue;
                topright.color = sf::Color::Blue;
                bottomright.color = sf::Color::Blue;
                bottomleft.color = sf::Color::Blue;
                break;
            case connectionType::cross:

                topleft.color = sf::Color::Red;
                topright.color = sf::Color::Red;
                bottomright.color = sf::Color::Red;
                bottomleft.color = sf::Color::Red;
                break;
            case connectionType::booster:

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
       

       

    }

    void Node::drawPopup() {
        std::stringstream nodeText;
        nodeText << "Node: " << NodeId;
        ImGui::Begin("Window", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
        ImGui::Text(nodeText.str().c_str());
        //ImGui::SetWindowPos(ImVec2(200, 200));
        //ImGui::SetWindowSize(ImVec2(100, 200));
        if (flowTypeVar == flowType::input) {
            if (inputTypeVar == inputType::knownFlow) {
                ImGui::InputDouble("Flow",&flowNode->flow);
            }
        }
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
   
   
