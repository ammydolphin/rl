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

#include <rl/math/algorithm.h>
#include <rl/math/Rotation.h>

#include "SixDof.h"

namespace rl
{
	namespace mdl
	{
		SixDof::SixDof() :
			Joint(7, 6)
		{
			this->max.tail<4>().setConstant(1); // TODO
			this->min.tail<4>().setConstant(-1); // TODO
			this->qUnits(0) = ::rl::math::UNIT_METER;
			this->qUnits(1) = ::rl::math::UNIT_METER;
			this->qUnits(2) = ::rl::math::UNIT_METER;
			this->qUnits(3) = ::rl::math::UNIT_NONE; // TODO
			this->qUnits(4) = ::rl::math::UNIT_NONE; // TODO
			this->qUnits(5) = ::rl::math::UNIT_NONE; // TODO
			this->qUnits(6) = ::rl::math::UNIT_NONE; // TODO
			this->qdUnits(0) = ::rl::math::UNIT_METER_PER_SECOND;
			this->qdUnits(1) = ::rl::math::UNIT_METER_PER_SECOND;
			this->qdUnits(2) = ::rl::math::UNIT_METER_PER_SECOND;
			this->qdUnits(3) = ::rl::math::UNIT_RADIAN_PER_SECOND;
			this->qdUnits(4) = ::rl::math::UNIT_RADIAN_PER_SECOND;
			this->qdUnits(5) = ::rl::math::UNIT_RADIAN_PER_SECOND;
			this->qddUnits(0) = ::rl::math::UNIT_METER_PER_SECOND_SQUARED;
			this->qddUnits(1) = ::rl::math::UNIT_METER_PER_SECOND_SQUARED;
			this->qddUnits(2) = ::rl::math::UNIT_METER_PER_SECOND_SQUARED;
			this->qddUnits(3) = ::rl::math::UNIT_RADIAN_PER_SECOND_SQUARED;
			this->qddUnits(4) = ::rl::math::UNIT_RADIAN_PER_SECOND_SQUARED;
			this->qddUnits(5) = ::rl::math::UNIT_RADIAN_PER_SECOND_SQUARED;
			this->S.topLeftCorner<3, 3>().setZero();
			this->S.topRightCorner<3, 3>().setIdentity();
			this->S.bottomLeftCorner<3, 3>().setIdentity();
			this->S.bottomRightCorner<3, 3>().setZero();
			this->speedUnits(0) = ::rl::math::UNIT_METER_PER_SECOND;
			this->speedUnits(1) = ::rl::math::UNIT_METER_PER_SECOND;
			this->speedUnits(2) = ::rl::math::UNIT_METER_PER_SECOND;
			this->speedUnits(3) = ::rl::math::UNIT_RADIAN_PER_SECOND;
			this->speedUnits(4) = ::rl::math::UNIT_RADIAN_PER_SECOND;
			this->speedUnits(5) = ::rl::math::UNIT_RADIAN_PER_SECOND;
			this->tauUnits(0) = ::rl::math::UNIT_NEWTON;
			this->tauUnits(1) = ::rl::math::UNIT_NEWTON;
			this->tauUnits(2) = ::rl::math::UNIT_NEWTON;
			this->tauUnits(3) = ::rl::math::UNIT_NEWTON_METER;
			this->tauUnits(4) = ::rl::math::UNIT_NEWTON_METER;
			this->tauUnits(5) = ::rl::math::UNIT_NEWTON_METER;
		}
		
		SixDof::~SixDof()
		{
		}
		
		void
		SixDof::clamp(::rl::math::VectorRef q) const
		{
			for (::std::size_t i = 0; i < 3; ++i)
			{
				q(i) = ::rl::math::clamp(q(i), this->min(i), this->max(i));
			}
			
			::Eigen::Map< ::rl::math::Quaternion>(q.tail<4>().data()).normalize();
		}
		
