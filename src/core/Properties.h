/* 
 * File:   Properties.h
 * Author: hans
 *
 * Created on August 1, 2015, 1:17 PM
 */

#ifndef PROPERTIES_H
#define	PROPERTIES_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <locale>
#include <stdexcept>
#include <type_traits>

#include "Parser.h"
#include "CharacterBuffer.h"
#include "Path.h"

namespace Core {

    class PropertyException : public std::runtime_error {
    public:
        PropertyException(std::string message);
    };

    class PropertySyntaxException : public PropertyException {
    public:
        PropertySyntaxException(std::string message, int line);

        int line() const;

    private:
        int line_;
    };

    class ConvertPropertyException : public PropertyException {
    public:
        ConvertPropertyException(std::string message);
    };

    template<typename Char, typename CharTraits, typename Output> class BasicNumericConverter {
        static_assert(std::is_integral<Output>::value || std::is_floating_point<Output>::value, "can only be used to convert integral types");
    protected:

        static void convert(const std::basic_string<Char, CharTraits> &input, Output &output) {
            std::string buffer{input.begin(), input.end()};
            std::ostringstream converter{buffer};
            converter >> output;
            if (converter.bad()) {
                throw ConvertPropertyException("unable to convert value");
            }
        };
    };

    template<typename Char, typename CharTraits, typename Output> class BasicConverterDelegate {
    public:

        static void convert(const std::basic_string<Char, CharTraits> &input, Output &output) {
            std::basic_ostringstream<Char, CharTraits> buffer{input};
            buffer >> output;
            if (buffer.bad() || !buffer.eof()) {
                throw ConvertPropertyException("unable to convert value");
            }
        };
    };

    template<typename Char, typename CharTraits, typename Output> class BasicConverter {
    public:

        static void convert(const std::basic_string<Char, CharTraits> &input, Output &output) {
            typename std::conditional <
                    (!(std::is_same<char, Char>::value && std::is_same<std::char_traits<char>, CharTraits>::value)) ||
                    std::is_integral<Output>::value ||
                    std::is_floating_point<Output>::value,
                    BasicNumericConverter<Char, CharTraits, Output>,
                    BasicConverterDelegate<Char, CharTraits, Output>
                    >::convert(input, output);
        };

    };

    template<typename Char, typename CharTraits> class BasicConverter<Char, CharTraits, std::basic_string<Char, CharTraits>>
    {
        public:

        static void convert(const std::basic_string<Char, CharTraits> &input, std::basic_string<Char, CharTraits> &output) {
            output = input;
        };

    };

    template<typename Char, typename CharTraits, typename Element> class BasicConverter<Char, CharTraits, std::list<Element>>
    {
        public:

        using String = std::basic_string<Char, CharTraits>;

        using Iterator = typename String::const_iterator;

        static ParseState parse(Char c) {
            if (CharTraits::to_int_type(c) == ',') {
                return ParseState::DELIMITER;
            } else {
                return ParseState::CONTINUE;
            }
        };

        static void convert(const std::basic_string<Char, CharTraits> &input, std::list<Element> &output) {
            ParseState state = ParseState::CONTINUE;
            Range<Iterator> range{input.begin(), input.end()};
            Iterator begin = range.begin();
            while (state == ParseState::CONTINUE) {
                state = Core::parseCode(range, BasicConverter<Char, CharTraits, std::list < Element>>::parse);
                Element element;
                BasicConverter<Char, CharTraits, Element>::convert(String{begin, range.begin()}, element);
                output.push_back(element);
                if (state == ParseState::DELIMITER) {
                    state = ParseState::CONTINUE;
                }
            }
        };

    };

    template<typename Char, typename CharTraits> class UnicodeFormat {
    private:
        const std::locale &locale_;

        static std::map<int, int> createEscapeMap() {
            return
            {
                {
                    0x6E, 0x0A
                },
                {
                    0x74, 0x0B
                }
            };
        };

        static std::map<int, int> &escapeMap() {
            static std::map<int, int> map{createEscapeMap()};
            return map;
        }
    public:

        UnicodeFormat(const std::locale &locale) : locale_(locale) {
        };

        UnicodeFormat() : UnicodeFormat(std::locale::classic()) {
        };

        bool isNewLine(Char c) const {
            return CharTraits::eq(c, '\n');
        };

        bool isWhitespace(Char c) const {
            return std::isspace<Char>(c, locale_);
        };

        bool isKeyValueDelimiter(Char c) const {
            return CharTraits::eq(c, '=') || CharTraits::eq(c, ':');
        };

        bool isEscapeFlag(Char c) const {
            return CharTraits::eq(c, '\\');
        };

        bool isComment(Char c) const {
            return CharTraits::eq(c, '#');
        };

        Char unescape(Char c) const {
            auto found = escapeMap().find(CharTraits::to_int_type(c));
            if (found == escapeMap().end()) {
                return c;
            } else {
                return CharTraits::to_char_type(found->second);
            }
        };

        const std::locale &locale() const {
            return locale_;
        };

    };

