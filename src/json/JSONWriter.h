/* 
 * File:   JSONWriter.h
 * Author: hans
 *
 * Created on 18 January 2015, 12:01
 */

#ifndef JSONWRITER_H
#define	JSONWRITER_H

#include "JSONType.h"
#include "JSONTokens.h"

namespace JSON{
    
    template<typename JSONTraits> class WriterBase{
    public:
        using Char = typename JSONTraits::Char;
        using CharTraits = typename JSONTraits::CharTraits;
        using String = typename JSONTraits::String;
        using FieldName = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using OutputStream = std::basic_ostream<Char, CharTraits>;
    protected:
        OutputStream &output_;
        
        WriterBase(OutputStream &output) : output_(output){};
        
        WriterBase(OutputStream &&output) : output_(output){};
                
        void writeEscapedString(String value){
            output_.put(Tokens::STRING_DELIMITER);
            for(auto i = value.begin(); i != value.end(); ++i){
                int escaped = Tokens::escape(*i);
                if(escaped == 0){
                    output_.put(*i);
                }else{
                    output_.put(escaped);
                }
            }
            output_.put(Tokens::STRING_DELIMITER);
        };
        
        void writeTokens(const int *tokens, int length){
            for(int i = 0;i < length; ++i, ++tokens){
                output_.put(*tokens);
            }
        };
    };
    
    template<typename JSONTraits = BasicJSONTraits<char, std::char_traits<char>, std::allocator<char> > > class MinifiedWriter : public WriterBase<JSONTraits>{
    public:
        using Char = typename JSONTraits::Char;
        using CharTraits = typename JSONTraits::CharTraits;
        using String = typename JSONTraits::String;
        using FieldName = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using OutputStream = std::basic_ostream<Char, CharTraits>;
        using Writer = MinifiedWriter<JSONTraits>;
    private:
        bool trailing_;
        
        void writeSeparator(){
            if(trailing_){
                WriterBase<JSONTraits>::output_.put(Tokens::ELEMENT_SEPARATOR);
            }
        };
    public:
        MinifiedWriter(OutputStream &output) : WriterBase<JSONTraits>(output), trailing_(){};
        
        MinifiedWriter(OutputStream &&output) : WriterBase<JSONTraits>(output), trailing_(){};
        
        Writer &beginObject(){
            writeSeparator();
            WriterBase<JSONTraits>::output_.put(Tokens::OBJECT_BEGIN);
            trailing_=false;
            return *this;
        };
        
        Writer &endObject(){
            WriterBase<JSONTraits>::output_.put(Tokens::OBJECT_END);
            trailing_=true;
            return *this;
        };
        
        Writer &beginArray(){
            writeSeparator();
            WriterBase<JSONTraits>::output_.put(Tokens::ARRAY_BEGIN);
            trailing_=false;
            return *this;
        };
        
        Writer &endArray(){
            WriterBase<JSONTraits>::output_.put(Tokens::ARRAY_END);
            trailing_=false;
            return *this;
        };
        
        Writer &beginField(FieldName name){
            writeSeparator();
            WriterBase<JSONTraits>::writeEscapedString(name);
            WriterBase<JSONTraits>::output_.put(Tokens::KEY_VALUE_SEPARATOR);
            trailing_=false;
            return *this;
        };
        
        Writer &endField(){
            trailing_=true;
            return *this;
        };
        
        Writer &writeString(String string){
            writeSeparator();
            WriterBase<JSONTraits>::writeEscapedString(string);
            trailing_=true;
            return *this;
        };
        
        Writer &writeNumber(Number number){
            writeSeparator();
            WriterBase<JSONTraits>::output_ << number;
            trailing_=true;
            return *this;
        };
        
        Writer &writeBoolean(Boolean boolean){
            writeSeparator();
            if(boolean){
                WriterBase<JSONTraits>::writeTokens(Tokens::LITERAL_TRUE, Tokens::LITERAL_TRUE_LENGTH);
            }else{
                WriterBase<JSONTraits>::writeTokens(Tokens::LITERAL_FALSE, Tokens::LITERAL_FALSE_LENGTH);
            }
            trailing_=true;
            return *this;
        };
        
        Writer &writeNull(){
            writeSeparator();
            WriterBase<JSONTraits>::writeTokens(Tokens::LITERAL_NULL, Tokens::LITERAL_NULL_LENGTH);
            return *this;
        };
    };
    
