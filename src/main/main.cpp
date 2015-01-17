#include "JSONReader.h"

int main(int argCount, const char **args){
        JSON::Document<JSON::BufferedInput<> > document{JSON::BufferedInput<>{std::istringstream{"true"}}};
        std::cout << "testing";
        std::cout << document.rootNode().boolean();
        return 0;
};
