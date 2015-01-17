/* 
 * File:   JSONTypePolicy.h
 * Author: hans
 *
 * Created on 30 December 2014, 19:47
 */

#ifndef JSON_TYPE_POLICY_H
#define	JSON_TYPE_POLICY_H

#include "JSONType.h"
#include "JSONTree.h"

#include <string>

namespace JSON{
    
    template<typename JSONTraits> class StrictTypePolicy{
    public:
        using Data = typename Tree<JSONTraits>::NodeData;
        using String = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        
        StrictTypePolicy(){};
        
        StrictTypePolicy(Data *data, NodeType expected){
            if(data && data->type() != expected){
                throw TypeException(expected, data->type());
            }
        };
        
        String getString(Data *data) const{
            if(data->type() == NodeType::STRING){
                return data->stringValue();
            }else{
                throw TypeException(NodeType::STRING, data->type());
            }
        };
        
        Number getNumber(Data *data) const{
            if(data->type() == NodeType::NUMBER){
                return data->numberValue();
            }else{
                throw TypeException(NodeType::NUMBER, data->type());
            }
        };
        
        Boolean getBoolean(Data *data) const{
            if(data->type() == NodeType::BOOLEAN){
                return data->booleanValue();
            }else{
                throw TypeException(NodeType::BOOLEAN, data->type());
            }
        };
        
        bool getNull(Data *data) const{
            if(data->type() == NodeType::NULL_VALUE){
                return true;
            }else{
                throw TypeException(NodeType::NULL_VALUE, data->type());
            }
        };
        
        Data *getObject(Data *data) const{
            return data;
        };
        
        typename std::list<Data*>::const_iterator getArrayBegin(Data *data) const{
            if(data){
                return data->arrayValue().begin();
            }else{
                throw JSONException("invalid list node");
            }
        };
        
        typename std::list<Data*>::const_iterator getArrayEnd(Data *data) const{
            if(data){
                return data->arrayValue().end();
            }else{
                throw JSONException("invalid list node");
            }
        };
        
        typename std::list<Data*>::size_type getArraySize(Data *data) const{
            if(data){
                return data->arrayValue().size();
            }else{
                throw JSONException("invalid list node");
            }
        };
        
    };
    
    template<typename JSONTraits> class FastTypePolicy{
    public:
        using Data = typename Tree<JSONTraits>::NodeData;
        using String = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        
        FastTypePolicy(){};
        
        FastTypePolicy(Data *data, NodeType expected){};
        
        String getString(Data *data) const{
            return data->stringValue();
        };
        
        Number getNumber(Data *data) const{
            return data->numberValue();
        };
        
        Boolean getBoolean(Data *data) const{
            return data->booleanValue();
        };
        
        bool getNull(Data *data) const{
            return true;
        };
        
        Data *getObject(Data *data) const{
            return data;
        };
        
        typename std::list<Data*>::const_iterator getArrayBegin(Data *data) const{
            return data->arrayValue().begin();
        };
        
        typename std::list<Data*>::const_iterator getArrayEnd(Data *data) const{
            return data->arrayValue().end();
        };
        
        typename std::list<Data*>::size_type getArraySize(Data *data) const{
            return data->arrayValue().size();
        };
        
    };
    
}


#endif	/* JSONTYPEPOLICY_H */

