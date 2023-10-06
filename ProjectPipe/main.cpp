#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "SFML/OpenGL.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    ImGui::SFML::Init(window);
    //window.setVerticalSyncEnabled(true);

    // activate the window
    window.setActive(true);

    // load resources, initialize the OpenGL states, ...

    // run the main loop
    sf::Clock deltaClock;
    bool running = true;
    while (running)
    {
        // handle events
        
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
        }
        ImGui::SFML::Update(window, deltaClock.restart());
      
        ImGui::Begin("Window Title", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowPos(ImVec2(0,0));
        ImGui::Text("Window Text!");
        ImGui::End();

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw...
            
        // end the current frame (internally swaps the front and back buffers)
        ImGui::SFML::Render(window);
        window.display();
    }

    // release resources...
    ImGui::SFML::Shutdown();
    return 0;
  
}