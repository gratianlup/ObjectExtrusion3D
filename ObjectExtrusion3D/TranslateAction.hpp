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
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
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

#ifndef TRANSLATE_ACTION_H
#define TRANSLATE_ACTION_H

#include "IAction.hpp"
#include "Point.hpp"
#include "List.hpp"
#include "ISerializable.hpp"
#include "Stream.hpp"

class TranslateAction: public IAction {
private:
	double deltaX_;
	double deltaY_;
	double deltaZ_;
	
public:
	//
	// Constructors.
	//
	TranslateAction() : IAction() {}

	TranslateAction(double dx, double dy, double dz) : IAction(),
		deltaX_(dx), deltaY_(dy), deltaZ_(dz) {}

	//
	// Public methods.
	//
	virtual ActionType Type() { 
		return ACTION_TRANSLATE; 
	}

	double DeltaX() { 
		return deltaX_;
	}

	void SetDeltaX(double value) {
		deltaX_ = value;
	}

	double DeltaY() { 
		return deltaY_;
	}

	void SetDeltaY(double value) {
		deltaY_ = value;
	}

	double DeltaZ() { 
		return deltaZ_;
	}

	void SetDeltaZ(double value) {
		deltaZ_ = value;
	}

	virtual void Execute(int step, List<Point> &points) {
		double dx = deltaX_ / (double)steps_;
		double dy = deltaY_ / (double)steps_;
		double dz = deltaZ_ / (double)steps_;
		size_t count = points.Count();

		for(size_t i = 0; i < count; i++) {
			Point &point = points[i];
			point.X += dx;
			point.Y += dy;
			point.Z += dz;
		}
	}

	//
	// Serialization.
	//
	virtual void Serialize(Stream &stream) const {
		stream.Write(steps_);
		stream.Write(withPrevious_);
		stream.Write(deltaX_);
		stream.Write(deltaY_);
		stream.Write(deltaZ_);
	}

	virtual void Deserialize(Stream &stream) {
		stream.Read(steps_);
		stream.Read(withPrevious_);
		stream.Read(deltaX_);
		stream.Read(deltaY_);
		stream.Read(deltaZ_);
	}
};

#endif
