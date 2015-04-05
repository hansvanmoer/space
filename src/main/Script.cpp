#include "Script.h"

#include <python3.4/Python.h>

using namespace Script;

ScriptException::ScriptException(std::string message) : std::runtime_error(message) {
}

ScriptException::~ScriptException() {
}

const std::string ExecutorSystem::NAME{"script"};

ExecutorSystem::ExecutorSystem() : mutex_() {
}

ExecutorSystem::~ExecutorSystem() {
}

void ExecutorSystem::doExecute(Executor* executor, std::string script) {
    executor->beforeInit();
    Py_Initialize();
    executor->beforeExecution();
    PyRun_SimpleString(script.data());
    executor->afterExecution();
    Py_Finalize();
    executor->afterFinalize();
}

void ExecutorSystem::execute(Executor* executor, std::string script) {
    std::lock_guard<std::mutex> lock{mutex_};
    doExecute(executor, script);
}

bool ExecutorSystem::executeImmediate(Executor* executor, std::string script) {
    std::unique_lock<std::mutex> lock{mutex_, std::try_to_lock_t
        {}};
    if (lock.owns_lock()) {
        try {
            doExecute(executor, script);
            lock.unlock();
            return true;
        } catch (...) {
            lock.unlock();
            throw;
        }
    }
    return false;
}

Executor::Executor() {
}

Executor::~Executor() {
}

void Executor::beforeExecution() {
}

void Executor::afterExecution() {
}

void Executor::beforeInit() {
}

void Executor::afterFinalize() {
}

void Executor::execute(std::string script) {
    Game::ApplicationSystem<ExecutorSystem>::instance().execute(this, script);
}

bool Executor::executeImmediate(std::string script) {
    Game::ApplicationSystem<ExecutorSystem>::instance().execute(this, script);
}

/*
 * Log redirect module
 */

Module::Module(std::string name) : name_(name){}

Module::~Module(){}

std::string Module::name() const {
    return name_;
}

bool Module::ready(std::ostream& errorOutput) {
    return true;
}

void Module::beforeInit() {}

void Module::beforeExecution(){}

void Module::afterExecution(){}

void Module::afterFinalize(){}

ModuleLoader::ModuleLoader() : modules_(), moduleMap_(){}

void ModuleLoader::add(Module* module){
    std::type_index type{typeid(module)};
    auto found = moduleMap_.find(type);
    if(found == moduleMap_.end()){
        modules_.push_back(module);
        moduleMap_.insert(std::pair<std::type_index, Module *>{type, module});
    }else{
        throw ScriptException(Core::toString("module with type '", type.name(),"' already added"));
    }
}

void ModuleLoader::remove(Module* module) {
    std::type_index type{typeid(module)};
    auto found = moduleMap_.find(type);
    if(found == moduleMap_.end()){
        throw ScriptException(Core::toString("module with type '", type.name(),"' not found"));
    }else{
        modules_.remove(module);
        moduleMap_.erase(found);
        delete module;
    }
}

ModuleLoader::~ModuleLoader(){
    for(auto module : modules_){
        delete module;
    }
}



void ModuleLoader::beforeInit() {
    std::ostringstream buffer;
    for(auto module : modules_){
        if(!module->ready(buffer)){
            throw ScriptException(Core::toString("unable to initialize module: '", module->name(), "' : ", buffer.str()));
        }
    }
    for(auto module : modules_){
        module->beforeInit();
    }
}

void ModuleLoader::beforeExecution() {
    for(auto module : modules_){
        module->beforeExecution();
    }
}

void ModuleLoader::afterExecution() {
    for(auto module : modules_){
        module->afterExecution();
    }
}

void ModuleLoader::afterFinalize() {
    for(auto module : modules_){
        module->afterFinalize();
    }
}


namespace LogRedirectModule_ {

    PyObject *moduleObject;
    std::ostream *output_;

    /*
     * Writer type
     */

    struct Writer{
        PyObject_HEAD
    };

    namespace WriterType {

        static PyObject *write(Writer* self, PyObject *args) {
            if (output_) {
                char *buffer = 0;
                if(PyArg_ParseTuple(args, "s", &buffer)){
                    (*output_) << buffer << std::endl;
                }else{
                    PyErr_SetString(PyExc_IOError, "input string");
                }
            } else {
                PyErr_SetString(PyExc_AttributeError, "output");
            }
            return (PyObject*)self;
        }
        
