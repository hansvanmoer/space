/* 
 * File:   JSONNode.h
 * Author: hans
 *
 * Created on 30 December 2014, 15:24
 */

#ifndef JSONNODE_H
#define	JSONNODE_H

#include "JSONTree.h"

#include <sstream>

namespace JSON {

    class UnknownFieldException : public JSONException {
    public:
        UnknownFieldException(std::string msg);
    };

    template<typename JSONTraits, typename TypePolicy> class ObjectNode;

    template<typename JSONTraits, typename TypePolicy> class ArrayNode;

    template<typename JSONTraits> class TreeNodeBase {
    private:
        using Data = typename Tree<JSONTraits>::NodeData;
    protected:
        Tree<JSONTraits> *tree_;
        Data *data_;
        
        TreeNodeBase(){};
        TreeNodeBase(Tree<JSONTraits> *tree, Data *data) : tree_(tree), data_(data){};
    
        NodeType type() const {
            return data_->type;
        };
        
        bool valid() const{
            return data_;
        };
        
        operator bool() const{
            return data_;
        };
        
        bool operator!() const{
            return !data_;
        };
        
        bool operator==(const TreeNodeBase<JSONTraits> &base) const{
            return data_ == base.data_;
        };

        bool operator!=(const TreeNodeBase<JSONTraits> &base) const{
            return data_ != base.data_;
        };
    };

    template<typename JSONTraits, typename TypePolicy> class TreeNode : private TypePolicy, public TreeNodeBase<JSONTraits> {
    public:
        using Data = typename Tree<JSONTraits>::NodeData;
        using String = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using Object = ObjectNode<JSONTraits, TypePolicy>;
        using Array = ArrayNode<JSONTraits, TypePolicy>;
    public:

        TreeNode() : TypePolicy(), TreeNodeBase<JSONTraits>() {
        };

        TreeNode(Tree<JSONTraits> *tree, Data *data) : TypePolicy(), TreeNodeBase<JSONTraits>(tree, data) {
        };

        TreeNode(Tree<JSONTraits> *tree) : TreeNode<JSONTraits, TypePolicy>(tree, tree->rootNode()) {
        };

        String string() const {
            return TypePolicy::getString(TreeNodeBase<JSONTraits>::data_);
        };

        Number number() const {
            return TypePolicy::getNumber(TreeNodeBase<JSONTraits>::data_);
        };

        Boolean boolean() const {
            return TypePolicy::getBoolean(TreeNodeBase<JSONTraits>::data_);
        };

        bool null() const {
            return TreeNodeBase<JSONTraits>::data_->type == NodeType::NULL_VALUE;
        };

        Object object() const {
            return Object{TreeNodeBase<JSONTraits>::tree_, TreeNodeBase<JSONTraits>::data_};
        };

        Array array() const {
            return Array{TreeNodeBase<JSONTraits>::tree_, TreeNodeBase<JSONTraits>::data_};
        };
    };

