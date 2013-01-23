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

#include "Point.hpp"
#include "List.hpp"
#include "Stream.hpp"
#include "Shape.hpp"
#include "BasicShapes.hpp"
#include "TranslateAction.hpp"
#include "ScaleAction.hpp"
#include "IAction.hpp"
#include "Storyboard.hpp"
#include "BasicShapes.hpp"
#include "RotateAction.hpp"
#include "Scene.hpp"
#include <glui.h>

#define NOMINMAX
#include <Windows.h>
#include <ShObjIdl.h>
#include <gl/gl.h>
#include <gl/GLU.h>
#include <glaux.h>

//
// Id-uri pentru controale.
//
static const int AXIS_ID = 1;
static const int PLAY_ID = 2;
static const int RESET_ID = 3;
static const int WIREFRAME_ID = 4;
static const int SHAPELIST_ID = 5;
static const int SHAPELIST_EMPTY_ID = 1000;
static const int SHAPELIST_EMPTY_BEZIER_ID = 1001;
static const int SHAPELIST_CIRCLE_ID = 1002;
static const int SHAPELIST_ARC_ID = 1003;
static const int SHAPELIST_SQUARE_ID = 1004;
static const int SHAPELIST_LINE_ID = 1005;
static const int LOAD_SHAPE_ID = 11;
static const int RESET_SHAPE_ID = 12;
static const int ACTION_ROTATE_ID = 13;
static const int ACTION_TRANSLATE_ID = 14;
static const int ACTION_SCALE_ID = 15;
static const int ACTION_ID = 2000;
static const int AXIS_X_ID = 16;
static const int AXIS_Y_ID = 17;
static const int AXIS_Z_ID = 18;
static const int ORIGIN_LEFT_ID = 19;
static const int ORIGIN_RIGHT_ID = 20;
static const int ORIGIN_TOP_ID = 21;
static const int ORIGIN_BOTTOM_ID = 22;
static const int ORIGIN_CENTER_ID = 23;
static const int ORIGIN_ZERO_ID = 24;
static const int TRANSLATE_X_ID = 25;
static const int TRANSLATE_Y_ID = 26;
static const int TRANSLATE_Z_ID = 27;
static const int SCALE_X_ID = 28;
static const int SCALE_Y_ID = 29;
static const int SCALE_Z_ID = 30;
static const int STEP_ID = 31;
static const int PREVIOUS_ID = 32;
static const int ROTATION_AXIS_ID = 33;
static const int ROTATION_ORIGIN_ID = 34;
static const int ROTATION_AMOUNT_ID = 35;
static const int ACTION_SELECTED_ID = 36;
static const int ADD_ACTION_ID = 37;
static const int EDIT_ACTION_ID = 38;
static const int REMOVE_ACTION_ID = 39;
static const int UPDATE_ACTION_ID = 40;
static const int REMOVE_ALL_ID = 41;
static const int OPEN_ID = 42;
static const int SAVE_ID = 43;
static const int ROTATE_X = 44;
static const int ROTATE_Y = 45;
static const int ROTATE_Z = 46;

int window_;
Scene scene_;

int showAxis_;
int showWireframe_;
int onZ;

GLUI_Listbox *axisList_;
GLUI_Listbox *originList_;

GLUI_Rollout *scalePanel;
GLUI_Rollout *rotatePanel;
GLUI_Button *editButton;

GLUI_Listbox *shapeList_;
int shapePoints_;
int shapeSize_;

int rotateCount_ = 0;
int translateCount_ = 0;
int scaleCount_ = 0;

IAction *selectedAction_ = NULL;
GLUI_Listbox *actionList_;
GLUI_Listbox *selectedList_;
int actionSteps_;
int withPrevious_;

GLUI_Rollout *translatePanel;
int translateX_;
int translateY_;
int translateZ_;

int scaleX_;
int scaleY_;
int scaleZ_;

int rotationAmount_;

bool mouseCaptured_;
int clickCount_ = 0;
Point *selectedPoint_ = NULL;
double rotationY_ = 0;
double rotationZ_ = 0;

