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

#ifndef POINT_HPP
#define POINT_HPP

#include "ISerializable.hpp"
#include "Stream.hpp"
#include "List.hpp"
#include <cmath>

class Point : public ISerializable {
public:
	static const double EPSILON;

	double X;
	double Y;
	double Z;
	
	//
	// Constructors.
	//
	Point() : X(0), Y(0), Z(0) {}
	Point(double x, double y) : X(x), Y(y), Z(0) {}
	Point(double x, double y, double z) : X(x), Y(y), Z(z) {}
	Point(const Point &other) : X(other.X), Y(other.Y), Z(other.Z) {}
	
	//
	// Public methods.
	//
	double Distance(const Point &other) const {
		double dxSq = (X - other.X) * (X - other.X);
		double dySq = (Y - other.Y) * (Y - other.Y);
		double dzSq = (Z - other.Z) * (Z - other.Z);
		return sqrt(dxSq + dySq + dzSq);
	}
	
	double Magnitutde() const {
		return sqrt(X*X + Y*Y + Z*Z);
	}

	static Point Centroid(const List<Point> &points) {
		double sumX = 0;
		double sumY = 0;
		double sumZ = 0;

		if(points.Count() == 0) {
			return Point();
		}

		size_t count = points.Count();

		for(size_t i = 0; i < count; i++) {
			Point &point = points[i];
			sumX += point.X;
			sumY += point.Y;
			sumZ += point.Z;
		}

		return Point(sumX / count, sumY / count, sumZ / count);
	}

	//
	// Serialization.
	//
	virtual void Serialize(Stream &stream) const {
		// Scrie valorile.
		stream.Write(X);
		stream.Write(Y);
		stream.Write(Z);
	}

	virtual void Deserialize(Stream &stream) {
		stream.Read(X);
		stream.Read(Y);
		stream.Read(Z);
	}
	
	bool operator ==(const Point &other) const {
		if(&other == this) return true;
		
		double dx = (X - other.X);
		double dy = (Y - other.Y);
		double dz = (Z - other.Z);
		return (abs(dx) < EPSILON) &&
				(abs(dy) < EPSILON) &&
				(abs(dz) < EPSILON);
	}
	
	bool operator !=(const Point &other) const {
		return !operator ==(other); 
	}
	
	Point &operator =(const Point &other) {
		X = other.X;
		Y = other.Y;
		Z = other.Z;
		return *this;
	}
	
	Point &operator +(const Point &other) {
		X += other.X;
		Y += other.Y;
		Z += other.Z;
		return *this;
	}
	
	Point &operator -(const Point &other) {
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;
		return *this;
	}

	Point &operator *(const Point &other) {
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;
		return *this;
	}

	Point &operator *(double value) {
		X *= value;
		Y *= value;
		Z *= value;
		return *this;
	}

	Point &operator /(double value) {
		X /= value;
		Y /= value;
		Z /= value;
	}
};

const double Point::EPSILON = 0.00001;

#endif