    template<typename JSONTraits = BasicJSONTraits<char, std::char_traits<char>, std::allocator<char> > > class PrettyWriter : public WriterBase<JSONTraits>{
    public:
        using Char = typename JSONTraits::Char;
        using CharTraits = typename JSONTraits::CharTraits;
        using String = typename JSONTraits::String;
        using FieldName = typename JSONTraits::String;
        using Number = typename JSONTraits::Number;
        using Boolean = typename JSONTraits::Boolean;
        using OutputStream = std::basic_ostream<Char, CharTraits>;
        using Writer = PrettyWriter<JSONTraits>;
    private:
        bool trailing_;
        int indentation_;
        
        void writeSeparator(){
            if(trailing_){
                WriterBase<JSONTraits>::output_.put(Tokens::ELEMENT_SEPARATOR);
                newLine();
            }
        };
        
        void indent(){
            if(indentation_ > 0){
                for(int i = 0;i < indentation_; ++i){
                    WriterBase<JSONTraits>::output_.put(Tokens::TAB);
                }
            }
        }
        
        void newLine(){
            WriterBase<JSONTraits>::output_ << std::endl;
            indent();
        }
        
    public:
        PrettyWriter(OutputStream &output) : WriterBase<JSONTraits>(output), trailing_(){};
        
        PrettyWriter(OutputStream &&output) : WriterBase<JSONTraits>(output), trailing_(){};
        
        Writer &beginObject(){
            writeSeparator();
            WriterBase<JSONTraits>::output_.put(Tokens::OBJECT_BEGIN);
            trailing_=false;
            ++indentation_;
            newLine();
            return *this;
        };
        
        Writer &endObject(){
            --indentation_;
            newLine();
            WriterBase<JSONTraits>::output_.put(Tokens::OBJECT_END);
            trailing_=true;
            return *this;
        };
        
        Writer &beginArray(){
            writeSeparator();
            WriterBase<JSONTraits>::output_.put(Tokens::ARRAY_BEGIN);
            trailing_=false;
            return *this;
        };
        
        Writer &endArray(){
            WriterBase<JSONTraits>::output_.put(Tokens::ARRAY_END);
            trailing_=false;
            return *this;
        };
        
        Writer &beginField(FieldName name){
            writeSeparator();
            WriterBase<JSONTraits>::writeEscapedString(name);
            WriterBase<JSONTraits>::output_.put(Tokens::KEY_VALUE_SEPARATOR);
            trailing_=false;
            return *this;
        };
        
        Writer &endField(){
            trailing_=true;
            return *this;
        };
        
        Writer &writeString(String string){
            writeSeparator();
            WriterBase<JSONTraits>::writeEscapedString(string);
            trailing_=true;
            return *this;
        };
        
        Writer &writeNumber(Number number){
            writeSeparator();
            WriterBase<JSONTraits>::output_ << number;
            trailing_=true;
            return *this;
        };
        
        Writer &writeBoolean(Boolean boolean){
            writeSeparator();
            if(boolean){
                WriterBase<JSONTraits>::writeTokens(Tokens::LITERAL_TRUE, Tokens::LITERAL_TRUE_LENGTH);
            }else{
                WriterBase<JSONTraits>::writeTokens(Tokens::LITERAL_FALSE, Tokens::LITERAL_FALSE_LENGTH);
            }
            trailing_=true;
            return *this;
        };
        
        Writer &writeNull(){
            writeSeparator();
            WriterBase<JSONTraits>::writeTokens(Tokens::LITERAL_NULL, Tokens::LITERAL_NULL_LENGTH);
            return *this;
        };
    };
}

#endif	/* JSONWRITER_H */

