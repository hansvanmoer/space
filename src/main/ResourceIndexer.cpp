#include "ResourceIndexer.h"

#include "Parser.h"

using namespace Game;

ResourceIndexException::ResourceIndexException(std::string message) : std::runtime_error(message){};
