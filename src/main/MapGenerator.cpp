#include "MapGenerator.h"
#include "Session.h"
#include "Module.h"

#include <python3.4/Python.h>
#include <fstream>

using namespace Game;

const std::string MapGenerator::NAME{"mapgenerator"};

MapGeneratorException::MapGeneratorException(std::string message) : std::runtime_error(message){}

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

void MapGenerator::run(std::string scriptCode){
    using namespace Script;
    BasicExecutor<Script::ModuleLoader> executor{};
    addModule<MapGeneratorModule>(executor).mapGenerator(this);
    try{
        executor.execute(scriptCode);
    }catch(ScriptException &e){
        throw MapGeneratorException{e.what()};
    }
}

void MapGenerator::run(Core::Path scriptPath) {
    std::ifstream input;
    if(scriptPath.openFile(input)){
        std::streampos pos = input.tellg();
        input.seekg(0, std::ios::end);
        std::streamsize length = static_cast<std::streamsize>(input.tellg() - pos);
        input.seekg(pos);
        if(length > 0){
            char *buffer = new char[length+1];
            input.read(buffer, length);
            buffer[length] = '\0';
            try{
                run(std::string{buffer});
            }catch(...){
                delete[] buffer;
                throw;
            }
            delete[] buffer;
        }else{
            throw MapGeneratorException{Core::toString("unable to open script ", scriptPath.data(), " : file is empty or not readable")};
        }
    }else{
        throw MapGeneratorException{Core::toString("unable to open script ", scriptPath.data())};
    }
}


namespace MapGenerator_ {

    PyObject *moduleObject;
    MapGenerator *mapGenerator;

    static MapGenerator &generator(){
        if(mapGenerator){
            return *mapGenerator;
        }else{
            throw MapGeneratorException("map generator not initialized");
        }
    }
    
    static PyObject* setPosition(PyObject *self, PyObject *args) {
        try{
            double x = 0.;
            double y = 0.;
            if(PyArg_ParseTuple(args, "dd", &x, &y)){
                generator().position = Position{x,y};
                return self;
            }else{
                PyErr_SetString(PyExc_RuntimeError, "unable to parse arguments: expected <double x, double y>");
                return NULL;
            }
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* setRadius(PyObject *self, PyObject *args) {
        try{
            double radius = 0.;
            if(PyArg_ParseTuple(args, "d", &radius)){
                generator().radius = radius;
                return self;
            }else{
                PyErr_SetString(PyExc_RuntimeError, "unable to parse arguments: expected <double radius>");
                return NULL;
            }
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* setName(PyObject *self, PyObject *args) {
        try{
            const char *name;
            if(PyArg_ParseTuple(args, "s", &name)){
                generator().name = std::u32string{name, name+strlen(name)};
                return self;
            }else{
                PyErr_SetString(PyExc_RuntimeError, "unable to parse arguments: expected <string name>");
                return NULL;
            }
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* setResourceId(PyObject *self, PyObject *args) {
        try{
            const char *resourceId;
            if(PyArg_ParseTuple(args, "s", &resourceId)){
                generator().resourceId = std::string{resourceId};
                return self;
            }else{
                PyErr_SetString(PyExc_RuntimeError, "unable to parse arguments: expected <string resourceId>");
                return NULL;
            }
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* staticOrbit(PyObject *self, PyObject *args) {
        try{
            double x = 0.;
            double y = 0.;
            if(PyArg_ParseTuple(args, "dd", &x, &y)){
                generator().staticOrbit(Position{x,y});
                return self;
            }else{
                PyErr_SetString(PyExc_RuntimeError, "unable to parse arguments: expected <double x, double y>");
                return NULL;
            }
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* circularOrbit(PyObject *self, PyObject *args) {
        try{
            double radius = 0.;
            double angularSpeed = 0.;
            double startAngle = 0;
            if(PyArg_ParseTuple(args, "ddd", &radius, &angularSpeed, &startAngle)){
                generator().circularOrbit(radius, angularSpeed, startAngle);
                return self;
            }else{
                PyErr_SetString(PyExc_RuntimeError, "unable to parse arguments: expected <double radius, double angularSpeed, double startAngle>");
                return NULL;
            }
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* nextStarSystem(PyObject *self, PyObject *args) {
        try{
            generator().nextStarSystem();
            return self;
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* pushOrbits(PyObject *self, PyObject *args) {
        try{
            generator().pushOrbits();
            return self;
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* pushStar(PyObject *self, PyObject *args) {
        try{
            generator().pushStar();
            return self;
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* pushPlanet(PyObject *self, PyObject *args) {
        try{
            generator().pushPlanet();
            return self;
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };
    
    static PyObject* popOrbits(PyObject *self, PyObject *args) {
        try{
            generator().popOrbits();
            return self;
        }catch(std::exception &e){
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return NULL;
        }
    };

    static PyMethodDef moduleMethods[] = {
        {"setPosition", setPosition, METH_VARARGS, "Sets the current object position"},
        {"setName", setName, METH_VARARGS, "Sets the current object name"},
        {"setRadius", setRadius, METH_VARARGS, "Sets the current object radius"},
        {"setResourceId", setResourceId, METH_VARARGS, "Sets the current object resource id"},
        {"staticOrbit", staticOrbit, METH_VARARGS, "Sets the current orbit to a fixed position relative to its parent system"},
        {"circularOrbit", circularOrbit, METH_VARARGS, "Sets the current orbit to a circular orbit around its parent system"},
        {"nextStarSystem", nextStarSystem, METH_NOARGS, "Saves the current star system and starts the next"},
        {"pushOrbits", pushOrbits, METH_NOARGS, "Pushes the current orbital system on the stack"},
        {"pushStar", pushStar, METH_NOARGS, "Pushes the current star on the stack"},
        {"pushPlanet", pushPlanet, METH_NOARGS, "Pushes the current planet on the stack"},
        {"popOrbits", popOrbits, METH_NOARGS, "Pops the current orbital system from the stack, returning to the parent orbital system"},
        {NULL, NULL, 0, NULL}
    };

    static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "mapgenerator",
        "Map generator module.",
        -1,
        moduleMethods,
        NULL, NULL, NULL, NULL
    };

    PyMODINIT_FUNC initMapGeneratorModule() {
        if (!moduleObject) {
            PyObject* m;

            m = PyModule_Create(&module);
            if (m == NULL)
                return NULL;
            moduleObject = m;
            return moduleObject;
        } else {
            return NULL;
        }
    }
    
    static void destroyMapGeneratorModule(){    
        mapGenerator = nullptr;
    }
}

MapGeneratorModule::MapGeneratorModule() : Script::Module("mapgenerator"), mapGenerator_() {
};

bool MapGeneratorModule::ready(std::ostream& errorOutput) {
    if (mapGenerator_) {
        return true;
    } else {
        errorOutput << "map generator not initialized";
        return false;
    }
}

void MapGeneratorModule::mapGenerator(MapGenerator* mapGenerator) {
    mapGenerator_ = mapGenerator;
}

void MapGeneratorModule::beforeInit() {
    MapGenerator_::mapGenerator = mapGenerator_;
    PyImport_AppendInittab("mapgenerator", &MapGenerator_::initMapGeneratorModule);
}

void MapGeneratorModule::beforeExecution() {
    
}

void MapGeneratorModule::afterFinalize() {
    MapGenerator_::destroyMapGeneratorModule();
}
