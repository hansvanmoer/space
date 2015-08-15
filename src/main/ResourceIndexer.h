/* 
 * File:   ResourceIndexer.h
 * Author: hans
 *
 * Created on August 11, 2015, 7:46 PM
 */

#ifndef RESOURCEINDEXER_H
#define	RESOURCEINDEXER_H

#include "IO.h"

#include <unordered_map>
#include <set>
#include <stdexcept>

namespace Game{

    class ResourceIndexException : public std::runtime_error{
    public:
        ResourceIndexException(std::string message);
    };
    
    

}

#endif	/* RESOURCEINDEXER_H */

