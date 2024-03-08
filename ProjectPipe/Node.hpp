#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "KnownFlowNode.hpp"

class Node : public sf::Drawable, public sf::Transformable {
public:
    enum flowType {
        input,
        output,
        connection
    };
    enum connectionType {
        bend,
        cross,
        booster

    };
    enum inputType {
        knownFlow
    };
    enum outputType {
        open
    };
    sf::Vector2u locationGrid;
    sf::Vector2f location;
    flowType flowTypeVar;
    outputType outputTypeVar;
    connectionType connectionTypeVar;
    inputType inputTypeVar;
    std::unique_ptr<KnownFlowNode> flowNode;
    int TEXTURE_SIZE = 5;

    Node(sf::Vector2u locG, sf::Vector2f loc, int typeint,int type2int, int Id);
    virtual void drawPopup();
    int getId();
    


private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    int NodeId;
    

};