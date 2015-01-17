/* 
 * File:   JSONTreeBuilder.h
 * Author: hans
 *
 * Created on 15 January 2015, 13:48
 */

#ifndef JSONTREEBUILDER_H
#define	JSONTREEBUILDER_H

#include <stack>

#include "JSONTree.h"

namespace JSON {

    class TreeBuilderException : public JSONException {
    public:
        TreeBuilderException(std::string message);
    };

    template<typename JSONTraits> class BasicTreeBuilder {
    public:
        using String = typename JSONTraits::String;
        using FieldName = String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using NodeData = typename Tree<JSONTraits>::NodeData;
    private:

        std::stack<NodeData*> stack_;
        FieldName fieldName_;
        Tree<JSONTraits> *tree_;
        bool valid_;
        std::string errorMessage_;

        bool isField() {
            if (stack_.empty()) {
                return false;
            } else {
                return stack_.top()->type == NodeType::OBJECT;
            }
        };

        NodeData *addNode(NodeData *data){
            if(stack_.empty()){
                tree_->rootNode(data);
            }else{
                NodeData *parent = stack_.top();
                switch(parent->type()){
                    case NodeType::OBJECT:
                        tree_->addNode(parent, fieldName_, data);
                        break;
                    case NodeType::ARRAY:
                        parent->arrayValue().push_back(data);
                        break;
                    default:
                        throw TreeBuilderException("no child node expected here");
                }
            }
            return data;
        };
        
    public:

        BasicTreeBuilder() : stack_(), fieldName_(), tree_(), valid_(false), errorMessage_(){
        };
        
        BasicTreeBuilder(Tree<JSONTraits> *tree) : stack_(), fieldName_(), tree_(tree), valid_(tree_ != nullptr), errorMessage_(){
        };
        
        ~BasicTreeBuilder(){
            
        };

        Tree<JSONTraits> *tree() const {
            return tree_;
        };
        
        void tree(Tree<JSONTraits> *tree){
            valid_=true;
            tree_ = tree;
        };
        
        void field(FieldName name){
            if(stack_.empty()){
                throw TreeBuilderException("no field expected here");
            }else if(stack_.top()->type() != NodeType::OBJECT){
                throw TreeBuilderException("no field expected here");
            }else{
                fieldName_ = name;
            }
        };

        void boolean(Boolean value){
            addNode(tree_->createBoolean(value));
        };
        
        void number(Number number){
            addNode(tree_->createNumber(number));
        };
        
        void null(){
            addNode(tree_->createNull());
        };
        
        void string(String value){
            addNode(tree_->createString(value));
        };
        
        void objectBegin(){
            stack_.push(addNode(tree_->createObject()));
        };
        
        void objectEnd(){
            if(stack_.empty()){
                throw TreeBuilderException("end of node not expected here");
            }else if(stack_.top()->type() != NodeType::OBJECT){
                throw TreeBuilderException("end of object node not expected here");
            }else{
                stack_.pop();
            }
        };
        
        void arrayBegin(){
            stack_.push(addNode(tree_->createArray()));
        };
        
        void arrayEnd(){
            if(stack_.empty()){
                throw TreeBuilderException("end of node not expected here");
            }else if(stack_.top()->type() != NodeType::ARRAY){
                throw TreeBuilderException("end of array node not expected here");
            }else{
                stack_.pop();
            }
        };
        
        void error(std::string message){
            valid_ = false;
            errorMessage_ = message;
        };
        
        bool valid() const{
            return valid_;
        };
        
        std::string errorMessage() const{
            return errorMessage_;
        };

    };

}

#endif	/* JSONTREEBUILDER_H */

