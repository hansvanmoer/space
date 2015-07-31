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
    
}

#endif	/* SCRIPT_H */