void Initialize() {
	glShadeModel(GL_SMOOTH);
	
	// Activate lighting.
	GLfloat ambient[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat lightPos[] = { 0.0, 100.0, 100.0, 1.0 };
	GLfloat lightPos2[] = { 0.0, 0.0, 200.0f, 1.0 };

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1,GL_POSITION,lightPos2);
	glEnable(GL_LIGHT1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// Activate antialiasing.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);

	// Create an empty shape and a sample action.
	scene_.SetShape(new Shape());
	IAction *action = new RotateAction(2 * M_PI, ROTATION_ZERO, AXIS_X);
	action->SetSteps(100);
	scene_.Storyboard().Actions().Add(action);
}

void Reshape(int w, int h) {
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);

	glViewport(tx, ty, tw, th);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-tw/2, tw/2, -th/2, th/2, -10000.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);
}

void DisplayBasicShape(Shape *shape) {
	// Draw the lines which form the shape.
	List<Point> &list = shape->Points();

	if(list.Count() >= 2) {
		glBegin(GL_LINE_STRIP);
			for(size_t i = 0; i < list.Count() - 1; i++) {
				glColor3f(0, 0, 0);
				glVertex3f(list[i].X, list[i].Y, list[i].Z);
				glColor3f(0, 0, 0);
				glVertex3f(list[i + 1].X, list[i + 1].Y, list[i + 1].Z);
			}
		glEnd();
	}

	// Draw the control points over the lines.
	glPointSize(8);

	glBegin(GL_POINTS);
		for(size_t i = 0;i < list.Count(); i++) {
			glColor3f(0, 0, 1);
			glVertex3f(list[i].X, list[i].Y, 1);
		}
	glEnd();
}

void DisplayBezierShape(BezierShape *shape) {
	// Draw the bezier lines which form the shape.
	List<Point> &list = shape->Points();

	if(list.Count() >= 2) {
		glBegin(GL_LINE_STRIP);
			for(size_t i = 0; i < list.Count() - 1; i++) {
				glColor3f(0, 0, 0);
				glVertex3f(list[i].X, list[i].Y, list[i].Z);
				glColor3f(0, 0, 0);
				glVertex3f(list[i + 1].X, list[i + 1].Y, list[i + 1].Z);
			}
		glEnd();
	}

	// Draw the control points over the lines.
	glPointSize(8);
	List<Point> &anchors = shape->AnchorPoints();

	glBegin(GL_POINTS);
		for(size_t i = 0; i < anchors.Count(); i++) {
			glColor3f(0, 0, 1);
			glVertex3f(anchors[i].X, anchors[i].Y, 0);
		}
	glEnd();

	glPointSize(6);
	List<Point> &control = shape->ControlPoints();

	glBegin(GL_POINTS);
		for(size_t i = 0; i < control.Count(); i++) {
			glColor3f(1, 0, 0);
			glVertex3f(control[i].X, control[i].Y, 0);
		}
	glEnd();

	// Draw a line between the control point and the anchor.
	glBegin(GL_LINES);
		for(size_t i = 0; i < control.Count(); i++) {
			int t = i > 0 && i % 2 ? 1 : 0;

			glColor3f(1, 0, 0);
			glVertex3f(control[i].X, control[i].Y, 0);
			glColor3f(1, 0, 0);
			glVertex3f(anchors[t+i/2].X, anchors[t+i/2].Y, 0);
		}
	glEnd();
}

void DisplayEdit() {
	switch(scene_.ShapeObject()->Type()) {
		case SHAPE_BASIC: {
			DisplayBasicShape(scene_.ShapeObject());
			break;
		}
		case SHAPE_BEZIER: {
			DisplayBezierShape((BezierShape *)scene_.ShapeObject());
			break;
		}
	}
}

void Normal(const Point &a, const Point &b, const Point &c) {
	double px = a.X - b.X;
	double py = a.Y - b.Y;
	double pz = a.Z - b.Z;
	double qx = c.X - b.X;
	double qy = c.Y - b.Y;
	double qz = c.Z - b.Z;

	Point normal = Point(py * qz - pz * qy,
						 px * qz - pz * qx,
						 px * qy - py * qx);		 
	double mag = normal.Magnitutde();
	normal.X /= mag;
	normal.Y /= mag;
	normal.Z /= mag;

	glNormal3f(normal.X, normal.Y, normal.Z);
}

