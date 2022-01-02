#pragma once

#include "../pin.h"
#include "../event-nodes/event-node.h"

namespace seam {
	namespace props {
		/// these are for drawing properties which are not pins
		bool Draw(
			std::string_view label,
			float& v,
			float min = -FLT_MAX,
			float max = FLT_MAX,
			float speed = .001f,
			ImGuiSliderFlags flags = 0
		);
		bool Draw(
			std::string_view label,
			int& v,
			int min = INT_MIN,
			int max = INT_MAX,
			float speed = .001f,
			ImGuiSliderFlags flags = 0
		);

		bool Draw(std::string_view label, std::string& v);

		/// draws a reload button in addition to providing a path setter
		bool DrawShaderPath(std::string_view label, std::string& path);

		bool DrawPin(std::string_view name, PinFloatMeta* pin, float* channels, size_t len_channels, float speed = 0.01f);
		
		bool DrawPin(std::string_view name, PinIntMeta* pin, int* channels, size_t len_channels, float speed = 0.01f);


		/// generic for drawing different input pin types
		bool DrawPinInput(IPinInput* input);

		/// utility function for drawing all a node's input pins
		bool DrawPinInputs(IEventNode* node);

		// ...TODO
	}
}