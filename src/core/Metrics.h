/* 
 * File:   Metrics.h
 * Author: hans
 *
 * Created on 29 December 2014, 16:47
 */

#ifndef METRICS_H
#define	METRICS_H

namespace Core{
    
    template<typename Scalar> struct Point{
        Scalar x;
        Scalar y;
        
        Point() : x(), y(){};
        Point(Scalar x_, Scalar y_) : x(x_), y(y_){};
    
        bool operator==(const Point<Scalar> &p) const{
            return p.x == x && p.y == y;
        };
        
        bool operator!=(const Point<Scalar> &p) const{
            return p.x != x || p.y != y;
        };
        
        template<typename Scalar2> Point<Scalar> &operator+=(const Point<Scalar2> &p){
            x+=static_cast<Scalar>(p.x);
            y+=static_cast<Scalar>(p.y);
            return *this;
        };
        
        template<typename Scalar2> Point<Scalar> &operator-=(const Point<Scalar2> &p){
            x-=static_cast<Scalar>(p.x);
            y-=static_cast<Scalar>(p.y);
            return *this;
        };
    };
    
    template<typename Scalar> Point<Scalar> operator+(const Point<Scalar> &a, const Point<Scalar> &b){
        return Point<Scalar>{a.x+b.x, a.y+b.y};
    };
    
    template<typename Scalar> Point<Scalar> operator-(const Point<Scalar> &a, const Point<Scalar> &b){
        return Point<Scalar>{a.x-b.x, a.y-b.y};
    };
    
    template<typename Scalar> struct Size{
        Scalar width;
        Scalar height;
        
        Size() : width(), height(){};
        Size(Scalar width_, Scalar height_) : width(width_), height(height_){};
        
        bool operator==(const Size<Scalar> &s) const{
            return s.width == width && s.height == height;
        };
        
        bool operator!=(const Size<Scalar> &s) const{
            return s.width != width || s.height != height;
        }
    };
    
    template<typename Scalar> struct Bounds{
        Scalar left;
        Scalar right;
        Scalar top;
        Scalar bottom;
        
        Bounds() : left(), right(), top(), bottom(){};
        Bounds(Point<Scalar> topLeft, Size<Scalar> size) : left(topLeft.x), right(topLeft.x+size.width), top(topLeft.y), bottom(topLeft.y+size.height){};
        Bounds(Scalar left_, Scalar right_, Scalar top_, Scalar bottom_) : left(left_), right(right_), top(top_), bottom(bottom_){};
    
        bool operator==(const Bounds<Scalar> b) const{
            return left == b.left && right == b.right && top == b.top && bottom == b.bottom;
        };
        
        bool operator!=(const Bounds<Scalar> b) const{
            return left != b.left || right != b.right || top != b.top || bottom != b.bottom;
        };
        
        bool contains(Scalar x, Scalar y) const{
            return x >= left && x <= right && y >= top && y <= bottom;
        };
        
        bool contains(Point<Scalar> p) const{
            return contains(p.x, p.y);
        };
    };
    
}


#endif	/* METRICS_H */

