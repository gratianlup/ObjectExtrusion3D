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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "Storyboard.hpp"
#include "Shape.hpp"
#include "BezierShape.h"
#include "ISerializable.hpp"
#include "Stream.hpp"
#include <cstdlib>

enum SceneState {
	SCENE_EDIT,
	SCENE_PLAY,
	SCENE_END
};

class Scene : public ISerializable {
private:
	Shape *shape_;
	Storyboard storyBoard_;
	SceneState state_;

public:
	//
	// Constructors / destructor.
	//
	Scene() : shape_(NULL), state_(SCENE_EDIT) {}

	~Scene() {
		if(shape_ != NULL) {
			delete shape_;
		}
	}

	//
	// Public methods.
	//
	Shape* ShapeObject() {
		return shape_;
	}

	void SetShape(Shape *shape) {
		if(shape_ != NULL) {
			delete shape_;
		}

		shape_ = shape;
		storyBoard_.SetShapeObject(shape);
	}

	Storyboard& Storyboard() {
		return storyBoard_;
	}

	SceneState State() { 
		return state_;
	}

	void SetState(SceneState state) {
		state_ = state;
	}

	bool Open(wchar_t *path) {
		Stream stream(path, false);
		if(stream.IsValid() == false) return false;

		Deserialize(stream);
		return true;
	}

	bool Save(wchar_t *path) {
		Stream stream(path, true);
		if(stream.IsValid() == false) return false;

		Serialize(stream);
		return true;
	}

	//
	// Serialization.
	//
	virtual void Serialize(Stream &stream) const {
		stream.Write((int)shape_->Type());
		stream.Write(*shape_);
		stream.Write(storyBoard_);
	}

	virtual void Deserialize(Stream &stream) {
		// Citeste figura.
		int temp;
		stream.Read(temp);
		ShapeType shapeType = (ShapeType)temp;

		switch(shapeType) {
			case SHAPE_BASIC: {
				shape_ = new Shape();
				stream.Read(*shape_);
				break;
			}
			case SHAPE_BEZIER: {
				shape_ = new BezierShape();
				stream.Read(*(BezierShape *)shape_);
				break;
			}
		}

		stream.Read(storyBoard_);
		storyBoard_.SetShapeObject(shape_);
	}
};

#endif
