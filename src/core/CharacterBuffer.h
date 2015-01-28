/* 
 * File:   CharacterBuffer.h
 * Author: hans
 *
 * Created on 25 January 2015, 17:00
 */

#ifndef CHARACTERBUFFER_H
#define	CHARACTERBUFFER_H

#include <iostream>
#include <string>

namespace Core {

    enum BufferState {
        OK, PARTIAL_INPUT, PARTIAL_OUTPUT, ERROR
    };

    template<typename Intern, typename Extern> class CharacterInputBuffer {
    public:

        using State = BufferState;
        
        using iterator = const Intern *;

        using const_iterator = const Intern *;

        CharacterInputBuffer() : buffer_(), length_(), readLength_(), capacity_(), state_(State::OK) {
        };

        CharacterInputBuffer(CharacterInputBuffer<Intern, Extern> &&buffer) : buffer_(), length_(buffer.length_), readLength_(buffer.readLength_), capacity_(buffer.capacity_), state_(buffer.state_) {
            std::swap(buffer.buffer_, buffer_);
        };

        CharacterInputBuffer<Intern, Extern> &operator=(CharacterInputBuffer<Intern, Extern> &&buffer) {
            std::swap(buffer.buffer_, buffer_);
            length_ = buffer.length_;
            readLength_ = buffer.readLength_;
            capacity_ = buffer.capacity_;
            state_ = buffer.state_;
            return *this;
        };

        ~CharacterInputBuffer() {
            delete[] buffer_;
        };

        void clear() {
            length_ = 0;
            readLength_ = 0;
            state_ = State::OK;
        };

        void reset() {
            delete[] buffer_;
            capacity_ = 0;
            buffer_ = nullptr;
            length_ = 0;
            readLength_ = 0;
            state_ = State::OK;
        };

        void reset(std::size_t capacity) {
            delete[] buffer_;
            capacity_ = capacity;
            buffer_ = new Intern[capacity_];
            length_ = 0;
            readLength_ = 0;
            state_ = State::OK;
        };

        std::size_t capacity() const {
            return capacity_;
        };

        bool capacity(std::size_t capacity) {
            if (capacity == capacity_) {
                return false;
            } else {
                if (length_ == 0) {
                    delete[] buffer_;
                    capacity_ = capacity;
                    buffer_ = new Intern[capacity_];
                } else {
                    if (length_ > capacity) {
                        length_ = capacity;
                    }
                    Intern *nBuffer = new Intern[capacity];
                    for (std::size_t i = 0; i < length_; ++i) {
                        nBuffer[i] = buffer_[i];
                    }
                    capacity = capacity_;
                    delete[] buffer_;
                    buffer_ = nBuffer;
                }
                return true;
            }
        };

        bool ensureCapacity(std::size_t capacity) {
            if (buffer_) {
                if (capacity > capacity_) {
                    if (length_ == 0) {
                        delete[] buffer_;
                        capacity_ = capacity;
                        buffer_ = new Intern[capacity_];
                    } else {
                        Intern *nBuffer = new Intern[capacity];
                        for (std::size_t i = 0; i < length_; ++i) {
                            nBuffer[i] = buffer_[i];
                        }
                        capacity_ = capacity;
                        delete[] buffer_;
                        buffer_ = nBuffer;
                    }
                    return true;
                } else {
                    return false;
                }
            } else {
                buffer_ = new Intern[capacity];
                capacity_ = capacity;
            }
        };

        bool empty() const {
            return length_ == 0;
        };

        std::size_t length() const {
            return length_;
        };

        std::size_t readLength() const {
            return readLength_;
        };

        const_iterator begin() const {
            return buffer_;
        };

        const_iterator end() const {
            return buffer_ + length_;
        };

        State state() const {
            return state_;
        };

        State read(const Extern *fromBegin, const Extern *fromEnd) {
            std::size_t capacity = capacity_ + maxLength(fromBegin, fromEnd);
            ensureCapacity(capacity);
            Intern *toNext = buffer_ + length_;
            const Extern *fromNext = fromBegin;
            state_ = convert(fromBegin, fromEnd, fromNext, toNext, buffer_ + capacity_, toNext);
            readLength_ = static_cast<std::size_t> (fromNext - fromBegin);
            length_ += static_cast<std::size_t> (toNext - (buffer_ + length_));
            return state_;
        };

        State read(const Extern *fromBegin, std::size_t fromLength) {
            return read(fromBegin, fromBegin + fromLength);
        };

        template<typename CharTraits> State read(std::basic_istream<Extern, CharTraits> &input) {
            if (input.good()) {
                std::streampos pos = input.tellg();
                input.seekg(0, std::ios::end);
                std::size_t fromLength = static_cast<std::size_t> (input.tellg() - pos);
                input.seekg(pos);
                Extern *inputBuffer = new Extern[fromLength];
                input.read(inputBuffer, fromLength);
                read(inputBuffer, fromLength);
                if (state_ == State::PARTIAL_INPUT) {
                    std::streampos back = -static_cast<std::streampos> (fromLength - readLength_);
                    input.seekg(back, std::ios::cur);
                }
                return state_;
            } else {
                return State::OK;
            }
        };

    protected:

        virtual State convert(const Extern *fromBegin, const Extern *fromEnd, const Extern *&fromNext, Intern *toBegin, Intern *toEnd, Intern *&toNext) = 0;

        virtual std::size_t maxLength(const Extern *fromBegin, const Extern *fromEnd) = 0;

    private:
        Intern *buffer_;
        std::size_t length_;
        std::size_t readLength_;
        std::size_t capacity_;
        State state_;

        CharacterInputBuffer(const CharacterInputBuffer<Intern, Extern> &buffer) = delete;

        CharacterInputBuffer<Intern, Extern> &operator=(const CharacterInputBuffer<Intern, Extern> &buffer) = delete;
    };

