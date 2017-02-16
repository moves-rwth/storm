// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_TRACKBALL_H
#define STORMEIGEN_TRACKBALL_H

#include <StormEigen/Geometry>

class Camera;

class Trackball
{
  public:

    enum Mode {Around, Local};

    Trackball() : mpCamera(0) {}

    void start(Mode m = Around) { mMode = m; mLastPointOk = false; }

    void setCamera(Camera* pCam) { mpCamera = pCam; }

    void track(const StormEigen::Vector2i& newPoint2D);

  protected:

    bool mapToSphere( const StormEigen::Vector2i& p2, StormEigen::Vector3f& v3);

    Camera* mpCamera;
    StormEigen::Vector3f mLastPoint3D;
    Mode mMode;
    bool mLastPointOk;

};

#endif // STORMEIGEN_TRACKBALL_H
