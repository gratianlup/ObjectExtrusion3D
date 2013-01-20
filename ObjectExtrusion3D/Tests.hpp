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

#ifndef TESTS_HPP
#define TESTS_HPP

#include "Point.hpp"
#include "List.hpp"
#include "Stream.hpp"
#include "Shape.hpp"
#include "BasicShapes.hpp"
#include "TranslateAction.hpp"
#include "ScaleAction.hpp"
#include "IAction.hpp"
#include "Storyboard.hpp"
#include <cassert>

void TestPoint() {
	Point a(1,2,3);
	assert(a.X == 1);
	assert(a.Y == 2);
	assert(a.Z == 3);

	Point b(a);
	assert(b.X == 1);
	assert(b.Y == 2);
	assert(b.Z == 3);

	Point c;
	assert(c.X == 0 & &c.Y == 0 & &c.Z == 0);
	c = a;
	assert(c.X == 1);
	assert(c.Y == 2);
	assert(c.Z == 3);

	a = a * 2.0;
	assert(a.X == 2);
	assert(a.Y == 4);
	assert(a.Z == 6);

	assert(c == b);
	assert(a != c);
}

void TestList() {
	List<int> a;
	a.Add(3);
	a.Add(5);
	assert(a.Count() == 2);
	assert(a.Contains(3));
	assert(a.Contains(5));
	assert(a[0] == 3);

	List<int> b(a);
	b.Add(a);
	assert(b.Count() == 4);
	a.Remove(3);
	assert(a.Count() == 1);
	assert(a.Contains(3) == false);
	assert(a[0] = 5);
	
	a.Insert(4, 0);
	a.Insert(6, 1);
	a.Insert(7, a.Count() - 1);
	assert(a[0] == 4);
	assert(a[1] == 6);
	assert(a[2] == 7);

	a.Remove(6);
	assert(a[2] == 5);
}

void TestSerialization() {
	Stream stream(L"test.dat", true);
	List<int> l;
	l.Add(1);
	l.Add(2);
	l.Add(3);
	l.Serialize(stream);
	l.Clear();
	stream.Close();

	stream.Open(L"test.dat");
	l.Deserialize(stream);

	List<Point> b;
	b.Add(Point(1, 2, 3));
	b.Add(Point(4, 5, 6));
	b.Add(Point(7, 8, 9));
	stream.Close();
	stream.Open(L"test.dat", true);
	b.Serialize(stream);
	
	stream.Close();

	stream.Open(L"test.dat");
	b.Deserialize(stream);
}

void TestStoryboard() {
	List<Point> points;
	points.Add(Point(0, 0, 0));
	points.Add(Point(1, 1, 1));
	points.Add(Point(2, 2, 2));
	points.Add(Point(3, 3, 3));

	Shape shape(points);
	IAction* a = new TranslateAction(3, 3 , 3);
	IAction* b = new TranslateAction(2, 2 , 2);
	IAction* c = new TranslateAction(5, 5 , 5);
	a->SetSteps(3);
	b->SetSteps(3);
	c->SetSteps(4);
	b->SetWithPrevious(true);

	Storyboard sb;
	sb.Actions().Add(a);
	sb.Actions().Add(b);
	sb.Actions().Add(c);
	sb.SetShapeObject(shape);

	assert(sb.TotalSteps() == 10);
	assert(sb.CurrentAction() == NULL);

	sb.Play();
	for(size_t i = 0; i < sb.TotalSteps(); i++) {
		sb.NextStep();
	}
}

#endif