void DisplayPlay() {
	glRotatef(rotationY_, 0, 1, 0);
	glRotatef(rotationZ_, 0, 0, 1);

	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0};
	GLfloat specReflection[] = { 0.9, 0.9, 0.9, 1.0};

	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
	glMateriali(GL_FRONT,GL_SHININESS, 200);

	// Build the surfaces by conecting the set of points.
	List<Point> *a;
	size_t listCount = scene_.Storyboard().Points().Count();

	for(size_t i = 0; i < listCount; i++) {
		List<Point> *b = scene_.Storyboard().Points()[i];

		if(i > 0) {
			glBegin(GL_QUAD_STRIP);
				for(size_t j = 0; j < b->Count(); j++) {
					glColor3f(0.3, 0.0, 1.0);				

					// Compute the normal to the surface.
					if(j < b->Count ()- 1) {
						Normal((*b)[j], (*b)[j + 1], (*a)[j]);
					}
					else {
						Normal((*b)[j], (*b)[j - 1], (*a)[j]);
					}

					glVertex3f((*a)[j].X, (*a)[j].Y, (*a)[j].Z);
					glColor3f(0.3, 0.0, 1.0);
					
					if(j < b->Count ()- 1) {
						Normal((*b)[j], (*b)[j + 1], (*a)[j]);
					}
					else {
						Normal((*b)[j], (*b)[j - 1], (*a)[j]);
					}


					glVertex3f((*b)[j].X, (*b)[j].Y, (*b)[j].Z);
				}
			glEnd();
		}

		a = b;
	}
}

void DisplayAxis() {
	glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		glVertex3f(-10000, 0, 0);
		glColor3f(0, 0, 0);
		glVertex3f(10000, 0, 0);

		glColor3f(0, 0, 0);
		glVertex3f(0, 10000, 0);
		glColor3f(0, 0, 0);
		glVertex3f(0, -10000, 0);

		glColor3f(0, 0, 0);
		glVertex3f(0, 0, -10000);
		glColor3f(0, 0, 0);
		glVertex3f(0, 0, 10000);
	glEnd();
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glClearColor(0.9, 0.9, 0.9, 1);

	// Select the current mode (edit or play).
	if(scene_.State() == SCENE_EDIT) {
		DisplayEdit();
	}
	else {
		DisplayPlay();
	}
	
	if(showAxis_) {
		DisplayAxis();
	}
	
	glPopMatrix();
	glutSwapBuffers(); 
}

void Idle() { 
	glutSetWindow(window_);  
	glutPostRedisplay();
  
	// Play the animation until the last frame is drawn.
	if(scene_.State() == SCENE_PLAY) {
		if(scene_.Storyboard().NextStep() == false) {
			scene_.SetState(SCENE_END);
		}
	}
}

int AxisToInt(RotationAxis axis) {
	return AXIS_X_ID + (int)axis;
}

RotationAxis AxisFromInt(int axis) {
	return (RotationAxis)(axis - AXIS_X_ID);
}

int OriginToInt(RotationOrigin origin) {
	return ORIGIN_LEFT_ID + (int)origin;
}

RotationOrigin OriginFromInt(int origin) {
	return (RotationOrigin)(origin - ORIGIN_LEFT_ID);
}

void LoadShape(int id, double size, int points) {
	scene_.Storyboard().Reset();
	scene_.SetState(SCENE_EDIT);

	switch(id) {
		case SHAPELIST_EMPTY_ID: {
			scene_.SetShape(new Shape());
			break;
		}
		case SHAPELIST_EMPTY_BEZIER_ID: {
			scene_.SetShape(new BezierShape());
			break;
		}
		case SHAPELIST_SQUARE_ID: {
			scene_.SetShape(ShapeGenerator::Square(size, points, (bool)onZ));
			break;
		}
		case SHAPELIST_CIRCLE_ID: {
			scene_.SetShape(ShapeGenerator::Circle(size, points, (bool)onZ));
			break;
		}
		case SHAPELIST_ARC_ID: {
			scene_.SetShape(ShapeGenerator::HalfCircle(size, points, (bool)onZ));
			break;
		}
		case SHAPELIST_LINE_ID: {
			scene_.SetShape(ShapeGenerator::Line(size, points, (bool)onZ));
			break;
		}
	}
}

