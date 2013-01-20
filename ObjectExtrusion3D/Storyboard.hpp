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

#ifndef STORYBOARD_HPP
#define STORYBOARD_HPP

#include "List.hpp"
#include "IAction.hpp"
#include "Shape.hpp"
#include "ISerializable.hpp"
#include "Stream.hpp"
#include "TranslateAction.hpp"
#include "ScaleAction.hpp"
#include "RotateAction.h"

class Storyboard {
private:
	typedef List<Point> PointList;

	List<IAction *> actions_;
	IAction* currentAction_;
	size_t currentPosition_;
	int currentStep_;
	List<PointList *> points_;
	Shape* shape_;

public:
	//
	// Constructors / destructor.
	//
	Storyboard() {
		Reset();
	}

	~Storyboard() {
		ClearActions();

	}

	//
	// Public methods.
	//
	List<IAction*> &Actions() { 
		return actions_; 
	}

	size_t ActionCount() { 
		return actions_.Count(); 
	}

	void ClearActions() 
	{
		for(size_t i = 0; i < actions_.Count(); i++) {
			delete actions_[i];
		}

		actions_.Clear();
	}

	Shape* ShapeObject() { 
		return shape_; 
	}

	void SetShapeObject(Shape *value) {
		shape_ = value; 
	}

	int TotalSteps() {
		int sum = 0;
		
		for(size_t i = 0; i < actions_.Count(); i++) {
			sum += actions_[i]->Steps();
		}

		return sum;
	}

	IAction* CurrentAction() { 
		return currentAction_; 
	}

	List<PointList*>& Points() { 
		return points_; 
	}

	void Play() {
		if(actions_.Count() == 0) return;

		currentAction_ = actions_[0];
		currentPosition_ = 0;
		currentStep_ = 0;

		List<Point> *firstPoints = new List<Point>(shape_->Points());
		points_.Add(firstPoints);

		// Initialize the start action and the ones connected to it.
		currentAction_->Initialize(*firstPoints);

		for(size_t i = 1; i < actions_.Count(); i++) {
			if(actions_[i]->WithPrevious() == false) return;
			actions_[i]->Initialize(*firstPoints);
		}
	}

	bool NextStep() {
		if(actions_.Count() == 0) return false;
		List<Point>* prevPoints = points_[points_.Count() - 1];

		// Check if the next action should be executed.
		if(currentStep_ == currentAction_->Steps()) {
			// Skip over all connected actions.
			size_t nextPosition = currentPosition_ + 1;

			while(nextPosition < actions_.Count() && actions_[nextPosition]->WithPrevious()) {
				nextPosition++;
			}

			size_t i = nextPosition + 1;
			while((i < actions_.Count()) && actions_[i]->WithPrevious()) {
				actions_[i]->Initialize(*prevPoints);
				i++;
			}

			if(nextPosition < actions_.Count()) {
				currentAction_ = actions_[nextPosition];
				currentAction_->Initialize(*prevPoints);
				currentPosition_ = nextPosition;
				currentStep_ = 0;
			}
			else {
				// All actions have been executed.
				return false; 
			}
		}

		// Compute the next state of the shape.
		// The generated points depend directly on the previous ones.
		List<Point>* newPoints = new List<Point>(*prevPoints);
		points_.Add(newPoints);

		// Apply to the points the current action and all actions liked with it.
		currentAction_->Execute(currentStep_, *newPoints);

		for(size_t i = currentPosition_ + 1; i < actions_.Count(); i++) {
			if(actions_[i]->WithPrevious()) {
				actions_[i]->Execute(currentStep_, *newPoints);
			}
			else break;
		}

		currentStep_++;
		return true;
	}

	void Reset() {
		currentAction_ = NULL;
		currentPosition_ = 0;
		currentStep_ = 0;

		// Remove all computed points in the current step.
		for(size_t i = 0; i < points_.Count(); i++) {
			delete points_[i];
		}

		points_.Clear();
	}

	//
	// Serialization.
	//
	virtual void Serialize(Stream &stream) const {
		// Only the actions are saved.
		stream.Write(actions_.Count());

		for(size_t i = 0; i < actions_.Count(); i++) {
			stream.Write((int)actions_[i]->Type());
			stream.Write(*actions_[i]);
		}
	}

	virtual void Deserialize(Stream &stream) {
		ClearActions();
		size_t count;
		stream.Read(count);

		for(size_t i = 0; i < count; i++) {
			int temp;
			stream.Read(temp);
			ActionType type = (ActionType)temp;

			switch(type) {
				case ACTION_TRANSLATE: {
					TranslateAction *action = new TranslateAction();
					stream.Read(*action);
					actions_.Add(action);
					break;
				}
				case ACTION_SCALE: {
					ScaleAction *action = new ScaleAction();
					stream.Read(*action);
					actions_.Add(action);
					break;
				}
				case ACTION_ROTATE: {
					RotateAction *action = new RotateAction();
					stream.Read(*action);
					actions_.Add(action);
					break;
				}
			}
		}
	}
};

#endif