    template<typename Char, typename CharTraits, typename Format> class PropertyParser {
    public:
        using Key = std::basic_string<Char, CharTraits>;
        using Value = Key;
    private:
        Format format_;

        using OutputBuffer = std::basic_stringstream<Char, CharTraits>;

        using ParseState = Core::ParseState;

        template<typename Iterator> class Add {
        public:

            static void add(OutputBuffer buffer, Iterator begin, Iterator end) {
                while (begin != end) {
                    buffer.put(*begin);
                    ++begin;
                }
            };
        };

        template<typename T> class Add<T *> {
        public:

            static void add(OutputBuffer &buffer, T *begin, T *end) {
                std::size_t length = static_cast<std::size_t> (end - begin);
                buffer.write(begin, length);
            };
        };

        template<typename Iterator> void add(OutputBuffer &buffer, Iterator begin, Iterator end) {
            Add<Iterator>::add(buffer, begin, end);
        };

        template<typename Range> Char parseEscape(Range &range) {
            ++range;
            if (range) {
                Char c = format_.unescape(*range);
                ++range;
                return c;
            } else {
                throw PropertyException("unexpected end of input, expected escape code");
            }
        };

        template<typename Range> void parseKey(OutputBuffer &buffer, Range &range) {
            auto keyPred = [&](Char c) -> ParseState {
                if (format_.isKeyValueDelimiter(c) || format_.isWhitespace(c)) {
                    return ParseState::DELIMITER;
                } else if (format_.isEscapeFlag(c)) {
                    return ParseState::ESCAPE;
                } else if (format_.isNewLine(c) || format_.isComment(c)) {
                    return ParseState::ERROR;
                } else {
                    return ParseState::CONTINUE;
                }
            };
            typedef typename Range::Iterator Iterator;
            Iterator begin = range.begin();
            ParseState state{ParseState::CONTINUE};
            do {
                state = Core::parseCode(range, keyPred);
                switch (state) {
                    case ParseState::ESCAPE:
                        add(buffer, begin, range.begin());
                        buffer.put(parseEscape(range));
                        begin = range.begin();
                        break;
                    case ParseState::DELIMITER:
                        add(buffer, begin, range.begin());
                        ++range;
                        break;
                    case ParseState::ERROR:
                        throw PropertyException("unexpected character");
                    case ParseState::END:
                        throw PropertyException("unexpected end of input");
                }
            } while (state == ParseState::ESCAPE);
        };

        template<typename Range> std::size_t skipWhitespace(Range &range) {
            auto whitespacePred = [&](Char c) -> bool {
                return format_.isWhitespace(c) && !format_.isNewLine(c);
            };
            return Core::parseWhile(range, whitespacePred);
        };

        template<typename Range> void skipKeyValueDelimiter(Range &range) {
            skipWhitespace(range);
            if (range) {
                if (format_.isKeyValueDelimiter(*range) || format_.isWhitespace(*range)) {
                    ++range;
                }
            } else {
                throw PropertyException("unexpected end of input");
            }
        };

        template<typename Range> void parseValue(OutputBuffer &buffer, Range &range) {
            typedef typename Range::Iterator Iterator;
            auto valuePred = [&](Char c) -> ParseState {
                if (format_.isEscapeFlag(c)) {
                    return ParseState::ESCAPE;
                } else if (format_.isNewLine(c) || format_.isComment(c)) {
                    return ParseState::DELIMITER;
                } else {
                    return ParseState::CONTINUE;
                }
            };
            ParseState state{ParseState::CONTINUE};
            Iterator begin = range.begin();
            do {
                state = Core::parseCode(range, valuePred);
                switch (state) {
                    case ParseState::ESCAPE:
                        add(buffer, begin, range.begin());
                        buffer.put(parseEscape(range));
                        begin = range.begin();
                        break;
                    case ParseState::DELIMITER:
                        add(buffer, begin, range.begin());
                        return;
                    case ParseState::END:
                        add(buffer, begin, range.begin());
                }
            } while (state == ParseState::ESCAPE);
        };

        template<typename Range> void skipToEndOfLine(Range &range) {
            auto linePred = [&](Char c) -> bool {
                return format_.isNewLine(c);
            };
            Core::parseUntil(range, linePred);
            if (range) {
                ++range;
            }
        };

        template<typename Range, typename ParserListener> void parseLine(Range &range, ParserListener &listener) {
            auto commentPred = [&](Char c) ->bool {
                return !format_.isNewLine(c);
            };
            skipWhitespace(range);
            if (range) {
                if (format_.isComment(*range)) {
                    Core::parseWhile(range, commentPred);
                } else if (format_.isNewLine(*range)) {
                    ++range;
                } else {
                    OutputBuffer keyBuffer;
                    parseKey(keyBuffer, range);
                    skipKeyValueDelimiter(range);
                    OutputBuffer valueBuffer;
                    parseValue(valueBuffer, range);
                    skipToEndOfLine(range);
                    listener.addProperty(keyBuffer.str(), valueBuffer.str());
                }
            }
        };

    public:

        PropertyParser(Format format) : format_(format) {
        };

        PropertyParser() : format_() {
        };

        const Format &format() const {
            return format_;
        };

        template<typename Range, typename ParserListener> void parse(Range range, ParserListener &listener) {
            int line = 0;
            try {
                while (range) {
                    parseLine(range, listener);
                    ++line;
                }
            } catch (PropertyException &e) {
                listener.error(std::string{e.what()}, line);
            }
        };

    };

