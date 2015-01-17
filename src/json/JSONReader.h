/* 
 * File:   JSONReader.h
 * Author: hans
 *
 * Created on 15 January 2015, 19:10
 */

#ifndef JSON_READER_H
#define	JSON_READER_H

#include "JSONTreeBuilder.h"
#include "JSONNode.h"
#include "JSONParser.h"
#include "JSONTypePolicy.h"

#include <iostream>
#include <string>

namespace JSON{
    
    class JSONReaderException : public JSONException{
    private:
        int line_;
        int column_;
    public:
        
        static const int NO_LINE;
        
        static const int NO_COLUMN;
        
        JSONReaderException(std::string message);
        
        JSONReaderException(std::string message, int line, int column);
        
        int line() const;
        
        int column() const;
    };
    
    template<typename Char> class BufferedRange{
    private:
        Char *begin_;
        Char *end_;
    public:
        
        using Iterator = Char *;
        
        BufferedRange() : begin_(), end_(){};
        
        BufferedRange(Char *begin, Char *end) : begin_(begin), end_(end){};
        
        Iterator begin() const{
            return begin_;
        };
        
        Iterator end() const{
            return end_;
        };
        
        Char &operator*(){
            return *begin_;
        };
        
        Char operator*() const{
            return *begin_;
        };
        
        bool operator!() const{
            return begin_ == end_;
        };
        
        operator bool() const{
            return begin_ != end_;
        };
        
        BufferedRange<Char> &operator++(){
            ++begin_;
            return *this;
        };
        
        BufferedRange<Char> operator++(int){
            BufferedRange<Char> range{begin_, end_};
            ++begin_;
            return range;
        };
        
        bool operator==(const BufferedRange<Char> &range) const{
            return begin_ == range.begin_;
        };
        
        bool operator!=(const BufferedRange<Char> &range) const{
            return begin_ != range.begin_;
        };
    };
    
    template<typename JSONTraits = BasicJSONTraits<char,std::char_traits<char>,std::allocator<char> > > class BufferedInput{
    public:
        using Char = typename JSONTraits::Char;
        using CharTraits = typename JSONTraits::CharTraits;
        using InputStream = std::basic_istream<Char, CharTraits>;
        using Range = BufferedRange<Char>;
    private:
        Char *buffer_;
        std::streamsize length_;
        
        void fill(InputStream &input){
            std::streampos begin = input.tellg();
            input.seekg(0, std::ios_base::end);
            std::streampos end = input.tellg();
            input.seekg(0, std::ios_base::beg);
            length_ = end - begin;
            buffer_ = new Char[length_];
            input.read(buffer_, length_);
        };
        
        BufferedInput(const BufferedInput<JSONTraits> &) = delete;
        BufferedInput<JSONTraits> &operator==(const BufferedInput<JSONTraits> &) = delete;
    public:    

        BufferedInput(InputStream &input){
            fill(input);
        };
        
        BufferedInput(InputStream &&input){
            fill(input);
        };
        
        BufferedInput(BufferedInput<JSONTraits> &&input) : buffer_(input.buffer_), length_(input.length_){
            input.buffer_ = nullptr;
        };
        
        BufferedInput<JSONTraits> &operator=(BufferedInput<JSONTraits> &&input){
            std::swap(buffer_, input.buffer_);
            length_ = input.length_;
            return *this;
        };
        
        ~BufferedInput(){
            delete[] buffer_;
        };
        
        Range data() const{
            return Range{buffer_, buffer_+length_};
        };
    };
    
    template<
        typename Input, 
        typename JSONTraits = BasicJSONTraits<char, std::char_traits<char>, std::allocator<char> >,
        typename TypePolicy = StrictTypePolicy<JSONTraits>, 
        typename TreeBuilder = BasicTreeBuilder<JSONTraits> 
    > class Document{
    private:
        
        using Range = typename Input::Range;
        
        Tree<JSONTraits> *tree_;
        Input &input_;
        
        JSONReaderException createException(Range begin, Range errorLocation, std::string message){
            int line = 0;
            int column = 0;
            Range range{begin.begin(), errorLocation.begin()};
            while(range){
                int c = JSONTraits::CharTraits::to_int_type(*range);
                switch(c){
                    case 0x0A:
                    case 0x0D:
                        ++line;
                        column=0;
                        break;
                    default:
                        ++column;
                }
                ++range;
            }
            return JSONReaderException{message, line, column};
        };
        
        void parse(){
            try{
                tree_ = new Tree<JSONTraits>();
                TreeBuilder builder_(tree_);
                Range data = input_.data();
                Parser<JSONTraits, Range, TreeBuilder> parser(data, builder_);
                parser.parse();
                if(!builder_.valid()){
                    throw createException(data, parser.current(), builder_.errorMessage());
                }
            }catch(JSONException &e){
                delete tree_;
                throw JSONReaderException(e.what());
            }catch(...){
                delete tree_;
                throw;
            }
        };
        
    public:
        
        using Boolean = typename JSONTraits::Boolean;
        using Number = typename JSONTraits::Number;
        using String = typename JSONTraits::String;
        using Node = TreeNode<JSONTraits, TypePolicy>;
        using Object = ObjectNode<JSONTraits, TypePolicy>;
        using Array = ArrayNode<JSONTraits, TypePolicy>;
        
        Document(Input &input) : tree_(new Tree<JSONTraits>()), input_(input){
            parse();
        };
        
        Document(Input &&input) : tree_(new Tree<JSONTraits>()), input_(input){
            parse();
        };
        
        Node rootNode() const{
            return Node{tree_, tree_->rootNode()};
        };
        
        ~Document(){
            delete tree_;
        };
    };
    
    
}

#endif	/* JSONREADER_H */

