#include "Resource.h"

using namespace Core;

ResourceException::ResourceException(std::string message) : std::runtime_error(message){
}