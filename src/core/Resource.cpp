#include "Resource.h"

#include "String.h"

using namespace Core;

ResourceException::ResourceException(std::string message) : std::runtime_error(message){
}

template<typename Key>
ResourceNotFoundException<Key>::ResourceNotFoundException(Key key) : ResourceException(toString("resource with key '", key, "' not found")), key_(key){
}

template<typename Key>
Key ResourceNotFoundException<Key>::key() const {
    return key_;
}