void AddPoint(int x, int y, Shape *shape) {
	switch(shape->Type()) {
		case SHAPE_BASIC: {
			shape->Points().Add(Point(x, y));
			break;
		}
		case SHAPE_BEZIER: {
			// Add a new bezier control point and an anchor point.
			BezierShape *bs = (BezierShape *)shape;
			bs->AnchorPoints().Add(Point(x, y));
			

			if(bs->AnchorPoints().Count() > 2) {
				Point &last = bs->ControlPoints()[bs->ControlPoints().Count() - 2];
				bs->ControlPoints().Add(Point(last.X + 20, last.Y - 20, 0));
			}

			bs->ControlPoints().Add(Point(x + 20, y - 20));

			break;
		}
	}
}

void UpdatePoint(Point *point, int x, int y) {
	point->X = x;
	point->Y = y;
}

char* GetActionName(IAction *action) {
	static char *ROTATE_NAME = "Rotate #%d";
	static char *TRANSLATE_NAME = "Translate #%d";
	static char *SCALE_NAME = "Scale #%d";
	static char name[100];
	
	switch(action->Type()) {
		case ACTION_ROTATE: {
			sprintf(name, ROTATE_NAME, ++rotateCount_);
			return name;
		}
		case ACTION_TRANSLATE: {
			sprintf(name, TRANSLATE_NAME, ++translateCount_);
			return name;
		}
		case ACTION_SCALE: {
			sprintf(name, SCALE_NAME, ++scaleCount_);
			return name;
		 }
	}

	assert(false && "Unknown action!");
	return "<UNKNOWN>";
}

double FromRadians(double value) {
	return ceil((value * 180) / M_PI);
}

double ToRadians(double value) {
	return (value * M_PI) / 180;
}

void PopulateActionList() {
	rotateCount_ = 0;
	translateCount_ = 0;
	scaleCount_ = 0;

	for(size_t i = 0; i < scene_.Storyboard().ActionCount(); i++) {
		editButton->enable();
		selectedList_->enable();
		selectedList_->add_item(ACTION_ID + i, GetActionName(scene_.Storyboard().Actions()[i]));
	}	
}

void ClearActionList() {
	selectedAction_ = NULL;
	
	for(size_t i = 0; i < scene_.Storyboard().ActionCount(); i++) {
		selectedList_->delete_item(ACTION_ID + i);
	}
}

void RemoveAllActions() {
	for(size_t i = 0; i < scene_.Storyboard().ActionCount(); i++) {
		delete scene_.Storyboard().Actions()[i];
	}

	ClearActionList();
	scene_.Storyboard().Actions().Clear();
	scalePanel->close();
	rotatePanel->close();
	translatePanel->close();
	selectedList_->disable();
	editButton->disable();
}

void RemoveAction(IAction *action) {
	ClearActionList();
	scene_.Storyboard().Actions().Remove(action);
	delete action;
	PopulateActionList();

	if(scene_.Storyboard().Actions().Count() == 0) {
		RemoveAllActions();
	}
}

void UpdateTranslate(TranslateAction *action) {
	action->SetDeltaX(translateX_);
	action->SetDeltaY(translateY_);
	action->SetDeltaZ(translateZ_);
}

void UpdateRotate(RotateAction *action) {
	action->SetRotation(ToRadians(rotationAmount_));
	action->SetOrigin(OriginFromInt(originList_->get_int_val()));
	action->SetAxis(AxisFromInt(axisList_->get_int_val()));
}

void UpdateScale(ScaleAction *action) {
	action->SetScaleX(scaleX_);
	action->SetScaleY(scaleZ_);
	action->SetScaleZ(scaleY_);
}

void UpdateAction(IAction *action) {
	action->SetSteps(actionSteps_);
	action->SetWithPrevious((bool)withPrevious_);

	switch(action->Type()) {
		case ACTION_ROTATE: {
			UpdateRotate((RotateAction *)action);
			break;
		}
		case ACTION_TRANSLATE: {
			UpdateTranslate((TranslateAction *)action);
			break;
		}
		case ACTION_SCALE: {
			UpdateScale((ScaleAction *)action);
			break;
		}
	}
}

void ResetScene() {
	scene_.Storyboard().Reset();
	scene_.SetState(SCENE_EDIT);
}

void Play() {
	if(selectedAction_ != NULL) {
		UpdateAction(selectedAction_);
	}

	ResetScene();
	scene_.Storyboard().Play();
	scene_.SetState(SCENE_PLAY);
}

