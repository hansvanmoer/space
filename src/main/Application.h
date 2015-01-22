/* 
 * File:   Application.h
 * Author: hans
 *
 * Created on 22 January 2015, 19:26
 */

#ifndef APPLICATION_H
#define	APPLICATION_H

#include <stdexcept>
#include <string>

namespace Game{
    
    class ApplicationException : public std::runtime_error{
    public:
        ApplicationException(std::string message);
    };
    
    template<typename System> class ApplicationSystem{
    public:
                
        static System &instance(){
            if(instance_){
                return *instance_;
            }else{
                throw ApplicationException(std::string{"subsystem not ready: "}+System::NAME);
            }
        };
        
        template<typename... T> static void initialize(T... args){
            if(instance_){
                throw ApplicationException(std::string{"subsystem already initialized: "}+System::NAME);
            }else{
                instance_ = new System{args...};
            }
        };
        
        static void shutdown(){
            if(instance_){
                delete instance_;
                instance_ = nullptr;
            }else{
                throw ApplicationException(std::string{"subsystem not ready: "}+System::NAME);
            }
        };
        
    private:
        static System *instance_;
    };
    
}

template<typename System> System *Game::ApplicationSystem<System>::instance_{};

#endif	/* APPLICATION_H */

