// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef STORMEIGEN_CAMERA_H
#define STORMEIGEN_CAMERA_H

#include <StormEigen/Geometry>
#include <QObject>
// #include <frame.h>

class Frame
{
  public:
    STORMEIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
    inline Frame(const StormEigen::Vector3f& pos = StormEigen::Vector3f::Zero(),
                 const StormEigen::Quaternionf& o = StormEigen::Quaternionf())
      : orientation(o), position(pos)
    {}
    Frame lerp(float alpha, const Frame& other) const
    {
      return Frame((1.f-alpha)*position + alpha * other.position,
                   orientation.slerp(alpha,other.orientation));
    }

    StormEigen::Quaternionf orientation;
    StormEigen::Vector3f position;
};

class Camera
{
  public:
    STORMEIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Camera(void);
    
    Camera(const Camera& other);
    
    virtual ~Camera();
    
    Camera& operator=(const Camera& other);
    
    void setViewport(uint offsetx, uint offsety, uint width, uint height);
    void setViewport(uint width, uint height);
    
    inline uint vpX(void) const { return mVpX; }
    inline uint vpY(void) const { return mVpY; }
    inline uint vpWidth(void) const { return mVpWidth; }
    inline uint vpHeight(void) const { return mVpHeight; }

    inline float fovY(void) const { return mFovY; }
    void setFovY(float value);
    
    void setPosition(const StormEigen::Vector3f& pos);
    inline const StormEigen::Vector3f& position(void) const { return mFrame.position; }

    void setOrientation(const StormEigen::Quaternionf& q);
    inline const StormEigen::Quaternionf& orientation(void) const { return mFrame.orientation; }

    void setFrame(const Frame& f);
    const Frame& frame(void) const { return mFrame; }
    
    void setDirection(const StormEigen::Vector3f& newDirection);
    StormEigen::Vector3f direction(void) const;
    void setUp(const StormEigen::Vector3f& vectorUp);
    StormEigen::Vector3f up(void) const;
    StormEigen::Vector3f right(void) const;
    
    void setTarget(const StormEigen::Vector3f& target);
    inline const StormEigen::Vector3f& target(void) { return mTarget; }
    
    const StormEigen::Affine3f& viewMatrix(void) const;
    const StormEigen::Matrix4f& projectionMatrix(void) const;
    
    void rotateAroundTarget(const StormEigen::Quaternionf& q);
    void localRotate(const StormEigen::Quaternionf& q);
    void zoom(float d);
    
    void localTranslate(const StormEigen::Vector3f& t);
    
    /** Setup OpenGL matrices and viewport */
    void activateGL(void);
    
    StormEigen::Vector3f unProject(const StormEigen::Vector2f& uv, float depth, const Eigen::Matrix4f& invModelview) const;
    StormEigen::Vector3f unProject(const StormEigen::Vector2f& uv, float depth) const;
    
  protected:
    void updateViewMatrix(void) const;
    void updateProjectionMatrix(void) const;

  protected:

    uint mVpX, mVpY;
    uint mVpWidth, mVpHeight;

    Frame mFrame;
    
    mutable StormEigen::Affine3f mViewMatrix;
    mutable StormEigen::Matrix4f mProjectionMatrix;

    mutable bool mViewIsUptodate;
    mutable bool mProjIsUptodate;

    // used by rotateAroundTarget
    StormEigen::Vector3f mTarget;
    
    float mFovY;
    float mNearDist;
    float mFarDist;
};

#endif // STORMEIGEN_CAMERA_H