void ShowAction(IAction *action) {
	if(selectedAction_ != NULL && selectedAction_ != action) {
		UpdateAction(selectedAction_);
	}

	actionSteps_ = action->Steps();
	withPrevious_ = action->WithPrevious();

	rotatePanel->close();
	translatePanel->close();
	scalePanel->close();
	  
	switch(action->Type()) {
		case ACTION_ROTATE: {
			RotateAction *ra = (RotateAction *)action;
			axisList_->set_int_val(AxisToInt(ra->Axis()));
			originList_->set_int_val(OriginToInt(ra->Origin()));
			rotationAmount_ = FromRadians(ra->Rotation());
			rotatePanel->open();
			break;
		}
		case ACTION_TRANSLATE: {
			TranslateAction *ta = (TranslateAction *)action;
			translateX_ = ta->DeltaX();
			translateY_ = ta->DeltaY();
			translateZ_ = ta->DeltaZ();
			translatePanel->open();
			break;
		}
		case ACTION_SCALE: {
			ScaleAction *sa = (ScaleAction *)action;
			scaleX_ = sa->ScaleX();
			scaleY_ = sa->ScaleY();
			scaleZ_ = sa->ScaleZ();
			scalePanel->open();
			break;
		}
	}

	GLUI_Master.sync_live_all();
}

IAction* AddAction() {
	if(selectedAction_ != NULL) {
		UpdateAction(selectedAction_);
	}

	IAction *action;

	switch(actionList_->get_int_val()) {
		case ACTION_ROTATE_ID: {
			action = new RotateAction(0, ROTATION_ZERO, AXIS_X);			
			break;
		}
		case ACTION_TRANSLATE_ID: {
			action = new TranslateAction(0, 0, 0);
			break;
		}
		case ACTION_SCALE_ID: {
			action = new ScaleAction(0, 0, 0);	
			break;
		}
	}

	action->SetSteps(32);
	ClearActionList();
	scene_.Storyboard().Actions().Add(action);
	PopulateActionList();

	return action;
}

