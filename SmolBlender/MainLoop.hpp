#pragma once

namespace Smol
{
	class MainLoop {
	public:
		virtual void initialize() {}
		virtual bool update(float deltaTime, float totalTime) = 0;
		virtual void finalize() {}
	};
}