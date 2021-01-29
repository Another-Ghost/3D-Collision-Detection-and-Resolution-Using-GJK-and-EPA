/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once

#include <chrono>

namespace NCL {
	typedef  std::chrono::time_point<std::chrono::high_resolution_clock>  Timepoint;

	class GameTimer {
	public:
		GameTimer(void);
		~GameTimer(void) {}

		double	GetTotalTimeSeconds()	const;
		double	GetTotalTimeMSec()		const;

		float	GetTimeDeltaSeconds()	const { return timeDelta; };
		float	GetTimeDeltaMSec()		const { return timeDelta * 1000.0f; };

		void	Tick();
	protected:
		float		timeDelta;
		Timepoint	firstPoint;
		Timepoint	nowPoint;
	};
}

