#include "JSONReader.h"

int main(int argCount, const char **args){
    typedef JSON::Document<JSON::BufferedInput<> > Document;
    typedef typename Document::Object Object;
    
    Document document{JSON::BufferedInput<>{std::istringstream{"{\"field1\" : true , \"field2\":\"test\", \"field3\":{\"field4\":true}}"}}};
    std::cout << document.rootNode().object().getObject("field3").getBoolean("field4");
    return 0;
};
