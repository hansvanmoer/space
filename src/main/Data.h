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
        
        Core::Path applicationPath();
                
        Core::Path dataPath();
        
        Core::Path runtimeDataPath();
        
    private:
        Core::Path applicationPath_;
        Core::Path dataPath_;
        Core::Path runtimeDataPath_;
        
        DataSystem(Core::Path applicationPath);
        
        friend class ApplicationSystem<DataSystem>;
    };
    
}

#endif	/* DATA_H */

