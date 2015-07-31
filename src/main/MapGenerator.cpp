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

std::u32string name;

void setName(std::u32string name){
    ApplicationSystem<MapGenerator>::instance().name = name;
};

void setRadius(Scalar radius){
    ApplicationSystem<MapGenerator>::instance().radius = radius;
};

void setPosition(Scalar x, Scalar y){
    ApplicationSystem<MapGenerator>::instance().position = Position{x,y};
};

void setResourceId(std::string resourceId){
    ApplicationSystem<MapGenerator>::instance().resourceId = resourceId;
};

class Writer{
public:
    void print(std::string msg){
        std::cout << msg;
    };
};

BOOST_PYTHON_MODULE(mapgenerator) {
    using namespace boost::python;

    def("setName", setName);
    def("setRadius", setRadius);
    def("setPosition", setPosition);
    def("setResourceId", setResourceId);
    
}

void Game::testMapGenerator(){
    /*using namespace boost::python;
    
    PyImport_AppendInittab( "mapgenerator", &PyInit_mapgenerator);
    Py_Initialize();
    
    
    try{
        object main_module((handle<>(borrowed(PyImport_AddModule("__main__")))));
        object main_namespace = main_module.attr("__dict__");
        //object cpp_module( (handle<>(PyImport_ImportModule("mapgenerator"))) );
        //main_namespace["mapgenerator"] = cpp_module;
        boost::python::exec("import mapgenerator", main_namespace, main_namespace);
    }catch(...){
        std::cout << "python error:" <<std::endl;
        PyErr_Print();
    }
    //boost::python::exec("i=1");
     * */
    
    
    using namespace Script;
    
    ApplicationSystem<ScriptSystem>::initialize();
    ModularExecutor executor{"mapgenerator"};
    executor.addModule(new LogModule(std::cout));
    executor.execute("print(\"test\")");
};