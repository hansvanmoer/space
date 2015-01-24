/* 
 * File:   JSONTree.h
 * Author: hans
 *
 * Created on 30 December 2014, 13:47
 */

#ifndef JSON_TREE_H
#define	JSON_TREE_H

#include "JSONType.h"

#include <unordered_map>
#include <list>
#include <algorithm>

namespace JSON{

    class TypeException : public JSONException{
    private:
        NodeType expected_;
        NodeType actual_;
    
        static std::string createMessage(NodeType expected, NodeType actual);
    public:
        
        TypeException(NodeType expected, NodeType actual, std::string msg);
        
        TypeException(NodeType expected, NodeType actual);
    
        NodeType expected() const;
        
        NodeType actual() const;
    };
    
    template<typename JSONTraits> class Tree{
        private:
            
            using FieldName = typename JSONTraits::String;
            using String = typename JSONTraits::String;
            using Number = typename JSONTraits::Number;
            using Boolean = typename JSONTraits::Boolean;
        public:
            class NodeData;
        private:
            
            union NodeContent{
                String *string;
                Number *number;
                Boolean *boolean;
                std::list<NodeData*> *elements;
            };
            
        public:
            class NodeData{
            private:
                NodeType type_;
                NodeContent content_;
            public:
                    
                NodeData(String value) : type_(NodeType::STRING), content_(){
                    content_.string = new String(value);
                };
                    
                NodeData(Number value) : type_(NodeType::NUMBER), content_(){
                    content_.number = new Number(value);
                };
                    
                NodeData(Boolean value) : type_(NodeType::BOOLEAN), content_(){
                    content_.boolean = new Boolean(value);
                };
                
                NodeData(NodeType type) : type_(type){
                    switch(type){
                        case NodeType::STRING:
                            content_.string = new String();
                            break;
                        case NodeType::NUMBER:
                            content_.number = new Number();
                            break;
                        case NodeType::BOOLEAN:
                            content_.boolean = new Boolean();
                            break;
                        case NodeType::ARRAY:
                            content_.elements = new std::list<NodeData*>();
                            break;
                    }
                };
                
                ~NodeData(){
                    switch(type_){
                        case NodeType::STRING:
                            delete content_.string;
                            break;
                        case NodeType::NUMBER:
                            delete content_.number;
                            break;
                        case NodeType::BOOLEAN:
                            delete content_.boolean;;
                            break;
                        case NodeType::ARRAY:
                            delete content_.elements;
                            break;
                    }
                };
                
                const String &stringValue() const{
                    return *content_.string;
                };
                
                const Number &numberValue() const{
                    return *content_.number;
                };
                
                const Boolean &booleanValue() const{
                    return *content_.boolean;
                };
                
                const std::list<NodeData*> &arrayValue() const{
                    return *content_.elements;
                };
                
                String &stringValue(){
                    return *content_.string;
                };
                
                Number &numberValue(){
                    return *content_.number;
                };
                
                Boolean &booleanValue(){
                    return *content_.boolean;
                };
                
                std::list<NodeData*> &arrayValue(){
                    return *content_.elements;
                };
                
                NodeType type() const{
                    return type_;
                };
                
            };
        private:
            
            struct NodeKey{
                NodeData *parent;
                FieldName fieldName;

                NodeKey() : parent(), fieldName(){};
                
                NodeKey(NodeData *parent_, FieldName fieldName_) : parent(parent_), fieldName(fieldName_){};
           
            };
            
            struct NodeKeyEquals{
                bool operator()(const NodeKey &first, const NodeKey &second) const{
                    return first.parent == second.parent && first.fieldName == second.fieldName;
                };
            };

            class NodeKeyHash{
            private:
                std::hash<NodeData*> nodeDataHash_;
                std::hash<FieldName> fieldNameHash_;
            public:    
                NodeKeyHash() : nodeDataHash_(), fieldNameHash_(){};
                
                
                std::size_t operator()(NodeKey key) const{
                    return ((nodeDataHash_(key.parent)) >> 1) ^ ((fieldNameHash_(key.fieldName)) << 1); 
                };
            };
            
            NodeData *rootNode_;
            std::unordered_map<NodeKey,NodeData *, NodeKeyHash, NodeKeyEquals> nodes_;
        public:
            
            Tree() : rootNode_(), nodes_(){}
            
            ~Tree(){
                delete rootNode_;
                for(auto i = nodes_.begin(); i!=nodes_.end(); ++i){
                    delete i->second;
                }
            };
            
            NodeData *rootNode() const{
                return rootNode_;
            };
            
            NodeData *childNode(NodeData *parentNode, FieldName fieldName) const{
                auto found = nodes_.find(NodeKey{parentNode, fieldName});
                if(found == nodes_.end()){
                    return nullptr;
                }else{
                    return found->second;
                }
            };
            
            NodeData *createObject(){
                return new NodeData(NodeType::OBJECT);
            };
            
            NodeData *createNull(){
                return new NodeData(NodeType::NULL_VALUE);
            };
            
            NodeData *createArray(){
                return new NodeData{NodeType::ARRAY};
            };
            
            NodeData *createString(String string){
                return new NodeData(string);
            };
            
            NodeData *createNumber(Number number){
                return new NodeData(number);
            };
            
            NodeData *createBoolean(Boolean boolean){
                return new NodeData(boolean);
            };
            
            void rootNode(NodeData *rootNode){
                rootNode_ = rootNode;
            };
            
            void addNode(NodeData *parent, FieldName fieldName, NodeData *data){
                nodes_.insert(std::make_pair(NodeKey{parent, fieldName}, data));
            };
    };
    
}

#endif	/* JSONTREE_H */