    template<typename JSONTraits, typename TypePolicy> class ObjectNode : private TypePolicy, public TreeNodeBase<JSONTraits> {
    public:
        using Data = typename Tree<JSONTraits>::NodeData;
        using String = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using Node = TreeNode<JSONTraits, TypePolicy>;
        using Array = ArrayNode<JSONTraits, TypePolicy>;
        using Object = ObjectNode<JSONTraits, TypePolicy>;
        using FieldId = String;
    private:

        Data *getChildData(FieldId fieldId) const {
            Data *data = TreeNodeBase<JSONTraits>::tree_->childNode(TreeNodeBase<JSONTraits>::data_, fieldId);
            if (data) {
                return data;
            } else {
                std::ostringstream msg;
                msg << "unknown field: '";
                JSONTraits::write(msg, fieldId);
                msg << "'";
                throw JSONException(msg.str());
            }
        };

        Data *findChildData(FieldId fieldId) const{
            return TreeNodeBase<JSONTraits>::tree_->childNode(TreeNodeBase<JSONTraits>::data_, fieldId);
            ;
        };

    public:

        ObjectNode() : TypePolicy(), TreeNodeBase<JSONTraits>() {
        };

        ObjectNode(Tree<JSONTraits> *tree, Data *data) : TypePolicy(data, NodeType::OBJECT), TreeNodeBase<JSONTraits>(tree, data) {
        };

        Node node() const{
            return Node{TreeNodeBase<JSONTraits>::tree_, TreeNodeBase<JSONTraits>::data_};
        };
        
        Node getNode(FieldId fieldId) const {
            return Node{TreeNodeBase<JSONTraits>::tree_, getChildData(fieldId)};
        };
        
        Node findNode(FieldId fieldId) const {
            return Node{TreeNodeBase<JSONTraits>::tree_, findChildData(fieldId)};
        };
        
        bool hasNode(FieldId fieldId) const{
            return findChildData(fieldId);
        };

        Object getObject(FieldId fieldId) const {
            return Object{TreeNodeBase<JSONTraits>::tree_, getChildData(fieldId)};
        };
        
        Object findObject(FieldId fieldId) const {
            return Object{TreeNodeBase<JSONTraits>::tree_, findChildData(fieldId)};
        };
        
        bool hasObject(FieldId fieldId) const{
            Data *data = findChildData(fieldId);
            return data && data->type() == NodeType::OBJECT;
        };
        
        Array getArray(FieldId fieldId) const {
            return Array{TreeNodeBase<JSONTraits>::tree_, getChildData(fieldId)};
        };
        
        Array findArray(FieldId fieldId) const {
            return Array{TreeNodeBase<JSONTraits>::tree_, findChildData(fieldId)};
        };
        
        bool hasArray(FieldId fieldId) const{
            Data *data = findChildData(fieldId);
            return data && data->type() == NodeType::ARRAY;
        };
        
        String getString(FieldId fieldId) const {
            return TypePolicy::getString(getChildData(fieldId));
        };
        
        String findString(FieldId fieldId) const {
            return TypePolicy::getString(findChildData(fieldId));
        };
        
        String findString(FieldId fieldId, String defaultValue) const {
            Data *data = findChildData(fieldId);
            if(data){
                return TypePolicy::getString(findChildData(fieldId));
            }else{
                return defaultValue;
            }
        };
        
        bool hasString(FieldId fieldId) const{
            Data *data = findChildData(fieldId);
            return data && data->type() == NodeType::STRING;
        };
        
        Number getNumber(FieldId fieldId) const {
            return TypePolicy::getNumber(getChildData(fieldId));
        };
        
        Number findNumber(FieldId fieldId) const {
            return TypePolicy::getNumber(findChildData(fieldId));
        };
        
        Number findNumber(FieldId fieldId, Number defaultValue) const {
            Data *data = findChildData(fieldId);
            if(data){
                return TypePolicy::getNumber(data);
            }else{
                return defaultValue;
            }
        };
        
        bool hasNumber(FieldId fieldId) const{
            Data *data = findChildData(fieldId);
            return data && data->type() == NodeType::NUMBER;
        };
        
        Boolean getBoolean(FieldId fieldId) const {
            return TypePolicy::getBoolean(getChildData(fieldId));
        };
        
        Boolean findBoolean(FieldId fieldId) const {
            return TypePolicy::getBoolean(findChildData(fieldId));
        };
        
        Boolean findBoolean(FieldId fieldId, Boolean defaultValue) const {
            Data *data = findChildData(fieldId);
            if(data){
                return TypePolicy::getBoolean(data);
            }else{
                return defaultValue;
            }
        };
        
        bool hasBoolean(FieldId fieldId) const{
            Data *data = findChildData(fieldId);
            return data && data->type() == NodeType::BOOLEAN;
        };
        
        void getNull(FieldId fieldId) const{
            TypePolicy::getNull(getChildData(fieldId));
        };
        
        bool findNull(FieldId fieldId) const{
            return TypePolicy::getNull(findChildData(fieldId));
        };
        
        bool hasNull(FieldId fieldId) const{
            Data *data = findChildData(fieldId);
            return data && data->type() == NodeType::NULL_VALUE;
        };
    };
    
