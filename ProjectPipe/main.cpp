#pragma once

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
#include "Node.hpp"
#include "Connection.hpp"
#include "App.hpp"




int main()
{
    App app;
    app.Run();
    
    return 0;
  
}