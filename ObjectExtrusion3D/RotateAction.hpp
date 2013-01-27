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

#ifndef ROTATE_ACTION_HPP
#define ROTATE_ACTION_HPP

#include "IAction.hpp"
#include "Point.hpp"
#include "List.hpp"
#include "ISerializable.hpp"
#include "Stream.hpp"

#undef max
#undef min
#include <limits>

enum RotationOrigin {
    ROTATION_LEFT,
    ROTATION_RIGHT,
    ROTATION_TOP,
    ROTATION_BOTTOM,
    ROTATION_CENTER,
    ROTATION_ZERO
};

enum RotationAxis {
    AXIS_X,
    AXIS_Y,
    AXIS_Z
};


class RotateAction : public IAction {
    private:
    double rotation_;
    double step_;
    RotationOrigin origin_;
    RotationAxis axis_;
    Point originPoint_;

public:
    //
    // Constructors.
    //
    RotateAction() : IAction() {}

    RotateAction(double rotation, RotationOrigin origin = ROTATION_CENTER,
                 RotationAxis axis = AXIS_Z) : IAction(),
                 rotation_(rotation), origin_(origin), axis_(axis) {}

    //
    // Public methods.
    //
    virtual ActionType Type() { 
        return ACTION_ROTATE; 
    }

    RotationOrigin Center() { 
        return origin_; 
    }

    void SetCenter(RotationOrigin center) { 
        origin_ = center; 
    }

    RotationOrigin Origin() { 
        return origin_;
    }

    void SetOrigin(RotationOrigin value) {
        origin_ = value;
    }

    RotationAxis Axis() { 
        return axis_; 
    }

    void SetAxis(RotationAxis axis) { 
        axis_ = axis; 
    }

    double Rotation() {
        return rotation_;
    }

    void SetRotation(double value) {
        rotation_ = value;
    }

    virtual void Initialize(const List<Point> &points) {
        SelectOrigin(points);
        step_ = rotation_ / (double)steps_;
    }

    virtual void Execute(int step, List<Point> &points) {
        for(size_t i = 0; i < points.Count(); i++) {
            switch(axis_) {
                case AXIS_X: {
                    RotateX(points[i], originPoint_, step);
                    break;
                }
                case AXIS_Y: {
                    RotateY(points[i], originPoint_, step);
                    break;
                }
                case AXIS_Z: {
                    RotateZ(points[i], originPoint_, step);
                    break;
                }
            }
        }
    }

    //
    // Serialization.
    //
    virtual void Serialize(Stream &stream) const {
        stream.Write(steps_);
        stream.Write(withPrevious_);
        stream.Write(rotation_);
        stream.Write((int)origin_);
        stream.Write((int)axis_);
    }

    virtual void Deserialize(Stream &stream) {
        stream.Read(steps_);
        stream.Read(withPrevious_);
        stream.Read(rotation_);

        int temp;
        stream.Read(temp);
        origin_ = (RotationOrigin)temp;

        stream.Read(temp);
        axis_ = (RotationAxis)temp;
    }
    
private:
    Point& Left(const List<Point> &points) {
        double left = std::numeric_limits<double>::max();
        int pos = 0;
        
        for(size_t i = 0; i < points.Count(); i++) {
            if(points[i].X < left) {
                left = points[i].X;
                pos = i;
            }
        }
        
        return points[pos];
    }

    Point& Right(const List<Point> &points) {
        double right = -std::numeric_limits<double>::max();
        int pos = 0;

        for(size_t i = 0; i < points.Count(); i++) {
            if(points[i].X > right) {
                right = points[i].X;
                pos = i;
            }
        }

        return points[pos];
    }

    Point& Top(const List<Point> &points) {
        double top = -std::numeric_limits<double>::max();
        int pos = 0;

        for(size_t i = 0; i < points.Count(); i++) {
            if(points[i].Y > top) {
                top = points[i].Y;
                pos = i;
            }
        }

        return points[pos];
    }

    Point& Bottom(const List<Point> &points) {
        double bottom = std::numeric_limits<double>::max();
        int pos = 0;
        
        for(size_t i = 0; i < points.Count(); i++) {
            if(points[i].Y < bottom) {
                bottom = points[i].Y;
                pos = i;
            }
        }
        
        return points[pos];
    }

    Point& Front(const List<Point> &points) {
        double front = -std::numeric_limits<double>::max();
        int pos = 0;
        
        for(size_t i = 0; i < points.Count(); i++) {
            if(points[i].Z > front) {
                front = points[i].Z;
                pos = i;
            }
        }
        
        return points[pos];
    }

    Point& Back(const List<Point> &points) {
        double back = std::numeric_limits<double>::max();
        int pos = 0;
        
        for(size_t i = 0; i < points.Count(); i++) {
            if(points[i].Z < back) {
                back = points[i].Z;
                pos = i;
            }
        }
        
        return points[pos];
    }

    void SelectOrigin(const List<Point> &points) {
        switch(origin_) {
            case ROTATION_CENTER: {
                originPoint_ = Point::Centroid(points);
                break;
            }
            case ROTATION_LEFT: {
                Point &point = Left(points);
                originPoint_ = Point(point.X, (Top(points).Y + Bottom(points).Y) / 2, point.Z);	
                break;
            }
            case ROTATION_RIGHT: {
                Point &point = Right(points);
                originPoint_ = Point(point.X, (Top(points).Y + Bottom(points).Y) / 2, point.Z);	
                break;
            }
            case ROTATION_TOP: {
                Point &point = Top(points);
                originPoint_ = Point(point.X, point.Y, (Front(points).Z + Back(points).Z) / 2);	
                break;
            }
            case ROTATION_BOTTOM: {
                Point &point = Bottom(points);
                originPoint_ = Point(point.X, point.Y, (Front(points).Z + Back(points).Z) / 2);
                break;
            }
            case ROTATION_ZERO: {
                originPoint_ = Point();
            }
        }
    }

    void RotateX(Point &point, const Point &origin, int step) {
        double y = point.Y - origin.Y;
        double z = point.Z - origin.Z;
        
        point.Y = y * cos(step_) - z * sin(step_);
        point.Z = y * sin(step_) + z * cos(step_);

        point.Y += origin.Y;
        point.Z += origin.Z;
    }

    void RotateY(Point &point, const Point &origin, int step) {
        double x = point.X - origin.X;
        double z = point.Z - origin.Z;
        
        point.X = z * sin(step_) + x * cos(step_);
        point.Z = z * cos(step_) - x * sin(step_);

        point.X += origin.X;
        point.Z += origin.Z;
    }
     
    void RotateZ(Point &point, const Point &origin, int step) {
        double x = point.X - origin.X;
        double y = point.Y - origin.Y;
        
        point.X = x * cos(step_) - y * sin(step_);
        point.Y = x * sin(step_) + y * cos(step_);

        point.X += origin.X;
        point.Y += origin.Y;
    }
};

#endif