    template<
    typename ExternChar = char,
    typename ExternCharTraits = std::char_traits<ExternChar>,
    typename InternChar = ExternChar,
    typename InternCharTraits = std::char_traits<InternChar>,
    typename Format = UnicodeFormat<ExternChar, ExternCharTraits>
    > class PropertyLoader {
    public:

        using Key = std::basic_string<InternChar, InternCharTraits>;
        using Value = Key;
        using ExternString = std::basic_string<ExternChar, ExternCharTraits>;
        using Parser = PropertyParser<ExternChar, ExternCharTraits, Format>;

    private:
        Parser parser_;

        template<typename Map> class PropertyMapBuilder {
        private:
            Map &map_;
        public:

            PropertyMapBuilder(Map &map) : map_(map) {
            };

            void error(std::string message, int line) {
                throw PropertySyntaxException{message, line};
            };

            void addProperty(const ExternString &key, const ExternString &value) {
                map_[Key{key.begin(), key.end()}
                ] = Value{value.begin(), value.end()};
            };

        };
    public:

        PropertyLoader(Format format) : parser_(format) {
        };

        PropertyLoader() : parser_() {
        };

        template<typename Iterator, typename Map> void load(Iterator begin, Iterator end, Map &map) {
            PropertyMapBuilder<Map> builder{map};
            parser_.parse(Core::Range<Iterator>{begin, end}, builder);
        };

        template<typename Map> void loadUTF8(std::istream &input, Map &map) {
            Core::UTF8ToUTF32InputBuffer<ExternChar, char> buffer;
            buffer.read(input);
            load(buffer.begin(), buffer.end(), map);
        };

        template<typename Map> void loadUTF8(Core::Path &path, Map &map) {
            std::ifstream input;
            path.openFile(input);
            loadUTF8(input, map);
        };
    };

    class PropertyNotFoundException : public PropertyException {
    public:
        PropertyNotFoundException(std::string message);
    };

    template<typename Char, typename CharTraits> class DefaultPropertyConverterPolicy {
    public:

        template<typename Output> static void execute(const std::basic_string<Char, CharTraits> &input, Output &output) {
            BasicConverter<Char, CharTraits, Output>::convert(input, output);
        };

    };

    template<typename Char = char, typename CharTraits = std::char_traits<Char>, typename ConverterPolicy = DefaultPropertyConverterPolicy<Char, CharTraits> > class PropertyValue {
    public:
        using String = std::basic_string<Char, CharTraits>;
    private:
        String value_;
    public:

        PropertyValue() : value_() {
        };

        PropertyValue(String value) : value_(value) {
        };

        operator String() const {
            return value_;
        };

        String get() const {
            return value_;
        };

        void write(std::basic_ostream<Char, CharTraits> &output) const {
            output << value_;
        };

        template<typename Output> void get(Output &output) const {
            ConverterPolicy::execute(value_, output);
        };

    };

}

template<typename Char, typename CharTraits, typename ConverterPolicy> std::basic_ostream<Char, CharTraits> &operator<<(std::basic_ostream<Char, CharTraits> &output, Core::PropertyValue<Char, CharTraits, ConverterPolicy> value) {
    value.write(output);
    return output;
};


#endif	/* PROPERTIES_H */