		::rl::math::Real
		SixDof::distance(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& q2) const
		{
			return ::std::sqrt(this->transformedDistance(q1, q2));
		}
		
		void
		SixDof::generatePositionGaussian(const ::rl::math::ConstVectorRef& rand, const ::rl::math::ConstVectorRef& mean, const ::rl::math::ConstVectorRef& sigma, ::rl::math::VectorRef q) const
		{
			for (::std::size_t i = 0; i < 3; ++i)
			{
				q(i) = mean(i) + rand(i) * sigma(i);
				q(i) = ::rl::math::clamp(q(i), this->min(i), this->max(i));
			}
			
			q.tail<4>() = ::rl::math::Quaternion::Random(rand.tail<3>(), ::Eigen::Map< const ::rl::math::Quaternion>(mean.tail<4>().data()), sigma.tail<3>()).coeffs();
		}
		
		void
		SixDof::generatePositionUniform(const ::rl::math::ConstVectorRef& rand, ::rl::math::VectorRef q) const
		{
			for (::std::size_t i = 0; i < 3; ++i)
			{
				q(i) = this->min(i) + rand(i) * (this->max(i) - this->min(i));
			}
			
			q.tail<4>() = ::rl::math::Quaternion::Random(rand.tail<3>()).coeffs();
		}
		
		void
		SixDof::interpolate(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& q2, const ::rl::math::Real& alpha, ::rl::math::VectorRef q) const
		{
			q.head<3>() = (1.0f - alpha) * q1.head<3>() + alpha * q2.head<3>();
			::Eigen::Map<const ::rl::math::Quaternion> quaternion1(q1.tail<4>().data());
			::Eigen::Map<const ::rl::math::Quaternion> quaternion2(q2.tail<4>().data());
			q.tail<4>() = quaternion1.slerp(alpha, quaternion2).coeffs();
		}
		
		bool
		SixDof::isValid(const ::rl::math::ConstVectorRef& q) const
		{
			for (::std::size_t i = 0; i < 3; ++i)
			{
				if (q(i) < this->min(i) || q(i) > this->max(i))
				{
					return false;
				}
			}
			
			return ::Eigen::internal::isApprox(q.tail<4>().norm(), static_cast< ::rl::math::Real>(1), 1.0e-3f);
		}
		
		void
		SixDof::normalize(::rl::math::VectorRef q) const
		{
			::Eigen::Map< ::rl::math::Quaternion>(q.tail<4>().data()).normalize();
		}
		
		void
		SixDof::setPosition(const ::rl::math::ConstVectorRef& q)
		{
			this->q = q;
			this->x.translation() = this->q.head<3>() + this->offset.head<3>();
			this->x.linear() = ::Eigen::Map<const ::rl::math::Quaternion>(this->q.tail<4>().data()).toRotationMatrix();
		}
		
		void
		SixDof::step(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& qdot, ::rl::math::VectorRef q2) const
		{
			q2.head<3>() = q1.head<3>() + qdot.head<3>();
			
			for (::std::size_t i = 0; i < 3; ++i)
			{
				q2(i) = ::rl::math::clamp(q2(i), this->min(i), this->max(i));
			}
			
			::Eigen::Map<const ::rl::math::Quaternion> quaternion1(q1.tail<4>().data());
			q2.tail<4>() = (quaternion1 * quaternion1.firstDerivative(qdot.tail<3>())).coeffs();
		}
		
		::rl::math::Real
		SixDof::transformedDistance(const ::rl::math::ConstVectorRef& q1, const ::rl::math::ConstVectorRef& q2) const
		{
			::Eigen::Map<const ::rl::math::Quaternion> quaternion1(q1.tail<4>().data());
			::Eigen::Map<const ::rl::math::Quaternion> quaternion2(q2.tail<4>().data());
			return (q2.head<3>() - q1.head<3>()).squaredNorm() + ::std::pow(quaternion1.angularDistance(quaternion2), 2);
		}
	}
}