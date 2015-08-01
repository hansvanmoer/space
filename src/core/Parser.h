/* 
 * File:   Parser.h
 * Author: hans
 *
 * Created on August 1, 2015, 2:01 PM
 */

#ifndef PARSER_H
#define	PARSER_H

namespace Core {

    template<typename Iterator_> class Range {
    public:
        using Iterator = Iterator_;
        using iterator = Iterator_;
    private:
        Iterator begin_;
        Iterator end_;

    public:

        Range() : begin_(), end_() {
        };

        Range(Iterator begin, Iterator end) : begin_(begin), end_(end) {
        }

        Iterator begin() const {
            return begin_;
        };

        Iterator end() const {
            return end_;
        };

        operator bool() const {
            return begin_ != end_;
        };

        bool operator!() const {
            return begin_ == end_;
        };

        bool operator==(const Range<Iterator> &range) const {
            return begin_ == range.begin_ && end == range.end_;
        };

        bool operator!=(const Range<Iterator> &range) const {
            return begin_ != range.begin_ || end_ != range.end_;
        };

        auto operator*() const -> decltype(*begin_) {
            return *begin_;
        };

        Range<Iterator> &operator++() {
            ++begin_;
            return *this;
        };

        Range<Iterator> operator++(int) {
            Range<Iterator> range{begin_, end_};
            ++begin_;
            return range;
        };

    };

    template<typename Range, typename Predicate, typename Code> Code parseCode(Range &range, Predicate pred, Code continueCode, Code endCode) {
        while (range) {
            Code code = pred(*range);
            if (code != continueCode) {
                return code;
            }
            ++range;
        }
        return endCode;
    };
    
    enum class ParseState {
        CONTINUE, DELIMITER, ESCAPE, ERROR, END
    };
    
    template<typename Range, typename Predicate> ParseState parseCode(Range &range, Predicate pred){
        return parseCode(range, pred, ParseState::CONTINUE, ParseState::END);
    };

    template<typename Range, typename Predicate> std::size_t parseWhile(Range &range, Predicate pred) {
        std::size_t count = 0;
        while (range && pred(*range)) {
            ++range;
            ++count;
        }
        return count;
    };

    template<typename Range, typename Predicate> std::size_t parseUntil(Range &range, Predicate pred) {
        std::size_t count = 0;
        while (range && (!pred(*range))) {
            ++range;
            ++count;
        }
        return count;
    };

}

#endif	/* PARSER_H */

