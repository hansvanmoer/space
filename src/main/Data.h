/* 
 * File:   Data.h
 * Author: hans
 *
 * Created on 22 January 2015, 19:53
 */

#ifndef DATA_H
#define	DATA_H

#include "Application.h"
#include "Path.h"

#include <string>

namespace Game{
    
    class DataSystem{
    public:
        static const std::string NAME;
        
        Path applicationPath();
                
        Path dataPath();
        
        Path runtimeDataPath();
        
    private:
        Path applicationPath_;
        Path dataPath_;
        Path runtimeDataPath_;
        
        DataSystem(Path applicationPath);
        
        friend class ApplicationSystem<DataSystem>;
    };
    
}

#endif	/* DATA_H */

