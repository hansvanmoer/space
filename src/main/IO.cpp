#include "IO.h"

using namespace Game;

IO::Document Game::IO::open(std::istream &input){
    return Document{JSON::BufferedInput<>{input}};
};
