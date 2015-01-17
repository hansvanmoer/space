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

        Data *getChildData(FieldId fieldId) {
            Data *data = TreeNodeBase<JSONTraits>::tree_->childNode(TreeNodeBase<JSONTraits>::data_, fieldId);
            if (data) {
                return data;
            } else {
                std::ostringstream msg;
                msg << "unknown field: '";
                JSONTraits::print(msg, fieldId);
                msg << "'";
                throw UnknownFieldException(msg.str());
            }
        };

        Data *findChildData(FieldId fieldId) {
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

        Object getObject(FieldId fieldId) const {
            return Object{TreeNodeBase<JSONTraits>::tree_, getChildData(fieldId)};
        };
        
        Object findObject(FieldId fieldId) const {
            return Object{TreeNodeBase<JSONTraits>::tree_, findChildData(fieldId)};
        };
        
        Array getArray(FieldId fieldId) const {
            return Node{TreeNodeBase<JSONTraits>::tree_, getChildData(fieldId)};
        };
        
        Array findArray(FieldId fieldId) const {
            return Array{TreeNodeBase<JSONTraits>::tree_, findChildData(fieldId)};
        };
        
        String getString(FieldId fieldId) const {
            return TypePolicy::getString(getNode(fieldId));
        };
        
        String findString(FieldId fieldId) const {
            return TypePolicy::getString(findNode(fieldId));
        };
        
        String getNumber(FieldId fieldId) const {
            return TypePolicy::getNumber(getNode(fieldId));
        };
        
        String findNumber(FieldId fieldId) const {
            return TypePolicy::getNumber(findNode(fieldId));
        };
        
        String getBoolean(FieldId fieldId) const {
            return TypePolicy::getBoolean(getNode(fieldId));
        };
        
        String findBoolean(FieldId fieldId) const {
            return TypePolicy::getBoolean(findNode(fieldId));
        };
        
        void getNull(FieldId fieldId) const{
            TypePolicy::getNull(getNode(fieldId));
        };
        
        bool findNull(FieldId fieldId) const{
            return TypePolicy::getNull(findNode(fieldId));
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
    public:
        
        ArrayIterator() : tree_(), iterator_(){};
        
        ArrayIterator(Tree<JSONTraits> *tree, typename std::list<Data*>::const_iterator iterator) : tree_(tree), iterator_(iterator){};
        
        Node operator*() const{
            return Node{tree_, *iterator_};
        };
        
        ArrayIterator<JSONTraits, TypePolicy> operator++(int){
            ArrayIterator<JSONTraits, TypePolicy> i{*this};
            iterator_++;
            return i;
        };
        
        ArrayIterator<JSONTraits, TypePolicy> &operator++(){
            iterator_++;
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

