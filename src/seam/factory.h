#pragma once

#include "nodes/i-node.h"

namespace seam {
	class EventNodeFactory {
	public:
		using CreateFunc = std::function<nodes::INode*()>;

		/// holds metadata about an event node, and a function that can create one
		struct Generator {
			/// the node's hash, calculated from its name
			seam::nodes::NodeId node_id;

			/// the node's reported human readable name
			std::string_view node_name;

			/// list of unique input types
			std::vector<PinType> pin_inputs;

			/// list of unique output types
			std::vector<PinType> pin_outputs;

			/// can create a unique instance of the node described by this struct
			CreateFunc Create;

			bool operator <(const Generator& other) {
				return node_id < other.node_id;
			}

			bool operator <(const seam::nodes::NodeId other) {
				return node_id < other;
			}

			bool operator==(const seam::nodes::NodeId other) {
				return node_id == other;
			}
		};

		EventNodeFactory();

		~EventNodeFactory();


		nodes::INode* Create(seam::nodes::NodeId node_id);

		/// \return the NodeId of the new node to create, or 0 if no new node was requested
		seam::nodes::NodeId DrawCreatePopup(PinType input_type = PinType::NONE, PinType output_type = PinType::NONE);

		/// creates a node and puts its metadata into a generator
		/// \return false if the node id is already registered, otherwise true
		bool Register(CreateFunc&& Create);
	private:

		bool generators_sorted = false;
		std::vector<Generator> generators;
	};
}