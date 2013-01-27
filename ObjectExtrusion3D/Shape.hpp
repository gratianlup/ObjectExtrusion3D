// Copyright (c) 2010 Gratian Lup. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials provided
// with the distribution.
//
// * The name "ObjectExtrusion3D" must not be used to endorse or promote
// products derived from this software without prior written permission.
//
// * Products derived from this software may not be called "ObjectExtrusion3D" nor
// may "ObjectExtrusion3D" appear in their names without prior written
// permission of the author.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "Point.hpp"
#include "List.hpp"
#include "ISerializable.hpp"
#include "Stream.hpp"
#include <cmath>

enum  ShapeType {
    SHAPE_BASIC,
    SHAPE_BEZIER
};

class Shape : public ISerializable {
protected:
    List<Point> points_;

public:
    //
    // Constructors.
    //
    Shape() {}

    Shape(const List<Point> &points) : points_(points) {}

    virtual ~Shape() {}

    //
    // Public methods.
    //
    virtual ShapeType Type() { 
        return SHAPE_BASIC; 
    }

    virtual List<Point>& Points() {
        return points_; 
    }

    virtual int PointCount() const { 
        return points_.Count(); 
    }

    virtual Point* HitTest(double x, double y, double radius) const {
        return HitTestImpl(x, y, radius, points_);
    }

    virtual void Clear() {
        points_.Clear();
    }

    //
    // Serialization.
    //
    virtual void Serialize(Stream &stream) const {
        stream.Write(points_);
    }

    virtual void Deserialize(Stream &stream) {
        stream.Read(points_);
    }

protected:
    virtual Point* HitTestImpl(double x, double y, double radius,
                               const List<Point> &points) const {
        for(size_t i = 0; i < points.Count(); i++) {
            double dx = points[i].X - x;
            double dy = points[i].Y - y;

            if((abs(dx) <= radius) && (abs(dy) <= radius)) {
                return &points[i];
            }
        }

        return NULL;
    }
};

#endif