        static PyObject *flush(Writer* self) {
            if (output_) {
                output_->flush();
            } else {
                PyErr_SetString(PyExc_AttributeError, "output");
            }
            return (PyObject*)self;
        }

        static PyMethodDef methods[] = {
            {"write", (PyCFunction) write, METH_VARARGS, "writes to an external output buffer"},
            {"flush", (PyCFunction) flush, METH_NOARGS, "flushes the external output buffer"},
            {NULL}
        };

        static PyObject *newWriter(PyTypeObject *type, PyObject *args, PyObject *kwds) {
            return type->tp_alloc(type, 0);
        };

        static void deleteWriter(PyObject* self) {
            Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*> (self));
        };

        static int initWriter(Writer *self, PyObject *args, PyObject *kwds) {
            return 0;
        };

        static PyTypeObject type = {
            PyVarObject_HEAD_INIT(NULL, 0)
            "logredirect.Writer", /* tp_name */
            sizeof (Writer), /* tp_basicsize */
            0, /* tp_itemsize */
            static_cast<destructor> (deleteWriter), /* tp_dealloc */
            0, /* tp_print */
            0, /* tp_getattr */
            0, /* tp_setattr */
            0, /* tp_reserved */
            0, /* tp_repr */
            0, /* tp_as_number */
            0, /* tp_as_sequence */
            0, /* tp_as_mapping */
            0, /* tp_hash  */
            0, /* tp_call */
            0, /* tp_str */
            0, /* tp_getattro */
            0, /* tp_setattro */
            0, /* tp_as_buffer */
            Py_TPFLAGS_DEFAULT, /* tp_flags */
            "Writes content to an external output stream", /* tp_doc */
            0, /* tp_traverse */
            0, /* tp_clear */
            0, /* tp_richcompare */
            0, /* tp_weaklistoffset */
            0, /* tp_iter */
            0, /* tp_iternext */
            methods, /* tp_methods */
            0, /* tp_members */
            0, /* tp_getset */
            0, /* tp_base */
            0, /* tp_dict */
            0, /* tp_descr_get */
            0, /* tp_descr_set */
            0, /* tp_dictoffset */
            (initproc) initWriter, /* tp_init */
            PyType_GenericAlloc, /* tp_alloc */
            PyType_GenericNew, /* tp_new */
        };

    }

    static PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "logredirect",
        "Log redirect module.",
        -1,
        NULL, NULL, NULL, NULL, NULL
    };

    PyMODINIT_FUNC initModule() {
        if (!moduleObject) {
            PyObject* m;
            if (PyType_Ready(&WriterType::type) < 0)
                return NULL;

            m = PyModule_Create(&module);
            if (m == NULL)
                return NULL;

            Py_INCREF(&WriterType::type);
            PyModule_AddObject(m, "Writer", (PyObject *) & WriterType::type);
            moduleObject = m;
            return moduleObject;
        } else {
            return NULL;
        }
    }

}

LogModule::LogModule() : Module{"logredirect"}, output_(), ownsOutput_(false){
}

LogModule::LogModule(std::ostream* output, bool ownsOutput) : Module{"logredirect"}, output_(output), ownsOutput_(ownsOutput){
}

void LogModule::output(std::ostream* output, bool ownsOutput) {
    if(ownsOutput_){
        delete output_;
    }
    ownsOutput_ = ownsOutput;
    output_ = output;
}

std::ostream* LogModule::output() const {
    return output_;
}

bool LogModule::ownsOutput() const {
    return ownsOutput_;
}

bool LogModule::ready(std::ostream& errorOutput) {
    if(output_){
        return true;
    }else{
        errorOutput << "log redirect stream not set";
        return false;
    }
}

void LogModule::beforeInit() {
    LogRedirectModule_::output_ = output_;
    PyImport_AppendInittab("logredirect", &LogRedirectModule_::initModule);
}

void LogModule::beforeExecution() {
    PyRun_SimpleString("import sys\nimport logredirect\nsys.stdout=logredirect.Writer()");
}

LogModule::~LogModule(){
    if(ownsOutput_){
        delete output_;
    }
}
