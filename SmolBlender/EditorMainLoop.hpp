#pragma once

#include "MainLoop.hpp"

namespace Smol {
	class EditorMainLoop : public MainLoop {
	public:
		EditorMainLoop() = default;
		~EditorMainLoop() = default;

		virtual void initialize() override;
		virtual bool update(float deltaTime, float totalTime) override;
		virtual void finalize() override;
	};
}