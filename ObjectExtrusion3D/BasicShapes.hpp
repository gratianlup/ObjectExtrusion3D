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

#ifndef SHAPE_GENERATOR_HPP
#define SHAPE_GENERATOR_HPP

#define _USE_MATH_DEFINES
#include "Point.hpp"
#include "Shape.hpp"
#include <math.h>

class ShapeGenerator {
private:
	static const int DEFAULT_POINTS = 32;

public:
	static Shape* Circle(double radius, size_t points, bool onZ = true) {
		Shape* shape = new Shape();
		double step = (2 * M_PI) / (double)points;

		for(size_t i = 0; i <= points; i++) {
			double angle = i * step;

			if(onZ) {
				Point point = Point(0, 
									radius * sin(angle),
									radius * cos(angle));
				shape->Points().Add(point);
			}
			else {
				Point point = Point(radius * cos(angle), 
									radius * sin(angle), 
									0);
				shape->Points().Add(point);
			}
		}

		return shape;
	}

	static Shape* HalfCircle(double radius, size_t points, bool onZ = true) {
		Shape* shape = new Shape();
		double step = (2 * M_PI_2) / (double)points;

		for(size_t i = 0; i <= points; i++) {
			double angle = i * step;

			if(onZ) {
				Point point = Point(0, 
									radius * sin(angle),
									radius * cos(angle));
				shape->Points().Add(point);
			}
			else {
				Point point = Point(radius * cos(angle), 
									radius * sin(angle), 
									0);
				shape->Points().Add(point);
			}
		}

		return shape;
	}

	static Shape* Square(double size, size_t points, bool onZ) {
		Shape* shape = new Shape();
		points /= 4;
		double step = size / points;

		for(size_t i = 0; i < points; i++) {
			shape->Points().Add(Point(-size/2 + i * step, size / 2));
		}

		for(size_t i = 0; i < points; i++) {
			shape->Points().Add(Point(size / 2, size/2- i * step));
		}

		for(size_t i = 0; i < points; i++) {
			shape->Points().Add(Point(size/2 - i * step, -size/2));
		}

		for(size_t i = 0; i < points; i++) {
			shape->Points().Add(Point(-size/2, -size/2 + i*step));
		}

		return shape;
	}

	static Shape* Line(double size, size_t points, bool onZ) {
		Shape* shape = new Shape();
		double step = size / points;

		for(size_t i = 0; i < points; i++) {
			shape->Points().Add(Point(-size/2 + i * step, size / 2));
		}

		return shape;
	}
};

#endif
