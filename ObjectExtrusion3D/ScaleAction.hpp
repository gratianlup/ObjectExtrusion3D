// Copyright (c) Gratian Lup. All rights reserved.
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

#ifndef SCALE_ACTION_HPP
#define SCALE_ACTION_HPP

#define _USE_MATH_DEFINES
#include "IAction.hpp"
#include "Point.hpp"
#include "List.hpp"
#include "ISerializable.hpp"
#include "Stream.hpp"
#include <math.h>
#undef max
#undef min
#include <limits>

class ScaleAction: public IAction {
private:
    double scaleX_;
    double scaleY_;
    double scaleZ_;
    double stepX_;
    double stepY_;
    double stepZ_;
    Point centroid_; // Shape center point.

public:
    //
    // Constructors.
    //
    ScaleAction() : IAction() {}

    ScaleAction(double sx, double sy, double sz) : IAction(),
        scaleX_(sx), scaleY_(sy), scaleZ_(sz) {}

    //
    // Public methods.
    //
    virtual ActionType Type() { 
        return ACTION_SCALE; 
    }

    double ScaleX() {
        return scaleX_;
    }

    void SetScaleX(double value) {
        scaleX_ = value;
    }

    double ScaleY() {
        return scaleY_;
    }

    void SetScaleY(double value) {
        scaleY_ = value;
    }

    double ScaleZ() {
        return scaleZ_;
    }

    void SetScaleZ(double value) {
        scaleZ_ = value;
    }

    virtual void Initialize(const List<Point> &points) {
        stepX_ = scaleX_ / (double)steps_;
        stepY_ = scaleY_ / (double)steps_;
        stepZ_ = scaleZ_ / (double)steps_;
    }

    virtual void Execute(int step, List<Point> &points) {
        centroid_ = Point::Centroid(points);
        double minDistance = std::numeric_limits<double>::max();

        for(size_t i = 0; i < points.Count(); i++) {
            double distance = points[i].Distance(centroid_);
            if(distance < minDistance) {
                minDistance = distance;
            }
        }

        for(size_t i = 0; i < points.Count(); i++) {
            Point &point = points[i];
            double distance = point.Distance(centroid_);

            // Compute the angle between the point and the center on each axis.
            double angle1 = atan2(point.Y - centroid_.Y,
                                  point.X - centroid_.X);
            double angle2 = acos((point.Z - centroid_.Z) / distance);
            double scale = distance / minDistance;

            point.X = centroid_.X + ((distance + stepX_ * scale) * cos(angle1) * sin(angle2));
            point.Y = centroid_.Y + ((distance + stepY_ * scale) * sin(angle1) * sin(angle2));
            point.Z = centroid_.Z + ((distance + stepZ_ * scale) * cos(angle2));
        }
    }

    //
    // Serialization.
    //
    virtual void Serialize(Stream &stream) const {
        stream.Write(steps_);
        stream.Write(withPrevious_);
        stream.Write(scaleX_);
        stream.Write(scaleY_);
        stream.Write(scaleZ_);
    }

    virtual void Deserialize(Stream &stream) {
        stream.Read(steps_);
        stream.Read(withPrevious_);
        stream.Read(scaleX_);
        stream.Read(scaleY_);
        stream.Read(scaleZ_);
    }
};

#endif
