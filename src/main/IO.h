/* 
 * File:   IO.h
 * Author: hans
 *
 * Created on 26 January 2015, 20:06
 */

#ifndef IO_H
#define	IO_H

#include "JSONReader.h"
#include "Properties.h"
#include "Path.h"

namespace Game {

    namespace IO {
        using Document = JSON::Document<JSON::BufferedInput<> >;
        using Object = typename JSON::Document<JSON::BufferedInput<> >::Object;
        using Array = typename JSON::Document<JSON::BufferedInput<> >::Array;
        using ArrayIterator = typename Array::Iterator;

        using PlainPropertyValue = Core::PropertyValue<char>;
        using UnicodePropertyValue = Core::PropertyValue<char32_t>;

        using Properties = std::map<std::string, PlainPropertyValue>;
        using UnicodeProperties = std::map<std::u32string, UnicodePropertyValue>;

        template<typename Char> class Buffer {
        private:
            Char *data_;
            std::size_t length_;
        public:
            
            using Range = Core::Range<Char*>;
            
            Buffer() : data_(), length_(){};
            
            template<typename CharTraits> Buffer(std::basic_istream<Char,CharTraits> &input) : Buffer(){
                fill(input);
            };
            
            ~Buffer(){
                delete[] data_;
            };
            
            template<typename CharTraits> void fill(std::basic_istream<Char,CharTraits> &input){
                std::streampos pos = input.tellg();
                input.seekg(0, std::ios::end);
                std::streamsize length = static_cast<std::streamsize>(input.tellg() - pos);
                input.seekg(pos);
                if(length > 0 && input.good()){
                    Char *data = new Char[length];
                    input.read(data, length);
                    delete[] data_;
                    data_ = data;
                    length_ = static_cast<std::size_t>(length);
                }
            };
            
            void clear(){
                delete[] data_;
                length_ = 0;
                data_= nullptr;
            };
            
            Char *begin() const{
                return data_;
            };
            
            Char *end() const{
                return data_+length_;
            };
            
            std::size_t length() const{
                return length_;
            };
            
            Range range() const{
                return Range{data_,data_+length_};
            };
        };

        Document open(std::istream &input);

        template<typename Char, typename CharTraits> void loadUTF8Properties(std::istream& input, std::map<std::basic_string<Char, CharTraits>, Core::PropertyValue<Char, CharTraits> > & properties) {
            static Core::PropertyLoader<Char, CharTraits> loader;
            loader.loadUTF8(input, properties);
        };

        template<typename Char, typename CharTraits> void loadUTF8Properties(Core::Path path, std::map<std::basic_string<Char, CharTraits>, Core::PropertyValue<Char, CharTraits> > & properties) {
            std::ifstream input;
            path.openFile(input);
            loadUTF8Properties(input, properties);
        };

        template<typename Char, typename CharTraits> void loadProperties(std::istream& input, std::map<std::basic_string<Char, CharTraits>, Core::PropertyValue<Char, CharTraits> > & properties) {
            static Core::PropertyLoader<Char, CharTraits> loader;
            IO::Buffer<char> buffer{input};
            loader.load(buffer.begin(), buffer.end(), properties);
        };

        template<typename Char, typename CharTraits> void loadProperties(Core::Path path, std::map<std::basic_string<Char, CharTraits>, Core::PropertyValue<Char, CharTraits> > & properties) {
            std::ifstream input;
            path.openFile(input);
            loadProperties(input, properties);
        };
    }

}

#endif	/* IO_H */

