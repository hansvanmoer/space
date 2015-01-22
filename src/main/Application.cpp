#include <stdexcept>

#include "Application.h"

using namespace Game;

ApplicationException::ApplicationException(std::string message)  : std::runtime_error(message){}