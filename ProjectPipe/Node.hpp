#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "KnownFlowNode.hpp"
#include "Booster.hpp"
#include "Connection.hpp"

#include <vector>

class Connection; // forward declaration because circular referencing
class Booster;

class Node : public sf::Drawable, public sf::Transformable {
public:
    enum flowType {
        input,
        output,
        connection
    };
    enum connectionType {
        bend,
        junction,
        booster

    };
    enum inputType {
        knownFlow
    };
    enum outputType {
        open
    };
    std::vector<Connection*> connectedPipes;
    sf::Vector2u locationGrid;
    sf::Vector2f location;
    flowType flowTypeVar;
    outputType outputTypeVar;
    connectionType connectionTypeVar;
    inputType inputTypeVar;
    std::unique_ptr<KnownFlowNode> flowNode;
    std::unique_ptr<Booster> boosterNode;
    int TEXTURE_SIZE = 5;
    double height = 0;
    double KValue = 0;

    Node(sf::Vector2u locG, sf::Vector2f loc, int typeint,int type2int, int Id);
    virtual void drawPopup();
    int getId();
    void updateId(int newId);
    int connectionsAmount = 0;
private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    sf::Text idText;
    sf::Font font;
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    int NodeId;
    

};