    template<typename JSONTraits, typename TypePolicy> class ArrayIterator{
    public:
        using Data = typename Tree<JSONTraits>::NodeData;
        using String = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using Object = ObjectNode<JSONTraits, TypePolicy>;
        using Node = TreeNode<JSONTraits, TypePolicy>;
        using Array = ArrayNode<JSONTraits, TypePolicy>;
    private:
        Tree<JSONTraits> *tree_;
        typename std::list<Data*>::const_iterator iterator_;
        Node *node_;
    public:
        
        ArrayIterator() : tree_(), iterator_(), node_(){};
        
        ArrayIterator(Tree<JSONTraits> *tree, typename std::list<Data*>::const_iterator iterator) : tree_(tree), iterator_(iterator), node_(new Node{tree_, *iterator}){};
        
        ArrayIterator(const ArrayIterator<JSONTraits, TypePolicy> &i) : tree_(i.tree_), iterator_(i.iterator_), node_(){
            if(i.node_){
                node_ = new Node{tree_, *iterator_};
            }
        };
        
        ArrayIterator<JSONTraits, TypePolicy> &operator=(const ArrayIterator<JSONTraits, TypePolicy> &i){
            if(&i != this){
                tree_ = i.tree_;
                iterator_ = i.iterator_;
                delete node_;
                if(i.node_){
                    node_ = new Node{tree_, iterator_};
                }else{
                    node_ = nullptr;
                }
            }
            return *this;
        };
        
        ~ArrayIterator(){
            delete node_;
        };
        
        const Node &operator*() const{
            return *node_;
        };
        
        const Node *operator->() const{
            return node_;
        };
        
        ArrayIterator<JSONTraits, TypePolicy> operator++(int){
            ArrayIterator<JSONTraits, TypePolicy> i{*this};
            iterator_++;
            delete node_;
            node_ = new Node{tree_, *iterator_};
            return i;
        };
        
        ArrayIterator<JSONTraits, TypePolicy> &operator++(){
            iterator_++;
            delete node_;
            node_ = new Node{tree_, *iterator_};
            *this;
        };
        
        bool operator==(const ArrayIterator<JSONTraits, TypePolicy> &i) const{
            return iterator_ == i.iterator_;
        };
        
        bool operator!=(const ArrayIterator<JSONTraits, TypePolicy> &i) const{
            return iterator_ != i.iterator_;
        };
    };
    
    template<typename JSONTraits, typename TypePolicy> class ArrayNode : private TypePolicy, public TreeNodeBase<JSONTraits>{
    public:
        using Data = typename Tree<JSONTraits>::NodeData;
        using String = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using Object = ObjectNode<JSONTraits, TypePolicy>;
        using Node = TreeNode<JSONTraits, TypePolicy>;
        using Iterator = ArrayIterator<JSONTraits, TypePolicy>;
        using const_iterator = Iterator;
        using size_type = typename std::list<Data*>::size_type;
        
        ArrayNode() : TypePolicy(), TreeNodeBase<JSONTraits>() {
        };

        ArrayNode(Tree<JSONTraits> *tree, Data *data) : TypePolicy(data, NodeType::ARRAY), TreeNodeBase<JSONTraits>(tree, data) {
        };
        
        const_iterator begin() const{
            return ArrayIterator<JSONTraits, TypePolicy>{TreeNodeBase<JSONTraits>::tree_, TypePolicy::getArrayBegin(TreeNodeBase<JSONTraits>::data_)};
        };
        
        const_iterator end() const{
            return ArrayIterator<JSONTraits, TypePolicy>{TreeNodeBase<JSONTraits>::tree_, TypePolicy::getArrayEnd(TreeNodeBase<JSONTraits>::data_)};
        };
        
        size_type size() const{
            return TypePolicy::getArraySize(TreeNodeBase<JSONTraits>::data_);
        };
        
    };
}

#endif	/* JSONNODE_H */

