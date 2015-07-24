#include "Script.h"

#include "String.h"

#include <python3.4/Python.h>
#include <boost/python.hpp>

using namespace Script;
using namespace Core;

const std::string ScriptSystem::NAME{"script executors"};

ScriptException::ScriptException(std::string message) : std::runtime_error(message){};

ScriptExecutionException::ScriptExecutionException(std::string exceptionName, std::string message, int line, int column) : std::runtime_error(message), name_(message), line_(line), column_(column){
}

std::string ScriptExecutionException::name() const {
    return name_;
}

int ScriptExecutionException::line() const {
    return line_;
}

int ScriptExecutionException::column() const {
    return column_;
}

std::string ScriptSystem::getCodeFromFile(Core::Path path){
    std::ifstream buffer;
    path.openFile(buffer);
    if(buffer.good()){
        std::streampos pos = buffer.tellg();
        buffer.seekg(0, std::ios::end);
        std::streamsize size = static_cast<std::streamsize>(buffer.tellg() - pos);
        if(size > 0){
            char *data = new char[size+1];
            buffer.read(data, size);
            data[buffer.gcount()] = '\0';
            std::string str{data};
            delete[] data;
            return str;
        }else{
            throw ScriptException(Core::toString("invalid file length:", path));
        }
    }else{
        throw ScriptException(Core::toString("unable to open file :", path));
    }
}

ScriptSystem::InterpreterGuard::InterpreterGuard(Executor *executor) : executor_(executor) {
    executor->beforeInitialize();
    Py_Initialize();
    try {
        executor->afterInitialize();
    } catch (...) {
        finalize();
        throw;
    }
}

void ScriptSystem::InterpreterGuard::finalize() {
    try {
        executor_->beforeFinalize();
    } catch (std::exception &e) {
        std::cout << "unable to finalize executor: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "unable to finalize executor: unknown exception" << std::endl;
    }
    try {
        Py_Finalize();
        executor_->afterFinalize();
    } catch (std::exception &e) {
        std::cout << "unable to finalize executor: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "unable to finalize executor: unknown exception" << std::endl;
    }
}

ScriptSystem::InterpreterGuard::~InterpreterGuard() {
    finalize();
}

ScriptSystem::ScriptSystem() : mutex_() {
}

void ScriptSystem::doExecute(Executor* executor, std::string scriptCode) {
    using namespace boost::python;
    InterpreterGuard guard{executor};
    try {
        executor->beforeExecute();
        exec(str{scriptCode.c_str()});
        executor->afterExecute();
    } catch (error_already_set &e) {
        PyObject *e, *v, *t;
        PyErr_Fetch(&e, &v, &t);
        object e_obj(handle<>(allow_null(e)));
        object v_obj(handle<>(allow_null(v)));
        std::string name{extract<std::string>(e_obj.attr("__name__"))()};
        PyErr_Restore(e, v, t);
        delete t;
        throw ScriptExecutionException{name,Core::toString("an exception has occurred in executor ", executor->name()), 0, 0};
    }
}

void ScriptSystem::execute(Executor* executor, std::string scriptCode) {
    std::lock_guard<std::timed_mutex> lock{mutex_};
    doExecute(executor, scriptCode);
}

bool ScriptSystem::executeImmediate(Executor* executor, std::string scriptCode) {
    if (mutex_.try_lock()) {
        std::lock_guard<std::timed_mutex> lock{mutex_, std::adopt_lock_t
            {}};
        doExecute(executor, scriptCode);
        return true;
    } else {
        return false;
    }
}

bool ScriptSystem::executeFileImmediate(Executor* executor, Core::Path scriptPath) {
    executeImmediate(executor, getCodeFromFile(scriptPath));
}

void ScriptSystem::executeFile(Executor* executor, Core::Path scriptPath) {
    execute(executor, getCodeFromFile(scriptPath));
}

Executor::Executor(std::string name) : name_(name) {
}

Executor::~Executor() {
}

void Executor::execute(std::string scriptCode) {
    Game::ApplicationSystem<ScriptSystem>::instance().execute(this, scriptCode);
}

bool Executor::executeImmediate(std::string scriptCode) {
    return Game::ApplicationSystem<ScriptSystem>::instance().executeImmediate(this, scriptCode);
}

void Executor::executeFile(Path scriptPath) {
    Game::ApplicationSystem<ScriptSystem>::instance().executeFile(this, scriptPath);
}

bool Executor::executeFileImmediate(Path scriptPath) {
    Game::ApplicationSystem<ScriptSystem>::instance().executeFileImmediate(this, scriptPath);
}

void Executor::beforeInitialize() {

}

void Executor::afterInitialize() {

}

void Executor::beforeExecute() {

}

void Executor::afterExecute() {

}

void Executor::beforeFinalize() {

}

void Executor::afterFinalize() {

}

std::string Executor::name() const {
    return name_;
}

Module::Module() {
}

Module::~Module() {
}

void Module::beforeInitialize() {

}

void Module::afterInitialize() {

}

void Module::beforeExecute() {

}

void Module::afterExecute() {

}

void Module::beforeFinalize() {

}

void Module::afterFinalize() {

}

ModularExecutor::ModularExecutor(std::string name) : Executor(name), modules_() {

}

ModularExecutor::~ModularExecutor() {
    for (auto i : modules_) {
        delete i;
    }
}

void ModularExecutor::beforeInitialize() {
    for (auto i : modules_) {
        i->beforeInitialize();
    }
}

void ModularExecutor::afterInitialize() {
    for (auto i : modules_) {
        i->afterInitialize();
    }
}

void ModularExecutor::beforeExecute() {
    for (auto i : modules_) {
        i->beforeExecute();
    }
}

void ModularExecutor::afterExecute() {
    for (auto i : modules_) {
        i->afterExecute();
    }
}

void ModularExecutor::beforeFinalize() {
    for (auto i : modules_) {
        i->beforeFinalize();
    }
}

void ModularExecutor::afterFinalize() {
    for (auto i : modules_) {
        i->afterFinalize();
    }
}