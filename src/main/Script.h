/* 
 * File:   Script.h
 * Author: hans
 *
 * Created on March 13, 2015, 2:00 PM
 */

#ifndef SCRIPT_H
#define	SCRIPT_H


#include <list>
#include <mutex>
#include <stdexcept>

#include <python3.4/Python.h>
#include <boost/python.hpp>

#include "Path.h"
#include "Application.h"

namespace Script {
    
    class ScriptException : public std::runtime_error{
    public:    
        ScriptException(std::string message);
    };
    
    class ScriptExecutionException : public std::runtime_error{
    private:
        std::string name_;
        int line_;
        int column_;
    public:
        ScriptExecutionException(std::string exceptionName, std::string message, int line ,int column);
        
        std::string name() const;
        
        int line() const;
  
        int column() const;
    };
    
    class Executor;
    
    class ScriptSystem{
        friend class Executor;
    public:
        
        static const std::string NAME;
        
        ScriptSystem();
        ~ScriptSystem();
    private:
        
        class InterpreterGuard{
        public:
            InterpreterGuard(Executor *executor);
            ~InterpreterGuard();
            
        private:
            Executor *executor_;
            
            void finalize();
            
            InterpreterGuard(const InterpreterGuard &) = delete;
            InterpreterGuard &operator=(const InterpreterGuard &) = delete;
        };
        
        std::string getCodeFromFile(Core::Path path);
        
        void doExecute(Executor * executor, std::string scriptCode);
        
        void execute(Executor * executor, std::string scriptCode);
        
        template<typename Rep, typename Period> bool execute(Executor *executor, std::string scriptCode, std::chrono::duration<Rep,Period> duration){
            if(mutex_.try_lock_for<Rep, Period>(duration)){
                std::lock_guard<std::timed_mutex> lock{mutex_, std::adopt_lock_t{}};
                doExecute(executor, scriptCode);
                return true;
            }else{
                return false;
            }
        };
        
        bool executeImmediate(Executor *executor, std::string scriptCode);
        
        void executeFile(Executor *executor, Core::Path scriptPath);
        
        template<typename Rep, typename Period> bool executeFile(Executor *executor, Core::Path scriptPath, std::chrono::duration<Rep,Period> duration){
            if(mutex_.try_lock_for<Rep, Period>(duration)){
                std::lock_guard<std::timed_mutex> lock{mutex_, std::adopt_lock_t{}};
                doExecute(executor, getCodeFromFile(scriptPath));
                return true;
            }else{
                return false;
            }
        };
        
        bool executeFileImmediate(Executor *executor, Core::Path scriptPath);
        
        std::timed_mutex mutex_;
    };
    
    
    class Executor{
        friend class ScriptSystem;
    private:
        std::string name_;
    public:
        
        Executor(std::string name);
        
        virtual ~Executor();
        
        void execute(std::string scriptCode);
        
        template<typename Rep, typename Period> bool execute(std::string scriptCode, std::chrono::duration<Rep,Period> duration){
            return Game::ApplicationSystem<ScriptSystem>::instance().execute(this, scriptCode, duration);
        };
        
        bool executeImmediate(std::string scriptCode);
        
        void executeFile(Core::Path scriptPath);
        
        template<typename Rep, typename Period> bool executeFile(Core::Path scriptPath, std::chrono::duration<Rep,Period> duration){
            return Game::ApplicationSystem<ScriptSystem>::instance().execute(this, scriptPath, duration);
        };
        
        bool executeFileImmediate(Core::Path scriptPath);
        
        std::string name() const;
        
    protected:
        virtual void beforeInitialize();
        
        virtual void afterInitialize();
        
        virtual void beforeExecute(boost::python::object mainNamespace);
        
        virtual void afterExecute(boost::python::object mainNamespace);
        
        virtual void beforeFinalize();
        
        virtual void afterFinalize();
        
    private:
        Executor(const Executor &) = delete;
        Executor &operator=(const Executor &) = delete;
    };
    
    class Module{
    public:
        virtual ~Module();
        
