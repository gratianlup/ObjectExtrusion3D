// Copyright (c) Gratian Lup. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
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

#ifndef BEZIER_SHAPE_HPP
#define BEZIER_SHAPE_HPP

#include "Point.hpp"
#include "List.hpp"
#include "ISerializable.hpp"
#include "Stream.hpp"

class BezierShape : public Shape {
private:
	static const int POINTS_PER_LINE = 10;
	static const int DEFAULT_POINTS = 32;

	List<Point> anchorPoints_;
	List<Point> controlPoints_;

public:
	//
	// Constructors / destructor.
	//
	BezierShape() {}

	BezierShape(const List<Point> anchPoints, const List<Point> ctrlPoints) :
		anchorPoints_(anchPoints), controlPoints_(ctrlPoints){}

	virtual ~BezierShape() {}

	//
	// Public methods.
	//
	virtual ShapeType Type() {
		return SHAPE_BEZIER;
	}

	List<Point>& AnchorPoints() {
		return anchorPoints_;
	}

	size_t AnchroCount() const {
		return anchorPoints_.Count();
	}

	List<Point>& ControlPoints() {
		return controlPoints_;
	}

	size_t ControlCount() const {
		return controlPoints_.Count();
	}

	virtual List<Point>& Points() {
		points_.Clear();
		if(anchorPoints_.Count() < 2) return points_;

		// Se adauga la lista AnchorCount - 1 curbe Bezier.
		for(size_t i = 0; i < anchorPoints_.Count() - 1; i++) {
			AddBezierPoints(anchorPoints_[i], anchorPoints_[i + 1],
						   controlPoints_[2*i], controlPoints_[2*i + 1]);
		}

		return points_;
	}

	virtual void Clear() {
		anchorPoints_.Clear();
		controlPoints_.Clear();
	}

	virtual Point* HitTest(double x, double y, double radius) const {
		Point *point = HitTestImpl(x, y, radius, controlPoints_);

		if(point == NULL) {
			point = HitTestImpl(x, y, radius, anchorPoints_);
		}
		
		return point;
	}

	//
	// Serialization.
	//
	virtual void Serialize(Stream &stream) const {
		stream.Write(anchorPoints_);
		stream.Write(controlPoints_);
	}

	virtual void Deserialize(Stream &stream) {
		stream.Read(anchorPoints_);
		stream.Read(controlPoints_);
	}

	void AddBezierPoints(const Point &anchor1, const Point &anchor2,
						 const Point &control1, const Point &control2) {
		double u = 0;
		double stepU = 1.0 / (POINTS_PER_LINE - 1);

		for(size_t i = 0; i < POINTS_PER_LINE; i++) {
			double x = pow(u,3)*(anchor2.X+3*(control1.X-control2.X)-anchor1.X) +
					   3*pow(u,2)*(anchor1.X-2*control1.X+control2.X) +
					   3*u*(control1.X-anchor1.X)+anchor1.X;

			double y = pow(u,3)*(anchor2.Y+3*(control1.Y-control2.Y)-anchor1.Y) +
					   3*pow(u,2)*(anchor1.Y-2*control1.Y+control2.Y) +
					   3*u*(control1.Y-anchor1.Y)+anchor1.Y;
		  
			points_.Add(Point(x, y));
			u += stepU;
		}
	}
};

#endif