    template<typename Intern, typename Extern> class UTF8ToUTF32InputBuffer : public CharacterInputBuffer<Intern, Extern> {
    private:
        
        using State = typename CharacterInputBuffer<Intern, Extern>::State;

        State nextCodePoint(const Extern *fromBegin, const Extern *fromEnd, const Extern *&fromNext, Intern &value) {
            unsigned int codePoint = static_cast<unsigned int> (*fromBegin);
            if ((codePoint & 0x80) == 0) {
                ++fromBegin;
            } else if ((codePoint & 0x40) == 0) {
                return State::ERROR;
            } else {
                unsigned int trailing;
                if ((codePoint & 0x20) == 0) {
                    codePoint &= 0x1F;
                    trailing = 1;
                } else if ((codePoint & 0x10) == 0) {
                    codePoint &= 0x0F;
                    trailing = 2;
                } else if ((codePoint & 0x08) == 0) {
                    codePoint &= 0x07;
                    trailing = 3;
                } else {
                    return State::ERROR;
                }
                ++fromBegin;
                for (unsigned int i = 0; i < trailing; ++i, ++fromBegin) {
                    if (fromBegin == fromEnd) {
                        return State::PARTIAL_INPUT;
                    } else {
                        unsigned int octet = static_cast<unsigned int> (*fromBegin);
                        if ((octet & 0xC0) == 0x80) {
                            codePoint = (codePoint << 6) | (octet & 0x7F);
                        } else {
                            return State::ERROR;
                        }
                    }
                }
            }
            fromNext = fromBegin;
            value = static_cast<Intern> (codePoint);
            return State::OK;
        };

    protected:

        State convert(const Extern *fromBegin, const Extern *fromEnd, const Extern *&fromNext, Intern *toBegin, Intern *toEnd, Intern *&toNext) {
            State state = State::OK;
            while (fromBegin != fromEnd) {
                if (toBegin == toEnd) {
                    state = State::PARTIAL_OUTPUT;
                } else {
                    state = nextCodePoint(fromBegin, fromEnd, fromBegin, *toBegin);
                }
                if (state == State::OK) {
                    ++toBegin;
                } else {
                    fromNext = fromBegin;
                    toNext = toBegin;
                    return state;
                }
            }
            fromNext = fromBegin;
            toNext = toBegin;
            return state;
        };

        std::size_t maxLength(const Extern *fromBegin, const Extern *fromEnd) {
            std::size_t count = 0;
            std::size_t trailing = 0;
            while (fromBegin != fromEnd) {
                if (trailing == 0) {
                    ++count;
                    unsigned int head = static_cast<unsigned int> (*fromBegin);
                    if ((head & 0x80) == 0) {
                        trailing = 0;
                    } else if ((head & 0x40) == 0) {
                        return count;
                    } else if ((head & 0x20) == 0) {
                        trailing = 1;
                    } else if ((head & 0x10) == 0) {
                        trailing = 2;
                    } else if ((head & 0x08) == 0) {
                        trailing = 3;
                    } else {
                        return count;
                    }
                } else {
                    --trailing;
                }
                ++fromBegin;
            }
            return count;
        };
    };
    
}

#endif	/* CHARACTERBUFFER_H */

