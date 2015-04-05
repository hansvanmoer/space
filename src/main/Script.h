/* 
 * File:   Script.h
 * Author: hans
 *
 * Created on March 13, 2015, 2:00 PM
 */

#ifndef SCRIPT_H
#define	SCRIPT_H

#include "Application.h"

#include <list>
#include <mutex>
#include <typeinfo>
#include <unordered_map>
#include <condition_variable>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include <typeindex>

#include "String.h"

namespace Script {

    class Executor;

    class ExecutorSystem {
    public:
        
        static const std::string NAME;
        
        ExecutorSystem();
        ~ExecutorSystem();

        void execute(Executor *executor, std::string script);

        template<typename Rep, typename Period> bool execute(Executor *executor, std::chrono::duration<Rep, Period> timeout, std::string script) {
            std::unique_lock<std::mutex> lock{mutex_, timeout};
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
        };

        bool executeImmediate(Executor *executor, std::string script);
    private:
        std::mutex mutex_;

        void doExecute(Executor *executor, std::string script);
    };

    class ScriptException : public std::runtime_error {
    public:
        ScriptException(std::string message);

        virtual ~ScriptException();
    };

    class Executor {
    public:
        Executor();

        virtual ~Executor();

        void execute(std::string script);

        bool executeImmediate(std::string script);
        
        template<typename Rep, typename Period> bool execute(Executor *executor, std::chrono::duration<Rep, Period> timeout, std::string script) {
            Game::ApplicationSystem<ExecutorSystem>::instance().execute(this, timeout, script);
        };

    protected:

        virtual void beforeInit();
        
        virtual void beforeExecution();

        virtual void afterExecution();
        
        virtual void afterFinalize();

        friend class ExecutorSystem;

    private:
        Executor(const Executor &) = delete;
        Executor &operator=(const Executor &) = delete;
    };

    template<typename ExecutorPolicy, typename... ExecutorPolicies> class BasicExecutorBase : public ExecutorPolicy, public BasicExecutorBase<ExecutorPolicies...> {
    public:

        virtual ~BasicExecutorBase() {
        };

    protected:
        
        BasicExecutorBase() : ExecutorPolicy(), BasicExecutorBase<ExecutorPolicies...>() {
        };

        virtual void beforeInit(){
            ExecutorPolicy::beforeInit();
            BasicExecutorBase < ExecutorPolicies...>::beforeInit();
        };
        
        virtual void beforeExecution() {
            ExecutorPolicy::beforeExecution();
            BasicExecutorBase < ExecutorPolicies...>::beforeExecution();
        };

        virtual void afterExecution() {
            BasicExecutorBase < ExecutorPolicies...>::afterExecution();
            ExecutorPolicy::afterExecution();
        };
        
        virtual void afterFinalize(){
            BasicExecutorBase < ExecutorPolicies...>::afterFinalize();
            ExecutorPolicy::afterFinalize();
        };
    };

    template<typename ExecutorPolicy> class BasicExecutorBase<ExecutorPolicy> : public ExecutorPolicy{
    public:
        virtual ~BasicExecutorBase() {
        };

    protected:

        BasicExecutorBase() {
        };
        
        virtual void beforeInit(){
            ExecutorPolicy::beforeInit();
        };
        
        virtual void beforeExecution() {
            ExecutorPolicy::beforeExecution();
        };

        virtual void afterExecution() {
            ExecutorPolicy::afterExecution();
        };
        
        virtual void afterFinalize(){
            ExecutorPolicy::afterFinalize();
        };
    };
    
    template<typename ExecutorPolicy, typename... ExecutorPolicies> class BasicExecutor : public virtual Executor, public BasicExecutorBase<ExecutorPolicy, ExecutorPolicies...>{
    public:
        BasicExecutor() : Executor(),  BasicExecutorBase<ExecutorPolicy, ExecutorPolicies...>(){};
        
        virtual ~BasicExecutor(){};
        
    protected:
        virtual void beforeInit() {
            BasicExecutorBase<ExecutorPolicy, ExecutorPolicies...>::beforeInit();
        };
        
        virtual void beforeExecution() {
            BasicExecutorBase<ExecutorPolicy, ExecutorPolicies...>::beforeExecution();
        };

        virtual void afterExecution(){
            BasicExecutorBase<ExecutorPolicy, ExecutorPolicies...>::afterExecution();
        };
        
        virtual void afterFinalize(){
            BasicExecutorBase<ExecutorPolicy, ExecutorPolicies...>::afterFinalize();
        };
    };
    
