#include "MapGenerator.h"
#include "Session.h"
#include "Module.h"

#include <python3.4/Python.h>
#include <boost/python.hpp>
#include <fstream>

using namespace Game;

const std::string MapGenerator::NAME{"mapgenerator"};

MapGeneratorException::MapGeneratorException(std::string message) : std::runtime_error(message) {
}

MapGenerator::MapGenerator() : systems_(), currentStarSystem_(), currentOrbitalSystem_(), currentOrbit_(), name(), radius(), position(), resourceId(), session_() {
}

MapGenerator::~MapGenerator() {
    delete currentStarSystem_;
    delete currentOrbit_;
    for (StarSystem *system : systems_) {
        delete system;
    }
}

void MapGenerator::beginMap(Session *session) {
    delete currentStarSystem_;
    currentStarSystem_ = nullptr;
    currentOrbitalSystem_ = nullptr;
    delete currentOrbit_;
    currentOrbit_ = nullptr;
    for (StarSystem *system : systems_) {
        delete system;
    }
    systems_.clear();
    session_ = session;
}

StarSystem& MapGenerator::currentSystem() {
    if (currentStarSystem_) {
        return *currentStarSystem_;
    } else {
        throw MapGeneratorException{"no current star system"};
    }
}

void MapGenerator::nextStarSystem() {
    if (currentStarSystem_) {
        systems_.push_back(currentStarSystem_);
    }
    currentStarSystem_ = new StarSystem(name, position);
    currentOrbitalSystem_ = currentStarSystem_;
}

void MapGenerator::circularOrbit(Scalar radius, Scalar angularSpeed, Scalar startAngle) {
    if (currentOrbit_) {
        delete currentOrbit_;
    }
    currentOrbit_ = new CircularOrbit{radius, angularSpeed, startAngle};
}

void MapGenerator::staticOrbit(Position relativePosition) {
    if (currentOrbit_) {
        delete currentOrbit_;
    }
    currentOrbit_ = new StaticOrbit{relativePosition};
}

void MapGenerator::push(OrbitalSystem* orbits) {
    if (currentOrbit_) {
        attach(currentOrbitalSystem_, orbits, currentOrbit_);
        currentOrbitalSystem_ = orbits;
        currentOrbit_ = nullptr;
    } else {
        delete orbits;
        throw MapGeneratorException{"no orbit specified"};
    }
}

void MapGenerator::pushOrbits() {
    push(new OrbitalSystem{});
}

void MapGenerator::pushStar() {
    if (currentStarSystem_) {
        push(new Star{currentStarSystem_, name, radius, starResource()});
    } else {
        throw MapGeneratorException{"no star system specified"};
    }
}

void MapGenerator::pushPlanet() {
    if (currentStarSystem_) {
        push(new Planet{currentStarSystem_, name, radius, planetResource()});
    } else {
        throw MapGeneratorException{"no star system specified"};
    }
}

void MapGenerator::popOrbits() {
    if (currentOrbitalSystem_) {
        OrbitalSystem *parent = currentOrbitalSystem_->orbit()->system();
        if (parent) {
            currentOrbitalSystem_ = parent;
        } else {
            throw MapGeneratorException{"no parent orbital system specified"};
        }
    } else {
        throw MapGeneratorException{"no current orbital system specified"};
    }
}

const OrbitalBodyResource* MapGenerator::planetResource() {
    if (session_) {
        return session_->planetResourceLoader()["gas_giant_01"];
    } else {
        throw MapGeneratorException{"no current session"};
    }
}

const OrbitalBodyResource* MapGenerator::starResource() {
    if (session_) {
        return session_->starResourceLoader()["main_sequence_yellow_01"];
    } else {
        throw MapGeneratorException{"no current session"};
    }
}

void MapGenerator::run(std::string scriptCode) {
    using namespace boost::python;
    boost::python::str buffer{scriptCode.c_str()};
    Py_Initialize();
    object main_module = import("mapgenerator");
    try {
        exec(buffer);
    } catch (boost::python::error_already_set &e) {
        PyErr_Print();
    }
}

class MapGeneratorWrapper{
private:
    MapGenerator *mapGenerator_;
public:
    MapGeneratorWrapper() : mapGenerator_(){};
            
    Scalar getRadius(){
        return mapGenerator_->radius;
    };
    
    void setRadius(Scalar radius){
        mapGenerator_->radius = radius;
    };
        
    Scalar getX(){
        return mapGenerator_->position.x;
    };
    
    void setX(Scalar x){
        mapGenerator_->position.x = x;
    };
    
    Scalar getY(){
        return mapGenerator_->position.y;
    };
    
    void setY(Scalar y){
        mapGenerator_->position.y = y;
    };
    
    void setPosition(Scalar x, Scalar y){
        mapGenerator_->position.x = x;
        mapGenerator_->position.y = y;
    };
        
    std::string getResourceId(){
        return mapGenerator_->resourceId;
    };
    
    void setResourceId(std::string resourceId){
        mapGenerator_->resourceId = resourceId;
    };
    
    void setMapGenerator(MapGenerator *mapGenerator){
        mapGenerator_ = mapGenerator;
    };
   
};

class MapGeneratorModule : public Script::ExposeObjectModule<MapGenerator, MapGeneratorWrapper>{
public:
    MapGeneratorModule(MapGenerator *mapGenerator, bool ownsObject = false) : Script::ExposeObjectModule<MapGenerator, MapGeneratorWrapper>("MapGenerator", "mapGenerator", &MapGeneratorWrapper::setMapGenerator, mapGenerator, ownsObject){};
    

    void defineWrapper(boost::python::class_<MapGeneratorWrapper> &wrapper){
        wrapper.def("getRadius", &MapGeneratorWrapper::getRadius);
        wrapper.def("setRadius", &MapGeneratorWrapper::setRadius);
        wrapper.def("getX", &MapGeneratorWrapper::getX);
        wrapper.def("setX", &MapGeneratorWrapper::setX);
        wrapper.def("getY", &MapGeneratorWrapper::getY);
        wrapper.def("setY", &MapGeneratorWrapper::setY);
        wrapper.def("setPosition", &MapGeneratorWrapper::setPosition);
        wrapper.def("getResourceId", &MapGeneratorWrapper::getResourceId);
        wrapper.def("setResourceId", &MapGeneratorWrapper::setResourceId);
    };
    
};

void Game::testMapGenerator(){
    MapGenerator *mapGenerator = new MapGenerator{};
    using namespace Script;
    ModularExecutor executor{"mapgenerator"};
    executor.addModule(new LogModule(std::cout));
    executor.addModule(new MapGeneratorModule{mapGenerator, true});
    executor.execute("print(mapGenerator.getX())");
};