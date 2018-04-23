//
// Copyright (c) 2009, Markus Rickert
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <iostream>
#include <rl/math/Matrix.h>
#include <rl/math/Rotation.h>
#include <rl/math/Spatial.h>
#include <rl/math/Transform.h>
#include <rl/math/Unit.h>
#include <rl/math/Vector.h>

void
testArticulatedBodyInertia()
{
	rl::math::ArticulatedBodyInertia abi1;
	abi1.cog().setRandom();
	abi1.inertia().setRandom();
	abi1.mass() = rl::math::DiagonalMatrix33(rl::math::Vector3::Random());
	
	rl::math::ArticulatedBodyInertia abi2;
	abi2.cog().setRandom();
	abi2.inertia().setRandom();
	abi2.mass() = rl::math::DiagonalMatrix33(rl::math::Vector3::Random());
	
	rl::math::ArticulatedBodyInertia abi3 = abi1 + abi2;
	rl::math::ArticulatedBodyInertia abi4 = abi3 - abi2;
	
	if (!abi4.matrix().isApprox(abi1.matrix()))
	{
		std::cerr << "abi1 + abi2 - abi2 != abi1" << std::endl;
		std::cerr << "abi1 + abi2 - abi2 = " << std::endl << abi4.matrix() << std::endl;
		std::cerr << "abi1 = " << std::endl << abi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::ArticulatedBodyInertia abi1_2 = abi1 * 2;
	rl::math::ArticulatedBodyInertia abi1_abi1 = abi1 + abi1;
	rl::math::ArticulatedBodyInertia abi1_2_05 = abi1_2 * 0.5;
	
	if (!abi1_2.matrix().isApprox(abi1_abi1.matrix()))
	{
		std::cerr << "abi1 * 2 != abi1 + abi1" << std::endl;
		std::cerr << "abi1 * 2 = " << std::endl << abi1_2.matrix() << std::endl;
		std::cerr << "abi1 + abi1 = " << std::endl << abi1_abi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if (!abi1_2_05.matrix().isApprox(abi1.matrix()))
	{
		std::cerr << "abi1 * 2 * 0.5 != abi1" << std::endl;
		std::cerr << "abi1 * 2 * 0.5 = " << std::endl << abi1_2.matrix() << std::endl;
		std::cerr << "abi1 = " << std::endl << abi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::PlueckerTransform pt1;
	pt1.rotation() = rl::math::Matrix33(rl::math::Quaternion::UniformRandom());
	pt1.translation().setRandom();
	
	rl::math::ArticulatedBodyInertia abi5 = pt1 * abi1;
	rl::math::ArticulatedBodyInertia abi6 = pt1 / abi5;
	
	if (!abi6.matrix().isApprox(abi1.matrix()))
	{
		std::cerr << "inv(pt1) * pt1 * abi1 != abi1" << std::endl;
		std::cerr << "inv(pt1) * pt1 * abi1 = " << std::endl << abi6.matrix() << std::endl;
		std::cerr << "abi1 = " << std::endl << abi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m1 = pt1.matrixForce() * abi1.matrix() * pt1.inverseMotion();
	
	if (!abi5.matrix().isApprox(m1))
	{
		std::cerr << "pt1 * abi1 != matrixForce(pt1) * abi1 * inverseMotion(pt1)" << std::endl;
		std::cerr << "pt1 * abi1 = " << std::endl << abi5.matrix() << std::endl;
		std::cerr << "matrixForce(pt1) * abi1 * inverseMotion(pt1) = " << std::endl << m1 << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m2 = pt1.matrixMotion().transpose() * m1 * pt1.matrixMotion();
	
	if (!abi1.matrix().isApprox(m2))
	{
		std::cerr << "abi1 != matrixMotion(pt1)^T * abi5 * matrixMotion(pt1)" << std::endl;
		std::cerr << "abi1 = " << std::endl << abi1.matrix() << std::endl;
		std::cerr << "matrixMotion(pt1)^T * abi5 * matrixMotion(pt1) = " << std::endl << m2 << std::endl;
		exit(EXIT_FAILURE);
	}
}

void
testEuclideanCrossProduct()
{
	rl::math::Vector3 a;
	a.setRandom();
	
	rl::math::Matrix33 m1a = a.cross33();
	rl::math::Matrix33 m1b = -a.cross33().transpose();
	
	if (!m1a.isApprox(m1b))
	{
		std::cerr << "ax != -ax^T" << std::endl;
		std::cerr << "ax = " << std::endl << m1a << std::endl;
		std::cerr << "-ax^T = " << std::endl << m1b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector3 b;
	b.setRandom();
	
	rl::math::Matrix33 m2a = (a + b).cross33();
	rl::math::Matrix33 m2b = a.cross33() + b.cross33();
	
	if (!m2a.isApprox(m2b))
	{
		std::cerr << "(a + b)x != ax + bx" << std::endl;
		std::cerr << "(a + b)x = " << std::endl << m2a << std::endl;
		std::cerr << "ax + bx = " << std::endl << m2b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix33 r = rl::math::Matrix33(rl::math::Quaternion::UniformRandom());
	
	rl::math::Matrix33 m3a = (r * a).cross33();
	rl::math::Matrix33 m3b = r * a.cross33() * r.transpose();
	
	if (!m3a.isApprox(m3b))
	{
		std::cerr << "(R * a)x != R ax R^T" << std::endl;
		std::cerr << "(R * a)x = " << std::endl << m3a << std::endl;
		std::cerr << "R ax R^T = " << std::endl << m3b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector3 v4a = a.cross33() * b;
	rl::math::Vector3 v4b = -b.cross33() * a;
	
	if (!v4a.isApprox(v4b))
	{
		std::cerr << "(ax) b != -(bx) a" << std::endl;
		std::cerr << "(ax) b = " << std::endl << v4a << std::endl;
		std::cerr << "-(bx) a = " << std::endl << v4b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector3 v5a = a.cross(b).transpose();
	rl::math::Vector3 v5b = a.transpose() * b.cross33();
	
	if (!v5a.isApprox(v5b))
	{
		std::cerr << "(a x b)^T != a^T bx" << std::endl;
		std::cerr << "(a x b)^T = " << std::endl << v5a << std::endl;
		std::cerr << "a^T bx = " << std::endl << v5b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix33 m6a = a.cross(b).cross33();
	rl::math::Matrix33 m6b = a.cross33() * b.cross33() - b.cross33() * a.cross33();
	
	if (!m6a.isApprox(m6b))
	{
		std::cerr << "(a x b)x != ax bx - bx ax" << std::endl;
		std::cerr << "(a x b)x = " << std::endl << m6a << std::endl;
		std::cerr << "ax bx - bx ax = " << std::endl << m6b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix33 m7a = a.cross(b).cross33();
	rl::math::Matrix33 m7b = b * a.transpose() - a * b.transpose();
	
	if (!m7a.isApprox(m7b))
	{
		std::cerr << "(a x b)x != b a^T - a b^T" << std::endl;
		std::cerr << "(a x b)x = " << std::endl << m7a << std::endl;
		std::cerr << "b a^T - a b^T = " << std::endl << m7b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix33 m8a = a.cross33() * b.cross33();
	rl::math::Matrix33 m8b = b * a.transpose() - a.dot(b) * rl::math::Matrix33::Identity();
	
	if (!m8a.isApprox(m8b))
	{
		std::cerr << "ax bx != b a^T - (a * b) I" << std::endl;
		std::cerr << "ax bx = " << std::endl << m8a << std::endl;
		std::cerr << "b a^T - (a * b) I = " << std::endl << m8b << std::endl;
		exit(EXIT_FAILURE);
	}
}

void
testForceVector()
{
	rl::math::Vector6 v0;
	v0.setRandom();
	
	rl::math::ForceVector fv0;
	fv0.moment() << v0.head<3>();
	fv0.force() << v0.tail<3>();
	
	if (!fv0.matrix().isApprox(v0))
	{
		std::cerr << "fv0.matrix() != v0" << std::endl;
		std::cerr << "fv0 = " << fv0.matrix().transpose() << std::endl;
		std::cerr << "v0 = " << v0.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v1;
	v1.setRandom();
	
	rl::math::ForceVector fv1(v1);
	
	if (!fv1.matrix().isApprox(v1))
	{
		std::cerr << "fv1(v1).matrix() != v1" << std::endl;
		std::cerr << "fv1(v1) = " << fv1.matrix().transpose() << std::endl;
		std::cerr << "v1 = " << v1.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v2;
	v2.setRandom();
	
	rl::math::ForceVector fv2 = v2;
	
	if (!fv2.matrix().isApprox(v2))
	{
		std::cerr << "(fv2 = v2).matrix() != v2" << std::endl;
		std::cerr << "(fv2 = v2) = " << fv2.matrix().transpose() << std::endl;
		std::cerr << "v2 = " << v2.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v3 = v1 + v2;
	rl::math::ForceVector fv3 = fv1 + fv2;
	
	if (!fv3.matrix().isApprox(v3))
	{
		std::cerr << "fv1 + fv2 != v1 + v2" << std::endl;
		std::cerr << "fv1 + fv2 = " << fv3.matrix().transpose() << std::endl;
		std::cerr << "v1 + v2 = " << v3.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v4 = v1 - v2;
	rl::math::ForceVector fv4 = fv1 - fv2;
	
	if (!fv4.matrix().isApprox(v4))
	{
		std::cerr << "fv1 - fv2 != v1 - v2" << std::endl;
		std::cerr << "fv1 - fv2 = " << fv4.matrix().transpose() << std::endl;
		std::cerr << "v1 - v2 = " << v4.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v5 = v1 * 1.23f;
	rl::math::ForceVector fv5 = fv1 * 1.23f;
	
	if (!fv5.matrix().isApprox(v5))
	{
		std::cerr << "fv1 * 1.23 != v1 * 1.23" << std::endl;
		std::cerr << "fv1 * 1.23 = " << fv5.matrix().transpose() << std::endl;
		std::cerr << "v1 * 1.23 = " << v5.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v6 = v1 / 1.23f;
	rl::math::ForceVector fv6 = fv1 / 1.23f;
	
	if (!fv6.matrix().isApprox(v6))
	{
		std::cerr << "fv1 / 1.23 != v1 / 1.23" << std::endl;
		std::cerr << "v1 * 1.23 = " << fv6.matrix().transpose() << std::endl;
		std::cerr << "v1 / 1.23 = " << v6.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::ForceVector fv7 = v1;
	
	if (!fv7.matrix().isApprox(v1))
	{
		std::cerr << "fv7 != v1" << std::endl;
		std::cerr << "fv7 = " << fv7.matrix().transpose() << std::endl;
		std::cerr << "v1 = " << v1.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void
testMotionVector()
{
	rl::math::Vector6 v0;
	v0.setRandom();
	
	rl::math::MotionVector mv0;
	mv0.angular() << v0.head<3>();
	mv0.linear() << v0.tail<3>();
	
	if (!mv0.matrix().isApprox(v0))
	{
		std::cerr << "mv0.matrix() != v0" << std::endl;
		std::cerr << "mv0 = " << mv0.matrix().transpose() << std::endl;
		std::cerr << "v0 = " << v0.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v1;
	v1.setRandom();
	
	rl::math::MotionVector mv1(v1);
	
	if (!mv1.matrix().isApprox(v1))
	{
		std::cerr << "mv1(v1).matrix() != v1" << std::endl;
		std::cerr << "mv1(v1) = " << mv1.matrix().transpose() << std::endl;
		std::cerr << "v1 = " << v1.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v2;
	v2.setRandom();
	
	rl::math::MotionVector mv2 = v2;
	
	if (!mv2.matrix().isApprox(v2))
	{
		std::cerr << "(mv2 = v2).matrix() != v2" << std::endl;
		std::cerr << "(mv2 = v2) = " << mv2.matrix().transpose() << std::endl;
		std::cerr << "v2 = " << v2.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v3 = v1 + v2;
	rl::math::MotionVector mv3 = mv1 + mv2;
	
	if (!mv3.matrix().isApprox(v3))
	{
		std::cerr << "mv1 + mv2 != v1 + v2" << std::endl;
		std::cerr << "mv1 + mv2 = " << mv3.matrix().transpose() << std::endl;
		std::cerr << "v1 + v2 = " << v3.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v4 = v1 - v2;
	rl::math::MotionVector mv4 = mv1 - mv2;
	
	if (!mv4.matrix().isApprox(v4))
	{
		std::cerr << "mv1 - mv2 != v1 - v2" << std::endl;
		std::cerr << "mv1 - mv2 = " << mv4.matrix().transpose() << std::endl;
		std::cerr << "v1 - v2 = " << v4.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v5 = v1 * 1.23f;
	rl::math::MotionVector mv5 = mv1 * 1.23f;
	
	if (!mv5.matrix().isApprox(v5))
	{
		std::cerr << "mv1 * 1.23 != v1 * 1.23" << std::endl;
		std::cerr << "mv1 * 1.23 = " << mv5.matrix().transpose() << std::endl;
		std::cerr << "v1 * 1.23 = " << v5.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v6 = v1 / 1.23f;
	rl::math::MotionVector mv6 = mv1 / 1.23f;
	
	if (!mv6.matrix().isApprox(v6))
	{
		std::cerr << "mv1 / 1.23 != v1 / 1.23" << std::endl;
		std::cerr << "v1 * 1.23 = " << mv6.matrix().transpose() << std::endl;
		std::cerr << "v1 / 1.23 = " << v6.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::MotionVector mv7 = v1;
	
	if (!mv7.matrix().isApprox(v1))
	{
		std::cerr << "mv7 != v1" << std::endl;
		std::cerr << "mv7 = " << mv7.matrix().transpose() << std::endl;
		std::cerr << "v1 = " << v1.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void
testPlueckerTransform()
{
	rl::math::Transform t0;
	t0 = rl::math::Quaternion::UniformRandom();
	t0.translation().setRandom();
	
	rl::math::PlueckerTransform pt0;
	pt0.rotation() = t0.linear();
	pt0.translation() = t0.translation();
	
	if (!pt0.rotation().isApprox(t0.linear()) || !pt0.translation().isApprox(t0.translation()))
	{
		std::cerr << "pt0 != t0" << std::endl;
		std::cerr << "pt0.rotation = " << std::endl << pt0.rotation() << std::endl;
		std::cerr << "pt0.translation = " << pt0.translation().transpose() << std::endl;
		std::cerr << "t0.linear = " << std::endl << t0.linear() << std::endl;
		std::cerr << "t0.translation = " << t0.translation().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Transform t1;
	t1 = rl::math::Quaternion::UniformRandom();
	t1.translation().setRandom();
	
	rl::math::PlueckerTransform pt1(t1);
	
	if (!pt1.rotation().isApprox(t1.linear()) || !pt1.translation().isApprox(t1.translation()))
	{
		std::cerr << "pt1(t1) != t1" << std::endl;
		std::cerr << "pt1(t1).rotation = " << std::endl << pt1.rotation() << std::endl;
		std::cerr << "pt1(t1).translation = " << pt1.translation().transpose() << std::endl;
		std::cerr << "t1.linear = " << std::endl << t1.linear() << std::endl;
		std::cerr << "t1.translation = " << t1.translation().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Transform t2;
	t2 = rl::math::Quaternion::UniformRandom();
	t2.translation().setRandom();
	
	rl::math::PlueckerTransform pt2 = t2;
	
	if (!pt2.rotation().isApprox(t2.linear()) || !pt2.translation().isApprox(t2.translation()))
	{
		std::cerr << "(pt2 = t2) != t2" << std::endl;
		std::cerr << "(pt2 = t2).rotation = " << std::endl << pt2.rotation() << std::endl;
		std::cerr << "(pt2 = t2).translation = " << pt2.translation().transpose() << std::endl;
		std::cerr << "t2.linear = " << std::endl << t2.linear() << std::endl;
		std::cerr << "t2.translation = " << t2.translation().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Transform t3 = t1 * t2;
	rl::math::PlueckerTransform pt3 = pt1 * pt2;
	
	if (!pt3.rotation().isApprox(t3.linear()) || !pt3.translation().isApprox(t3.translation()))
	{
		std::cerr << "pt1 * pt2 != t1 * t2" << std::endl;
		std::cerr << "(pt1 * pt2).rotation = " << std::endl << pt3.rotation() << std::endl;
		std::cerr << "(pt1 * pt2).translation = " << pt3.translation().transpose() << std::endl;
		std::cerr << "(t1 * t2).linear = " << std::endl << t3.linear() << std::endl;
		std::cerr << "(t1 * t2).translation = " << t3.translation().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Transform t4 = t1.inverse() * t3;
	rl::math::PlueckerTransform pt4 = pt1.inverse() * pt3;
	
	if (!pt4.rotation().isApprox(pt2.rotation()) || !pt4.translation().isApprox(pt2.translation()))
	{
		std::cerr << "inv(pt1) * pt1 * pt2 != pt2" << std::endl;
		std::cerr << "(inv(pt1) * pt1 * pt2).rotation = " << std::endl << pt4.rotation() << std::endl;
		std::cerr << "(inv(pt1) * pt1 * pt2).translation = " << pt4.translation().transpose() << std::endl;
		std::cerr << "pt2.rotation = " << std::endl << pt2.rotation().matrix() << std::endl;
		std::cerr << "pt2.translation = " << pt2.translation().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if (!pt4.rotation().isApprox(t4.linear()) || !pt4.translation().isApprox(t4.translation()))
	{
		std::cerr << "inv(pt1) * pt3 != inv(t1) * t3" << std::endl;
		std::cerr << "(inv(pt1) * pt3).rotation = " << std::endl << pt4.rotation() << std::endl;
		std::cerr << "(inv(pt1) * pt3).translation = " << pt4.translation().transpose() << std::endl;
		std::cerr << "(inv(t1) * t3).linear = " << std::endl << t4.linear() << std::endl;
		std::cerr << "(inv(t1) * t3).translation = " << t4.translation().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::PlueckerTransform pt5;
	pt5.setIdentity();
	rl::math::PlueckerTransform pt6 = pt1.inverse() * pt1;
	
	if (!pt5.rotation().isApprox(pt6.rotation()) || !pt5.translation().isMuchSmallerThan(pt6.translation()))
	{
		std::cerr << "I != inv(pt1) * pt1" << std::endl;
		std::cerr << "I.rotation = " << std::endl << pt5.rotation().matrix() << std::endl;
		std::cerr << "I.translation = " << pt5.translation().transpose() << std::endl;
		std::cerr << "(inv(pt1) * pt1).rotation = " << std::endl << pt6.rotation() << std::endl;
		std::cerr << "(inv(pt1) * pt1).translation = " << pt6.translation().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m1a = pt1.matrixMotion().inverse();
	rl::math::Matrix66 m1b = pt1.inverseMotion();
	
	if (!m1a.matrix().isApprox(m1b))
	{
		std::cerr << "matrixMotion(pt1)^-1 != inverseMotion(pt1)" << std::endl;
		std::cerr << "matrixMotion(pt1)^-1 = " << std::endl << m1a << std::endl;
		std::cerr << "inverseMotion(pt1) = " << std::endl << m1b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m2a = pt1.matrixForce().inverse();
	rl::math::Matrix66 m2b = pt1.inverseForce();
	
	if (!m2a.matrix().isApprox(m2b))
	{
		std::cerr << "matrixForce(pt1)^-1 != inverseForce(pt1)" << std::endl;
		std::cerr << "matrixForce(pt1)^-1 = " << std::endl << m2a << std::endl;
		std::cerr << "inverseForce(pt1) = " << std::endl << m2b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m3a = pt1.matrixForce();
	rl::math::Matrix66 m3b = pt1.matrixMotion().inverse().transpose();
	
	if (!m3a.matrix().isApprox(m3b))
	{
		std::cerr << "matrixForce(pt1) != matrixMotion(pt1)^-T" << std::endl;
		std::cerr << "matrixForce(pt1) = " << std::endl << m3a << std::endl;
		std::cerr << "matrixMotion(pt1)^-T = " << std::endl << m3b << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Vector6 v1;
	v1.setRandom();
	
	{
		rl::math::MotionVector mv1 = v1;
		
		rl::math::Vector6 v2 = pt1.matrixMotion() * v1;
		rl::math::MotionVector mv2 = pt1 * mv1;
		
		if (!mv2.matrix().isApprox(v2))
		{
			std::cerr << "pt1 * mv1 != matrixMotion(pt1) * v1" << std::endl;
			std::cerr << "pt1 * mv1 = " << mv2.matrix().transpose() << std::endl;
			std::cerr << "matrixMotion(pt1) * v1 = " << v2.transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
		
		rl::math::Vector6 v3 = pt1.inverseMotion() * v2;
		rl::math::MotionVector mv3 = pt1 / mv2;
		
		if (!mv3.matrix().isApprox(v3))
		{
			std::cerr << "inv(pt1) / mv2 != inverseMotion(pt1) * v2" << std::endl;
			std::cerr << "inv(pt1) / mv2 = " << mv3.matrix().transpose() << std::endl;
			std::cerr << "inverseMotion(pt1) * v2 = " << v3.transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
		
		if (!mv3.matrix().isApprox(mv1.matrix()))
		{
			std::cerr << "pt1.inverse() * pt1 * mv1 != mv1" << std::endl;
			std::cerr << "pt1.inverse() * pt1 * mv1 = " << mv3.matrix().transpose() << std::endl;
			std::cerr << "mv1 = " << mv1.matrix().transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
		
		rl::math::MotionVector mv4 = pt1.inverse() * mv2;
		
		if (!mv4.matrix().isApprox(mv1.matrix()))
		{
			std::cerr << "pt1.inverse() * pt1 * mv1 != mv1" << std::endl;
			std::cerr << "pt1.inverse() * pt1 * mv1 = " << mv4.matrix().transpose() << std::endl;
			std::cerr << "mv1 = " << mv1.matrix().transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	
	{
		rl::math::ForceVector fv1 = v1;
		
		rl::math::Vector6 v2 = pt1.matrixForce() * v1;
		rl::math::ForceVector fv2 = pt1 * fv1;
		
		if (!fv2.matrix().isApprox(v2))
		{
			std::cerr << "pt1 * fv1 != matrixForce(pt1) * v1" << std::endl;
			std::cerr << "pt1 * fv1 = " << fv2.matrix().transpose() << std::endl;
			std::cerr << "matrixForce(pt1) * v1 = " << v2.transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
		
		rl::math::Vector6 v3 = pt1.inverseForce() * v2;
		rl::math::ForceVector fv3 = pt1 / fv2;
		
		if (!fv3.matrix().isApprox(v3))
		{
			std::cerr << "inv(pt1) / fv2 != inverseForce(pt1) * v2" << std::endl;
			std::cerr << "inv(pt1) / fv2 = " << fv3.matrix().transpose() << std::endl;
			std::cerr << "inverseForce(pt1) * v2 = " << v3.transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
		
		if (!fv3.matrix().isApprox(fv1.matrix()))
		{
			std::cerr << "pt1.inverse() * pt1 * fv1 != fv1" << std::endl;
			std::cerr << "pt1.inverse() * pt1 * fv1 = " << fv3.matrix().transpose() << std::endl;
			std::cerr << "fv1 = " << fv1.matrix().transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
		
		rl::math::ForceVector fv4 = pt1.inverse() * fv2;
		
		if (!fv4.matrix().isApprox(fv1.matrix()))
		{
			std::cerr << "pt1.inverse() * pt1 * fv1 != fv1" << std::endl;
			std::cerr << "pt1.inverse() * pt1 * fv1 = " << fv4.matrix().transpose() << std::endl;
			std::cerr << "fv1 = " << fv1.matrix().transpose() << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

void
testRigidBodyInertia()
{
	rl::math::RigidBodyInertia rbi1;
	rbi1.cog().setRandom();
	rbi1.inertia().setRandom();
	rbi1.mass() = 10;
	
	rl::math::RigidBodyInertia rbi2;
	rbi2.cog().setRandom();
	rbi2.inertia().setRandom();
	rbi2.mass() = 20;
	
	rl::math::RigidBodyInertia rbi3 = rbi1 + rbi2;
	rl::math::RigidBodyInertia rbi4 = rbi3 - rbi2;
	
	if (!rbi4.matrix().isApprox(rbi1.matrix()))
	{
		std::cerr << "rbi1 + rbi2 - rbi2 != rbi1" << std::endl;
		std::cerr << "rbi1 + rbi2 - rbi2 = " << std::endl << rbi4.matrix() << std::endl;
		std::cerr << "rbi1 = " << std::endl << rbi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::RigidBodyInertia rbi1_2 = rbi1 * 2;
	rl::math::RigidBodyInertia rbi1_rbi1 = rbi1 + rbi1;
	rl::math::RigidBodyInertia rbi1_2_05 = rbi1_2 * 0.5;
	
	if (!rbi1_2.matrix().isApprox(rbi1_rbi1.matrix()))
	{
		std::cerr << "rbi1 * 2 != rbi1 + rbi1" << std::endl;
		std::cerr << "rbi1 * 2 = " << std::endl << rbi1_2.matrix() << std::endl;
		std::cerr << "rbi1 + rbi1 = " << std::endl << rbi1_rbi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if (!rbi1_2_05.matrix().isApprox(rbi1.matrix()))
	{
		std::cerr << "rbi1 * 2 * 0.5 != rbi1" << std::endl;
		std::cerr << "rbi1 * 2 * 0.5 = " << std::endl << rbi1_2_05.matrix() << std::endl;
		std::cerr << "rbi1 = " << std::endl << rbi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::PlueckerTransform pt1;
	pt1.rotation() = rl::math::Matrix33(rl::math::Quaternion::UniformRandom());
	pt1.translation().setRandom();
	
	rl::math::RigidBodyInertia rbi5 = pt1 * rbi1;
	rl::math::RigidBodyInertia rbi6 = pt1 / rbi5;
	
	if (!rbi6.matrix().isApprox(rbi1.matrix()))
	{
		std::cerr << "inv(pt1) * pt1 * rbi1 != rbi1" << std::endl;
		std::cerr << "inv(pt1) * pt1 * rbi1 = " << std::endl << rbi6.matrix() << std::endl;
		std::cerr << "rbi1 = " << std::endl << rbi1.matrix() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m1 = pt1.matrixForce() * rbi1.matrix() * pt1.inverseMotion();
	
	if (!rbi5.matrix().isApprox(m1))
	{
		std::cerr << "pt1 * rbi1 != matrixForce(pt1) * rbi1 * inverseMotion(pt1)" << std::endl;
		std::cerr << "pt1 * rbi1 = " << std::endl << rbi5.matrix() << std::endl;
		std::cerr << "matrixForce(pt1) * rbi1 * inverseMotion(pt1) = " << std::endl << m1 << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m2 = pt1.matrixMotion().transpose() * m1 * pt1.matrixMotion();
	
	if (!rbi1.matrix().isApprox(m2))
	{
		std::cerr << "rbi1 != matrixMotion(pt1)^T * rbi5 * matrixMotion(pt1)" << std::endl;
		std::cerr << "rbi1 = " << std::endl << rbi1.matrix() << std::endl;
		std::cerr << "matrixMotion(pt1)^T * rbi5 * matrixMotion(pt1) = " << std::endl << m2 << std::endl;
		exit(EXIT_FAILURE);
	}
}

void
testSpatialCrossProduct()
{
	rl::math::Vector6 v1;
	v1.setRandom();
	
	rl::math::Vector6 v2;
	v2.setRandom();
	
	rl::math::MotionVector mv1 = v1;
	
	rl::math::MotionVector mv2 = v2;
	
	rl::math::Vector6 v3 = mv1.cross66Motion() * v2;
	rl::math::MotionVector mv3 = mv1.cross(mv2);
	
	if (!mv3.matrix().isApprox(v3))
	{
		std::cerr << "mv1 x mv2 != crossMotion(mv1) x v2" << std::endl;
		std::cerr << "mv1 x mv2 = " << mv3.matrix().transpose() << std::endl;
		std::cerr << "crossMotion(mv1) x v2 = " << v3.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::MotionVector mv4 = -mv2.cross(mv1);
	
	if (!mv4.matrix().isApprox(mv3.matrix()))
	{
		std::cerr << "mv1 x mv2 != -mv2 x mv1" << std::endl;
		std::cerr << "mv1 x mv2 = " << mv3.matrix().transpose() << std::endl;
		std::cerr << "-mv2 x mv1 = " << mv4.matrix().transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::ForceVector fv2 = v2;
	
	rl::math::Vector6 v5 = mv1.cross66Force() * v2;
	rl::math::ForceVector fv5 = mv1.cross(fv2);
	
	if (!fv5.matrix().isApprox(v5))
	{
		std::cerr << "mv1 x fv2 != crossForce(mv1) x v2" << std::endl;
		std::cerr << "mv1 x fv2 = " << fv5.matrix().transpose() << std::endl;
		std::cerr << "crossForce(mv1) x v2 = " << v5.transpose() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 mv1CrossForce = -mv1.cross66Motion().transpose();
	
	if (!mv1CrossForce.matrix().isApprox(mv1.cross66Force()))
	{
		std::cerr << "crossForce(mv1) != -crossMotion(mv1)^T" << std::endl;
		std::cerr << "crossForce(mv1) = " << std::endl << mv1CrossForce << std::endl;
		std::cerr << "-crossMotion(mv1)^T = " << std::endl << mv1.cross66Force() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 cross1 = (mv1 + mv2).cross66Motion();
	rl::math::Matrix66 cross2 = mv1.cross66Motion() + mv2.cross66Motion();
	
	if (!cross1.matrix().isApprox(cross2))
	{
		std::cerr << "crossMotion(mv1 + mv2) != crossMotion(mv1) + crossMotion(mv2)" << std::endl;
		std::cerr << "crossMotion(mv1 + mv2) = " << std::endl << cross1 << std::endl;
		std::cerr << "crossMotion(mv1) + crossMotion(mv2) = " << std::endl << cross2 << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 cross3 = (mv1 + mv2).cross66Force();
	rl::math::Matrix66 cross4 = mv1.cross66Force() + mv2.cross66Force();
	
	if (!cross3.matrix().isApprox(cross4))
	{
		std::cerr << "crossForce(mv1 + mv2) != crossForce(mv1) + crossForce(mv2)" << std::endl;
		std::cerr << "crossForce(mv1 + mv2) = " << std::endl << cross3 << std::endl;
		std::cerr << "crossForce(mv1) + crossForce(mv2) = " << std::endl << cross4 << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::PlueckerTransform pt;
	pt.rotation() = rl::math::Matrix33(rl::math::Quaternion::UniformRandom());
	pt.translation().setRandom();
	
	rl::math::Matrix66 m1 = (pt * mv1).cross66Motion();
	rl::math::Matrix66 m2 = pt.matrixMotion() * mv1.cross66Motion() * pt.inverse().matrixMotion();
	
	if (!m1.matrix().isApprox(m2))
	{
		std::cerr << "crossMotion(pt * mv1) != pt * crossMotion(mv1) * pt^-1" << std::endl;
		std::cerr << "crossMotion(pt * mv1) = " << std::endl << m1 << std::endl;
		std::cerr << "pt * crossMotion(mv1) * pt^-1 = " << std::endl << m2 << std::endl;
		exit(EXIT_FAILURE);
	}
	
	rl::math::Matrix66 m3 = (pt * mv1).cross66Force();
	rl::math::Matrix66 m4 = pt.matrixForce() * mv1.cross66Force() * pt.inverse().matrixForce();
	
	if (!m3.matrix().isApprox(m4))
	{
		std::cerr << "crossForce(pt * mv1) != pt * crossForce(mv1) * pt^-1" << std::endl;
		std::cerr << "crossForce(pt * mv1) = " << std::endl << m3 << std::endl;
		std::cerr << "pt * crossForce(mv1) * pt^-1 = " << std::endl << m4 << std::endl;
		exit(EXIT_FAILURE);
	}
}

void
testSpatialDotProduct()
{
	rl::math::MotionVector mv1;
	mv1.angular().setRandom();
	mv1.linear().setRandom();
	
	rl::math::ForceVector fv2;
	fv2.moment().setRandom();
	fv2.force().setRandom();
	
	rl::math::Real dot1 = mv1.angular().dot(fv2.moment()) + mv1.linear().dot(fv2.force());
	rl::math::Real dot2 = mv1.dot(fv2);
	
	if (!Eigen::internal::isApprox(dot2, dot1))
	{
		std::cerr << "mv1 * fv2 != mv1.angular * fv2.moment + mv1.linear * fv2.force" << std::endl;
		std::cerr << "mv1 * fv2 = " << dot2 << std::endl;
		std::cerr << "mv1.angular * fv2.moment + mv1.linear * fv2.force = " << dot1 << std::endl;
		exit(EXIT_FAILURE);
	}
}

int
main(int argc, char** argv)
{
	testEuclideanCrossProduct();
	testMotionVector();
	testForceVector();
	testSpatialCrossProduct();
	testSpatialDotProduct();
	testPlueckerTransform();
	testRigidBodyInertia();
	testArticulatedBodyInertia();
	return EXIT_SUCCESS;
}