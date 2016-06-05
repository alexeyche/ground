#pragma once

#include <ground/base/base.h>
#include <ground/ptr.h>
#include <ground/protos/distr.pb.h>

#include <random>
#include <chrono>

namespace NGround {

	class TRandEngine {
	public:
		TRandEngine(int seed)
			: Unif(0.0,1.0)
			, Norm(0.0, 1.0)
		{
			ui32 seedNum = seed >= 0 ? seed : std::chrono::system_clock::now().time_since_epoch().count();
			Generator = std::mt19937(seedNum);
			L_DEBUG << "Initialize rand engine with seed " << seedNum;
		}

		double GetUnif() {
			return Unif(Generator);
		}

		double GetNorm() {
			return Norm(Generator);
		}
		
		double GetExp(double rate) {
			return -std::log(GetUnif())/rate;
		}

		double DrawValue(const NGroundProto::TDistr& d) {
			if (d.has_value()) {
				return d.value();
			} else
			if (d.has_exp()) {
				return GetExp(d.exp().rate());
			} else 
			if (d.has_norm()) {
				return d.norm().mean() + d.norm().sd()*GetNorm();
			}
			return 0.0;
		}

 	private:
		std::uniform_real_distribution<double> Unif;
		std::normal_distribution<double> Norm;

		std::mt19937 Generator;
	};



} // namespace NGround