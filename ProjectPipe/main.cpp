#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"
#include "SFML/System.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include<sstream>
#include<iostream>
#include <fstream>
#include <vector>
#include <map>


class Grid : public sf::Drawable, public sf::Transformable
{
public:

    bool load(float gridsize,unsigned int rows, unsigned int columns)
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

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};















int main()
{
    //Init grid
    float gridSizeF = 20.f;
    unsigned gridSizeU = static_cast<unsigned>(gridSizeF);
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
    grid.load(gridSizeF,1000,1000);
    

    // create the window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    ImGui::SFML::Init(window);
    //window.setVerticalSyncEnabled(true);
    // 
    //init view
    sf::View view;
    view.setViewport(sf::FloatRect(0.2f, 0.f, 1.f, 1.f));
    view.setSize(960.f, 540.f);
    view.setCenter(window.getSize().x * 0.6f, window.getSize().y / 2.f);


    sf::View SideUiView;
    SideUiView.setViewport(sf::FloatRect(0.f, 0.f, 0.2f, 1.f));
    SideUiView.setSize(960.f, 540.f);
    SideUiView.setCenter(window.getSize().x * 0.1f, window.getSize().y / 2.f);
    
    float viewSpeed = 400.f;


    // activate the window
    window.setActive(true);

    //Load resources, initialize the OpenGL states, ...
    sf::RectangleShape shape(sf::Vector2f(gridSizeF/2,gridSizeF/2));
    

    //Run the main loop
    sf::Clock deltaClock;
    bool running = true;
    while (running)
    {
        //Handle events
        dt = deltaClock.restart();
        window.setView(view);
        //Update mouse positions
        mousePosScreen = sf::Mouse::getPosition();
        mousePosWindow = sf::Mouse::getPosition(window);
        mousePosView = window.mapPixelToCoords(mousePosWindow);
        if (mousePosView.x >= 0.f) {
            mousePosGrid.x = mousePosView.x / gridSizeU;
            selectPosGrid.x = round(mousePosView.x / gridSizeU);
            selectPos.x = selectPosGrid.x * gridSizeF;
        }
        if (mousePosView.y >= 0.f) {
            mousePosGrid.y = mousePosView.y / gridSizeU;
            selectPosGrid.y = round(mousePosView.y / gridSizeU);
            selectPos.y = selectPosGrid.y * gridSizeF;
            
        }
        shape.setPosition(sf::Vector2f(selectPos.x - gridSizeF/4,selectPos.y - gridSizeF / 4));
        std::stringstream ss;
        ss << "Screen" << mousePosScreen.x << " " << mousePosScreen.y << "\n"
            << "Window" << mousePosWindow.x << " " << mousePosWindow.y << "\n"
            << "View" << mousePosView.x << " " << mousePosView.y << "\n"
            << "Grid" << mousePosGrid.x << " " << mousePosGrid.y << "\n"
            << "SelectGrid" << selectPosGrid.x << " " << selectPosGrid.y << "\n"
            << "Select" << selectPos.x << " " << selectPos.y << "\n";


        sf::Event event;
        while (window.pollEvent(event))
        {

            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::MouseWheelMoved) { // zoom in
                if (event.mouseWheel.delta > 0 && view.getSize().x >100) {
                    view.zoom(10 / 11.f);
                    window.setView(view);
                    if ((view.getCenter().x - view.getSize().x) < 0 || (view.getCenter().y - view.getSize().y) < 0) {
                        view.setCenter(sf::Vector2f(view.getSize().x, view.getSize().y));
                    }
                }
                else if(view.getSize().x < 4000) { // zoom out
                    view.zoom(11 / 10.f);
                    window.setView(view);
                    if ((view.getCenter().x - view.getSize().x) < 0 || (view.getCenter().y - view.getSize().y) < 0) {
                        view.setCenter(sf::Vector2f(view.getSize().x, view.getSize().y));
                    }

                }
                
            }
        }
       

        //Update
        //Update Input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { // left
            if ((view.getCenter().x - (view.getSize().x / 2.f) - (viewSpeed * dt.asSeconds())) > 0) // cant move into negatives
               view.move(-viewSpeed*dt.asSeconds(), 0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { // right
            view.move(viewSpeed * dt.asSeconds(), 0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { // down
            if ((view.getCenter().y + (view.getSize().y / 2.f) + (viewSpeed * dt.asSeconds())) > 0)
                view.move(0.f ,viewSpeed * dt.asSeconds());
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { // up
            if ((view.getCenter().y - (view.getSize().y / 2.f) - (viewSpeed * dt.asSeconds())) > 0) // cant move into negatives
            view.move(0.f,-viewSpeed * dt.asSeconds());
        }
       
        ImGui::SFML::Update(window, deltaClock.restart());
        //draw ui
        ImGui::Begin("Window Title", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(window.getSize().x*0.2f, window.getSize().y));
        ImGui::Text(ss.str().c_str());
        ImGui::End();


        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window.clear();
        
        // draw...
        window.draw(shape);
        window.draw(grid);
        
        


        // end the current frame (internally swaps the front and back buffers)
        window.setView(window.getDefaultView());




        ImGui::SFML::Render(window);
        window.display();
    }





    // release resources...
    ImGui::SFML::Shutdown();
    return 0;
  
}