    class ModuleLoader;
    
    class Module{
    public:
        
        virtual ~Module();
        
        std::string name() const;
        
    protected:
        
        Module(std::string name);
        
        virtual bool ready(std::ostream &errorOutput);
        
        virtual void beforeInit();
        
        virtual void beforeExecution();
        
        virtual void afterExecution();
        
        virtual void afterFinalize();
        
    private:
        
        std::string name_;

        friend class ModuleLoader;
        
        Module(const Module &) = delete;
        Module &operator=(const Module &) = delete;
    };
    
    class ModuleLoader : public virtual Executor{
    public:
        
        ModuleLoader();
        
        virtual ~ModuleLoader();
        
        void add(Module *module);
        
        void remove(Module *module);
        
    protected:
        
        virtual void beforeInit();
        
        virtual void beforeExecution();
        
        virtual void afterExecution();
        
        virtual void afterFinalize();
        
    private:
        
        template<typename ModuleType> friend bool hasModule(const ModuleLoader &loader);
        template<typename ModuleType> friend ModuleType &getModule(const ModuleLoader &loader);
        template<typename ModuleType> friend ModuleType &addModule(ModuleLoader &loader, ModuleType *module);
        template<typename ModuleType> friend void removeModule(ModuleLoader &loader);
        
        std::list<Module *> modules_;
        std::unordered_map<std::type_index, Module *> moduleMap_;
    };
    
    
    template<typename ModuleType> bool hasModule(const ModuleLoader &loader){
        return loader.moduleMap_.find(std::type_index{typeid(ModuleType *)}) != loader.moduleMap_.end();
    };
    
    template<typename ModuleType> ModuleType &getModule(const ModuleLoader &loader){
        std::type_index type{typeid(ModuleType *)};
        auto found = loader.moduleMap_.find(type);
        if(found == loader.moduleMap_.end()){
            throw ScriptException(Core::toString("no such module: ", type.name()));
        }else{
            return dynamic_cast<ModuleType &>(*found->second);
        }
    };
    
    template<typename ModuleType> ModuleType &addModule(ModuleLoader &loader, ModuleType *module){
        std::type_index type{typeid(ModuleType *)};
        auto found = loader.moduleMap_.find(type);
        if(found == loader.moduleMap_.end()){
            loader.moduleMap_.insert(std::pair<std::type_index, Module *>{type, module});
            loader.modules_.push_back(module);
        }else{
            throw ScriptException(Core::toString("module '", module->name(), "' already added"));
        }
        return dynamic_cast<ModuleType &>(*module);
    };
    
    template<typename ModuleType> ModuleType &addModule(ModuleLoader &loader){
        ModuleType *module = new ModuleType{};
        try{
            addModule<ModuleType>(loader, module);
        }catch(...){
            delete module;
            throw;
        }
    };
    
    template<typename ModuleType> void removeModule(ModuleLoader &loader){
        std::type_index type{typeid(ModuleType *)};
        auto found = loader.moduleMap_.find(type);
        if(found == loader.moduleMap_.end()){
            throw ScriptException(Core::toString("module '", type.name(), "' not found"));
        }else{
            Module *module = found->second;
            loader.moduleMap_.erase(found);
            loader.modules_.remove(module);
            delete module;
        }
    };
    
    class LogModule : public Module{
    public:
        
        LogModule();
        
        LogModule(std::ostream *output, bool ownsOutput = false);
        
        void output(std::ostream *output, bool ownsOutput = false);
        
        std::ostream *output() const;
        
        bool ownsOutput() const;
        
        ~LogModule();
        
    protected:
        
        bool ready(std::ostream &errorOutput);
        
        void beforeInit();
        
        void beforeExecution();
        
    private:
        
        std::ostream *output_;
        bool ownsOutput_;
    };

    
}

#endif	/* SCRIPT_H */

