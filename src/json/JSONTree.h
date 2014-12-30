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
            struct NodeData;
        private:
            
            union NodeContent{
                String string;
                Number number;
                Boolean boolean;
                std::list<NodeData*> elements;
                
                NodeContent(String value){
                    string = value;
                };
                
                NodeContent(Number value){
                    number = value;
                };
                
                NodeContent(Boolean value){
                    boolean = value;
                };
                
                NodeContent(std::list<NodeData*> elements){
                    elements = elements;
                }
            };
            
        public:
            struct NodeData{
                NodeType type;
                NodeContent content;
                
                NodeData(NodeType type_) : type(type_), content(){};
                
                NodeData(String value) : type(NodeType::STRING), content(value){};
                
                NodeData(Number value) : type(NodeType::NUMBER), content(value){};
                
                NodeData(Boolean value) : type(NodeType::BOOLEAN), content(value){};
                
                NodeData(std::list<NodeData*> value) : type(NodeType::ARRAY), content(value){};
                
                String stringValue() const{
                    return content.string;
                };
                
                Number numberValue() const{
                    return content.number;
                };
                
                Boolean booleanValue() const{
                    return content.boolean;
                };
                
                std::list<NodeData*> arrayValue() const{
                    return content.elements;
                };
                
            };
        private:
            
            struct NodeKey{
                const NodeData *parent;
                FieldName fieldName;
                
                NodeKey(NodeData *parent_, FieldName fieldName_) : parent(parent_), fieldName(fieldName_){};
            };
            
            NodeData *rootNode_;
            std::unordered_map<NodeKey,NodeData*> nodes_;
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
                return new NodeData{std::list<NodeData*>{}};
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