        virtual void beforeInitialize();
        
        virtual void afterInitialize();
        
        virtual void beforeExecute(boost::python::object mainNamespace);
        
        virtual void afterExecute(boost::python::object mainNamespace);
        
        virtual void beforeFinalize();
        
        virtual void afterFinalize();
    protected:
        Module();
    };
    
    class ModularExecutor : public Executor{
    public:
        ModularExecutor(std::string name);
        
        ~ModularExecutor();
        
        void addModule(Module *module);
        
        template<typename ModuleType> void addModule(){
            addModule(new ModuleType{});
        };
        
    private:
        
        std::list<Module *> modules_;
        
        void beforeInitialize();
        
        void afterInitialize();
        
        void beforeExecute(boost::python::object mainNamespace);
        
        void afterExecute(boost::python::object mainNamespace);
        
        void beforeFinalize();
        
        void afterFinalize();
    };
    
    class LogModule : public Module{
    public:
        LogModule(std::ostream &output);
        ~LogModule();
        
        void beforeExecute(boost::python::object mainNamespace);
        
        void afterExecute(boost::python::object mainNamespace);
    private:
        std::ostream &output_;
        boost::python::object writer_;
    };
    
    template<typename Object, typename Wrapper> class ExposeObjectModule : public Module{
    public:
        
        using Field = Object *Wrapper::*;
        
        using Method = void (Wrapper::*)(Object *);
    private:
        class Injector{
        public:
            
            virtual ~Injector(){};
            
            virtual void inject(Wrapper &wrapper, Object *object) = 0;
        };
        
        class FieldInjector : public Injector{
        private:
            Field field_;
        public:
            FieldInjector(Field field) : field_(field){};
            
            void inject(Wrapper &wrapper, Object *object){
                (wrapper.*field_) = object;
            };
            
        };
        
        class MethodInjector : public Injector{
        private:
            Method method_;
        public:
            MethodInjector(Method method) : method_(method){};
            
            void inject(Wrapper &wrapper, Object *object){
                (wrapper.*method_)(object);
            };
            
        };
        
        std::string typeName_;
        std::string attributeName_;
        Object *object_;
        bool ownsObject_;
        Injector *injector_;
        
    protected:
        
        ExposeObjectModule(std::string typeName, std::string attributeName, Field field, Object *object = nullptr, bool ownsObject = false) : typeName_(typeName), attributeName_(attributeName), object_(object), ownsObject_(ownsObject), injector_(new FieldInjector{field}){};
        
        ExposeObjectModule(std::string typeName, std::string attributeName, Method method, Object *object = nullptr, bool ownsObject = false) : typeName_(typeName), attributeName_(attributeName), object_(object), ownsObject_(ownsObject), injector_(new MethodInjector{method}){};
        
        virtual void defineWrapper(boost::python::class_<Wrapper> &wrapper) = 0;
        
    public:
        
        virtual ~ExposeObjectModule(){
            if(ownsObject_){
                delete object_;
            }
        };
        
        void setObject(Object *object){
            object_ = object;
        };
        
        void setObject(Object *object, bool ownsObject){
            object_ = object;
            ownsObject_ = ownsObject;
        };
        
        virtual void beforeExecute(boost::python::object mainNamespace){
            if(object_){
                using namespace boost::python;
                class_<Wrapper> wrapperType{typeName_.c_str()};
                defineWrapper(wrapperType);
                object handle{wrapperType()};
                Wrapper &wrapper = extract<Wrapper &>(handle);
                injector_->inject(wrapper, object_);
                mainNamespace[attributeName_.c_str()] = wrapper;
            }else{
                throw ScriptException("exposed object must not be null");
            }
        };
        
        virtual void afterExecute(boost::python::object mainNamespace){
            using namespace boost::python;
            Wrapper &wrapper = extract<Wrapper &>(mainNamespace[attributeName_.c_str()]);
            injector_->inject(wrapper, nullptr);
        };
    };
    
}

#endif	/* SCRIPT_H */

