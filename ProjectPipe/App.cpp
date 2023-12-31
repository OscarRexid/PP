#pragma once
#include "App.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "Simulation.hpp"

#include<sstream>
#include<iostream>
#include <fstream>
#include <vector>
#include <map>



App::App() {
	grid.load(gridSizeF, 1000, 1000);
}

void App::Run() {
	static int selectedMain = 0;
	static int selectedNodeBuild = 0;
	int selectedNode = -1;
    float AccumZoom = 1.f;
    sf::RectangleShape shape(sf::Vector2f(gridSizeF / 2, gridSizeF / 2));


    // create the window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
    ImGui::SFML::Init(window);
    window.setVerticalSyncEnabled(false);

    //init view
    sf::View view;
    view.setViewport(sf::FloatRect(0.2f, 0.f, 1.f, 1.f));
    view.setSize(960.f, 540.f);
    view.setCenter(window.getSize().x * 0.6f, window.getSize().y / 2.f);

    //sideview for menu
    sf::View SideUiView;
    SideUiView.setViewport(sf::FloatRect(0.f, 0.f, 0.2f, 1.f));
    SideUiView.setSize(960.f, 540.f);
    SideUiView.setCenter(window.getSize().x * 0.1f, window.getSize().y / 2.f);

    // activate the window
    window.setActive(true);

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
        shape.setPosition(sf::Vector2f(selectPos.x - gridSizeF / 4, selectPos.y - gridSizeF / 4)); //Gridsnap display(Displays a white box where the item would be placed)

        std::stringstream ss; // Debugging
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
            auto& io = ImGui::GetIO();
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
            else if (event.type == sf::Event::MouseWheelMoved && !io.WantCaptureMouse) { // zoom in
                if (event.mouseWheel.delta > 0 && view.getSize().x > 100) {

                    
                    //First we make a vector from the MousePosView to the current Center
                    float xvec = view.getCenter().x - mousePosView.x;
                    float yvec = view.getCenter().y - mousePosView.y;

                    //We then make the MousePosView the center(note we dont update the view for the window)
                    view.setCenter(mousePosView);
                    //Then Zoom
                    view.zoom(0.9f);
                    //Then add the vector to the old center scaled by the zoom, that way if our mouse is on grid (x,y) before zoom it should remain the on (x,y) after
                    view.setCenter(sf::Vector2f(view.getCenter().x + xvec* 0.9f,view.getCenter().y+ yvec*0.9f));
                    //Safety checks incase the zoom causes the view to go outside the grid area,not the best solution as it resets the view far away instead of just vector adding the difference but it will do for now.
                    if ((view.getCenter().x - 0.5f*view.getSize().x) < 0 || (view.getCenter().y - 0.5f*view.getSize().y) < 0) {
                        view.setCenter(sf::Vector2f(view.getSize().x, view.getSize().y));
                    }
                    //Set the view for the window.
                    window.setView(view);
                }
                else if (view.getSize().x < 4000) { // zoom out
                    //same as zooming in just the zoom > 1
                    float xvec = view.getCenter().x - mousePosView.x;
                    float yvec = view.getCenter().y - mousePosView.y;

                    view.setCenter(mousePosView);
                    view.zoom(1.1f);
                    view.setCenter(sf::Vector2f(view.getCenter().x + xvec *  1.1f, view.getCenter().y + yvec * 1.1f));
                    window.setView(view);
                    if ((view.getCenter().x - 0.5f*view.getSize().x) < 0 || (view.getCenter().y - 0.5f*view.getSize().y) < 0) {
                        view.setCenter(sf::Vector2f(view.getSize().x, view.getSize().y));
                    }
                    window.setView(view);

                }

            }
            else if (event.type == sf::Event::MouseButtonPressed && !io.WantCaptureMouse) { //make sure the there is no ui above what we are clicking

                //Building Nodes
                if (selectedMain == 1) {
                    if ((view.getCenter().x - 0.5f * view.getSize().x) < mousePosView.x && (view.getCenter().x + 0.5f * view.getSize().x) > mousePosView.x && (view.getCenter().y - 0.5f * view.getSize().y) < mousePosView.y && (view.getCenter().y + 0.5f * view.getSize().y) > mousePosView.y) { // make sure the mouse is inside the view
                        if (!Nodes.empty()) {

                            //Check if the place we clicked in already has a node
                            bool validPlacement = true;
                            int invalidNode;
                            for (int i = 0; i < Nodes.size(); i++) {
                                if (Nodes[i]->locationGrid == selectPosGrid) {
                                    validPlacement = false;
                                    invalidNode = i;
                                    break;
                                }
                            }
                            if (validPlacement) {
                                //Create new Node, of selected type at clicked point
                                std::unique_ptr<Node> newNode = std::make_unique<Node>(selectPosGrid, selectPos, selectedNodeBuild, Nodes.size() + 1);
                                Nodes.push_back(std::move(newNode));

                            }
                            else if (invalidNode == selectedNode) { // Unselect
                                selectedNode = -1;
                            }
                            else { // select new node for config
                                selectedNode = invalidNode;
                            }

                        }
                        else {
                            //Create new Node, of selected type at clicked point
                            std::unique_ptr<Node> newNode = std::make_unique<Node>(selectPosGrid, selectPos, selectedNodeBuild, Nodes.size() + 1);
                            Nodes.push_back(std::move(newNode));

                        }


                    }
                    else {
                        std::cout << (view.getCenter().x - 0.5f * view.getSize().x) << "\n";
                        std::cout << 0.5f * view.getSize().x << "\n";
                        std::cout << mousePosView.x << "\n";
                        std::cout << (view.getCenter().x + 0.5f * view.getSize().x) << "\n";
                    }
                }


                // Building connections(Pipes)
                else if (selectedMain == 2) {
                    if ((view.getCenter().x - 0.5f * view.getSize().x) < mousePosView.x && (view.getCenter().x + 0.5f * view.getSize().x) > mousePosView.x && (view.getCenter().y - 0.5f * view.getSize().y) < mousePosView.y && (view.getCenter().y + 0.5f * view.getSize().y) > mousePosView.y) { // make sure the mouse is inside the view

                        //We will have 2 cases
                        //1. We have already selected a node to build from and want to either A place a new node or B connect it to another existing node
                        //2. We want to select a node to build from

                        if (selectedNode >= 0) {//Case 1
                            if (!Nodes.empty()) { // should be impossible but does not hurt to check
                                bool validPlacement = false;
                                int Nodeid;
                                for (int i = 0; i < Nodes.size(); i++) {
                                    if (Nodes[i]->locationGrid == selectPosGrid) {
                                        validPlacement = true;
                                        Nodeid = i;
                                        break;
                                    }
                                }
                                if (validPlacement && Nodeid != selectedNode) { // Make sure we we dont connect to the same node
                                    std::unique_ptr<Connection> newCon = std::make_unique<Connection>(Nodes[Nodeid].get(), Nodes[selectedNode].get(), Pipes.size() + 1);
                                    Pipes.push_back(std::move(newCon));
                                    selectedNode = -1;

                                }
                                else if (!validPlacement) { //we clicked on a non existing Node so we first create a node there and then connect
                                    std::unique_ptr<Node> newNode = std::make_unique<Node>(selectPosGrid, selectPos, 1, Nodes.size() + 1);
                                    Nodes.push_back(std::move(newNode));
                                    std::unique_ptr<Connection> newCon = std::make_unique<Connection>(Nodes[Nodeid].get(), Nodes[selectedNode].get(), Pipes.size() + 1);
                                    Pipes.push_back(std::move(newCon));
                                    selectedNode = Nodes.size(); // select the new built node as the selected node
                                }
                                else { // We clicked on the alread selected node so we deselect it
                                    selectedNode = -1;
                                }
                            }

                        }
                        else if (selectedNode == -1) { // case 2
                            if (!Nodes.empty()) {
                                bool validPlacement = false;
                                int Nodeid;
                                for (int i = 0; i < Nodes.size(); i++) {
                                    if (Nodes[i]->locationGrid == selectPosGrid) {
                                        validPlacement = true;
                                        Nodeid = i;
                                        break;
                                    }
                                }
                                if (validPlacement) {
                                    selectedNode = Nodeid;
                                }
                            }
                        }



                    }
                }

            }


        }


        //Update
        //Update Input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { // left
            if ((view.getCenter().x - (view.getSize().x / 2.f) - (viewSpeed * dt.asSeconds() * (view.getSize().x / 960))) > 0) // cant move into negatives
                view.move(-viewSpeed * dt.asSeconds() * (view.getSize().x / 960), 0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { // right
            view.move(viewSpeed * dt.asSeconds() * (view.getSize().x / 960), 0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { // down
            if ((view.getCenter().y + (view.getSize().y / 2.f) + (viewSpeed * dt.asSeconds())) > 0)
                view.move(0.f, viewSpeed * dt.asSeconds() * (view.getSize().y / 540));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { // up
            if ((view.getCenter().y - (view.getSize().y / 2.f) - (viewSpeed * dt.asSeconds() * (view.getSize().y / 540))) > 0) // cant move into negatives
                view.move(0.f, -viewSpeed * dt.asSeconds() * (view.getSize().y / 540));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) { // debuggin

            for (int i = 0; i < Nodes.size(); i++) {
                std::cout << Nodes[i]->locationGrid.x << " " << Nodes[i]->locationGrid.y << "\n";
            }


        }





        ImGui::SFML::Update(window, deltaClock.restart());

        //draw ui
        ImGui::Begin("Window Title", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(window.getSize().x * 0.2f, window.getSize().y));
        ImGui::Text(ss.str().c_str()); // display debugging
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Ctrl+W")) { /*Do stuff*/ }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::Button("Run Simulation")) { 
            simulation mysim(this);
            mysim.run();
        }
        if (ImGui::Selectable("Mouse", selectedMain == 0)) { selectedMain = 0; }
        if (ImGui::Selectable("Place Node", selectedMain == 1)) { selectedMain = 1; }
        if (selectedMain == 1) {
            if (ImGui::Selectable("Type 0", selectedNodeBuild == 0)) { selectedNodeBuild = 0; }
            if (ImGui::Selectable("Type 1", selectedNodeBuild == 1)) { selectedNodeBuild = 1; }
            if (ImGui::Selectable("Type 2", selectedNodeBuild == 2)) { selectedNodeBuild = 2; }
        }
        if (ImGui::Selectable("Build Connection", selectedMain == 2)) { selectedMain = 2; }

        ImGui::ShowDemoWindow();
        ImGui::End();
        if (selectedNode >= 0) {
            Nodes[selectedNode]->drawPopup();
        }

        //This will become the permanenet ui for each Node such as its name and attributes being displayed in the view
        //for (int i = 0; i < Nodes.size(); i++) {
         //   Nodes[i]->drawPopup();
        //}


        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window.clear();

        // draw...
        window.draw(shape);
        window.draw(grid);
        //Draw Pipes
        if (!Pipes.empty()) {
            for (int i = 0; i < Pipes.size(); i++) {
                window.draw(*Pipes[i]);
            }
        }
        //Draw the nodes(Draw nodes after pipes so they appear above them)
        if (!Nodes.empty()) {
            for (int i = 0; i < Nodes.size(); i++) {
                window.draw(*Nodes[i]);
            }
        }
      



        // end the current frame (internally swaps the front and back buffers)
        window.setView(window.getDefaultView());




        ImGui::SFML::Render(window);
        window.display();

    }





    // release resources...
    ImGui::SFML::Shutdown();



}