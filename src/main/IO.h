/* 
 * File:   IO.h
 * Author: hans
 *
 * Created on 26 January 2015, 20:06
 */

#ifndef IO_H
#define	IO_H

#include "JSONReader.h"

namespace Game{
    
    namespace IO{
        using Document = JSON::Document<JSON::BufferedInput<> >;
        using Object = typename JSON::Document<JSON::BufferedInput<> >::Object;
        using Array = typename JSON::Document<JSON::BufferedInput<> >::Array;
        using ArrayIterator = typename Array::Iterator;
        
        Document open(std::istream &input);
    }
    
}

#endif	/* IO_H */

