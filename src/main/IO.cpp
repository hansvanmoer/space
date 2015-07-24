#include "IO.h"
#include "Path.h"

#include <fstream>

using namespace Game;

IO::Document Game::IO::open(std::istream &input){
    return Document{JSON::BufferedInput<>{input}};
};