void Open() {
	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH + 1];

	// Initialize the open file dialog.
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Scene\0*.SCN\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(GetOpenFileName(&ofn)) {
		RemoveAllActions();
		scene_.SetShape(NULL);

		if(scene_.Open(ofn.lpstrFile)) {		
			PopulateActionList();
		}
		else {
			MessageBox(NULL, L"Failed to open file.", L"Error", MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void Save() {
	if(selectedAction_ != NULL) {
		UpdateAction(selectedAction_);
	}

	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH + 1];

	// Initialize the save file dialog.
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"Scene\0*.SCN\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(GetSaveFileName(&ofn)) {
		wcscat(ofn.lpstrFile, L".scn");

		if(scene_.Save(ofn.lpstrFile) == false) {
			MessageBox(NULL, L"Failed to save file.", L"Error", MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void MouseHandler(int button, int state, int x, int y) {
	// Compute the coordinates relative to the origin of the coordinate system.
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	x -= tw / 2;
	y = th/2 - y;

	if(button != GLUT_LEFT_BUTTON) {
		return;
	}

	if(scene_.State() == SCENE_END) {
		ResetScene();
		return;
	}
	else if(scene_.State() == SCENE_PLAY) {
		return;
	}

	if(state == GLUT_DOWN) {
		// Try to find the point found under the cursor.
		// If a point is found it is selected, otherwise a new point is created.
		selectedPoint_ = scene_.ShapeObject()->HitTest(x, y, 8);
		if(selectedPoint_ != NULL) {
			mouseCaptured_ = true;
		}
		else {
			AddPoint(x, y, scene_.ShapeObject());
		}
	}
	else {
		if(selectedPoint_ != NULL) {
			UpdatePoint(selectedPoint_, x, y);
		}

		mouseCaptured_ = false;
	}
}

void KeyHandler(unsigned char key, int x, int y) {
	// Handle key presses.
	switch(key) {
		case 13: { // Enter.
			Play();
			break;
		}
		case 27: { // Escape.
			ResetScene();
			break;
		}
		case 'o': {
			Open();
			break;
		}
		case 'a': {
			rotationY_ += 5;
			break;
		}
		case 's': {
			rotationY_ -= 5;
			break;
		}
		case 'd': {
			rotationZ_ += 5;
			break;
		}
		case 'f': {
			rotationZ_ -= 5;
			break;
		}
	}
}

void ControlHandler(int id) {
	// Handle actions associated with the buttons on the interface.
	switch(id) {
		case AXIS_ID: {
			break;
		}
		case PLAY_ID: {
			Play();
			break;
		}
		case LOAD_SHAPE_ID: {
			ResetScene();
			LoadShape(shapeList_->get_int_val(), shapeSize_, shapePoints_);
			break;
		}
		case RESET_SHAPE_ID: {
			ResetScene();
			scene_.ShapeObject()->Clear();			
			break;
		}
		case EDIT_ACTION_ID: {
			// A fost selectata o actiune.
			selectedAction_ = scene_.Storyboard().Actions()[selectedList_->get_int_val() - ACTION_ID];
			ShowAction(selectedAction_);
			break;
		}
		case UPDATE_ACTION_ID: {
			UpdateAction(selectedAction_);
			break;
		}
		case REMOVE_ACTION_ID: {
			RemoveAction(selectedAction_);
			selectedAction_ = NULL;
			break;
		}
		case ADD_ACTION_ID: {
			selectedAction_ = AddAction();
			ShowAction(selectedAction_);
			selectedList_->set_int_val(ACTION_ID + scene_.Storyboard().ActionCount() - 1);
			break;
		}
		case REMOVE_ALL_ID: {
			RemoveAllActions();
			selectedAction_ = NULL;
			break;
		}
		case OPEN_ID: {
			Open();
			break;
		}
		case SAVE_ID: {
			Save();
			break;
		}
		case ROTATE_Y: {
			rotationY_ += 5;
		}
		case ROTATE_Z: {
			rotationZ_ += 5;
		}
	}
}

int main(int argc, char* argv[]) {
	// Initialize and display the main window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition( 50, 50 );
	glutInitWindowSize(1200, 800);

	window_ = glutCreateWindow("Object Extrusion 3D | Copyright (c) Gratian Lup");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);  

	Initialize();

	// Register mouse and keyboard callbacks.
	GLUI_Master.set_glutKeyboardFunc(KeyHandler);
	GLUI_Master.set_glutMouseFunc(MouseHandler);
	GLUI_Master.set_glutIdleFunc(Idle);

	// Create the user interface by adding panels and other controls.
	GLUI *g = GLUI_Master.create_glui_subwindow(window_, GLUI_SUBWINDOW_RIGHT);

	GLUI_Panel *panel = g->add_panel("");
	panel->set_alignment(GLUI_ALIGN_LEFT);
	g->add_button_to_panel(panel,"Open", OPEN_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_button_to_panel(panel,"Save", SAVE_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	
	g->add_separator_to_panel(panel);

	g->add_button_to_panel(panel,"Play", PLAY_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_button_to_panel(panel,"Rotate Y", ROTATE_Y, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_button_to_panel(panel,"Rotate Z", ROTATE_Z, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_checkbox_to_panel(panel, "Show Axis", &showAxis_, AXIS_ID, ControlHandler);

	// Controls for adding shapes.
	GLUI_Rollout *loadPanel = g->add_rollout("New Shape");
	loadPanel->set_alignment(GLUI_ALIGN_LEFT);
	shapeList_ = g->add_listbox_to_panel(loadPanel, "Shape Type");
	shapeList_->add_item(SHAPELIST_EMPTY_ID, "Empty");
	shapeList_->add_item(SHAPELIST_EMPTY_BEZIER_ID, "Empty Bezier");
	shapeList_->add_item(SHAPELIST_ARC_ID, "Arc");
	shapeList_->add_item(SHAPELIST_CIRCLE_ID, "Circle");
	shapeList_->add_item(SHAPELIST_SQUARE_ID, "Square");
	shapeList_->add_item(SHAPELIST_LINE_ID, "Line");
	shapeList_->set_alignment(GLUI_ALIGN_LEFT);
	
	shapeSize_ = 150;
	shapePoints_ = 20;
	g->add_spinner_to_panel(loadPanel, "Size", 2, &shapeSize_)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_spinner_to_panel(loadPanel, "Points", 2, &shapePoints_)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_checkbox_to_panel(loadPanel, "On Z axis", &onZ);
	g->add_button_to_panel(loadPanel,"Create Shape", LOAD_SHAPE_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_button_to_panel(loadPanel,"Reset Shape", RESET_SHAPE_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);

	// Controls for adding actions.
	GLUI_Rollout *actionPanel = g->add_rollout("Actions");
	actionPanel->set_alignment(GLUI_ALIGN_LEFT);
	actionList_ = g->add_listbox_to_panel(actionPanel, "Action Type");
	g->add_button_to_panel(actionPanel,"Add Action", ADD_ACTION_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	actionList_->add_item(ACTION_ROTATE_ID, "Rotate");
	actionList_->add_item(ACTION_TRANSLATE_ID, "Translate");
	actionList_->add_item(ACTION_SCALE_ID, "Scale");
	g->add_button_to_panel(actionPanel,"Remove All", REMOVE_ALL_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);

	// Controls for editing the selected action.
	GLUI_Rollout *editPanel = g->add_rollout("Edit Action");
	selectedList_ = g->add_listbox_to_panel(editPanel, "Selected Action", NULL, ACTION_SELECTED_ID);
	editButton = g->add_button_to_panel(editPanel,"Edit Action", EDIT_ACTION_ID, ControlHandler);
	g->add_button_to_panel(editPanel,"Remove", REMOVE_ACTION_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	
	g->add_separator_to_panel(editPanel);

	editButton->set_alignment(GLUI_ALIGN_LEFT);
	g->add_spinner_to_panel(editPanel, "Steps", 2, &actionSteps_, STEP_ID)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_checkbox_to_panel(editPanel, "Play With Previous", &withPrevious_, PREVIOUS_ID, ControlHandler);
	g->add_button_to_panel(editPanel,"Update", UPDATE_ACTION_ID, ControlHandler)->set_alignment(GLUI_ALIGN_LEFT);
	
	// Controls for editing a translation action.
	translatePanel = g->add_rollout("Translate Action", 0);
	translatePanel->set_alignment(GLUI_ALIGN_LEFT);
	translatePanel->close();
	g->add_spinner_to_panel(translatePanel, "X", 2, &translateX_, TRANSLATE_X_ID)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_spinner_to_panel(translatePanel, "Y", 2, &translateY_, TRANSLATE_Y_ID)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_spinner_to_panel(translatePanel, "Z", 2, &translateZ_, TRANSLATE_Y_ID)->set_alignment(GLUI_ALIGN_LEFT);

	// Controls for editing a scalling action.
	scalePanel = g->add_rollout("Scale Action", 0);
	scalePanel->set_alignment(GLUI_ALIGN_LEFT);
	g->add_spinner_to_panel(scalePanel, "X", 2, &scaleX_, SCALE_X_ID)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_spinner_to_panel(scalePanel, "Y", 2, &scaleY_, SCALE_Y_ID)->set_alignment(GLUI_ALIGN_LEFT);
	g->add_spinner_to_panel(scalePanel, "Z", 2, &scaleZ_, SCALE_Z_ID)->set_alignment(GLUI_ALIGN_LEFT);

	// Controls for editing a rotation action.
	rotatePanel = g->add_rollout("Rotate Action", 0);
	rotatePanel->set_alignment(GLUI_ALIGN_LEFT);
	axisList_ = g->add_listbox_to_panel(rotatePanel, "Axis", NULL, ROTATION_AXIS_ID);
	axisList_->add_item(AXIS_X_ID, "X");
	axisList_->add_item(AXIS_Y_ID, "Y");
	axisList_->add_item(AXIS_Z_ID, "Z");

	originList_ = g->add_listbox_to_panel(rotatePanel, "Origin", NULL, ROTATION_ORIGIN_ID);
	originList_->add_item(ORIGIN_ZERO_ID, "Zero");
	originList_->add_item(ORIGIN_CENTER_ID, "Center");
	originList_->add_item(ORIGIN_LEFT_ID, "Left");
	originList_->add_item(ORIGIN_RIGHT_ID, "Right");
	originList_->add_item(ORIGIN_TOP_ID, "Top");
	originList_->add_item(ORIGIN_BOTTOM_ID, "Bottom");

	g->add_spinner_to_panel(rotatePanel, "Amount", 2, &rotationAmount_, ROTATION_AMOUNT_ID)
		->set_alignment(GLUI_ALIGN_LEFT);
	PopulateActionList();

	// Start drawing the interface and listening for events.
	glutMainLoop();
	return 0;
}
