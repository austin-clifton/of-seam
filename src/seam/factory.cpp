#include "factory.h"
#include "hash.h"

#include "nodes/cos.h"
#include "nodes/midi-in.h"
#include "nodes/notes-printer.h"
#include "nodes/shader.h"
#include "nodes/texgen-perlin.h"
#include "nodes/timer.h"
#include "nodes/force-grid.h"
#include "nodes/compute-particles.h"
#include "nodes/percussive-trigger.h"

using namespace seam;

namespace {
	template <typename T>
	EventNodeFactory::CreateFunc MakeCreate() {
		return [] {
			return new T();
		};
	}
}

EventNodeFactory::EventNodeFactory() {
	// register seam-internal nodes here
	Register(MakeCreate<nodes::ComputeParticles>());
	Register(MakeCreate<nodes::Cos>());
	Register(MakeCreate<nodes::ForceGrid>());
	Register(MakeCreate<nodes::MidiIn>());
	Register(MakeCreate<nodes::NotesPrinter>());
	Register(MakeCreate<nodes::PercussiveTrigger>());
	Register(MakeCreate<nodes::Shader>());
	Register(MakeCreate<nodes::TexgenPerlin>());
	Register(MakeCreate<nodes::Timer>());

	// TODO register more seam internal generators here
}

EventNodeFactory::~EventNodeFactory() {
	// nothing to do?
}

nodes::INode* EventNodeFactory::Create(seam::nodes::NodeId node_id) {
	if (!generators_sorted) {
		std::sort(generators.begin(), generators.end());
		generators_sorted = true;
	}
	// find the generator
	auto it = std::lower_bound(generators.begin(), generators.end(), node_id);
	if (it != generators.end() && it->node_id == node_id) {
		nodes::INode* node = it->Create();
		assert(node);

		// make sure each input pin has this node set as its parent
		size_t size;
		IPinInput** pin_inputs = node->PinInputs(size);
		for (size_t i = 0; i < size; i++) {
			pin_inputs[i]->node = node;
		}

		return node;
	} else {
		printf("node with node id %u not found, did you register it?", node_id);
		return nullptr;
	}
}

bool EventNodeFactory::Register(EventNodeFactory::CreateFunc&& Create) {
	Generator gen;
	// TODO it'd be nice to stack alloc dummy nodes instead but probably doesn't matter that much;
	// this should only run once per node type
	std::unique_ptr<nodes::INode> n(Create());
	gen.node_name = n->NodeName();
	gen.node_id = SCHash(gen.node_name.data(), gen.node_name.length());

	// make sure it's not already registered before continuing
	if (std::find(generators.begin(), generators.end(), gen.node_id) != generators.end()) {
		// node id already registered, probably a bug
		printf("node with name %s and id %u already registered\n", 
			gen.node_name.data(), gen.node_id);
		return false;
	}
	
	size_t size;

	// read inputs
	{
		IPinInput** inputs = n->PinInputs(size);
		for (size_t i = 0; i < size; i++) {
			PinType type = inputs[i]->type;
			// filter only for types that aren't already in the list
			if (std::find(gen.pin_inputs.begin(), gen.pin_inputs.end(), type) == gen.pin_inputs.end()) {
				gen.pin_inputs.push_back(type);
			}
		}
	}

	// read outputs
	{
		PinOutput* outputs = n->PinOutputs(size);
		for (size_t i = 0; i < size; i++) {
			PinType type = outputs[i].pin.type;
			if (std::find(gen.pin_outputs.begin(), gen.pin_outputs.end(), type) == gen.pin_outputs.end()) {
				gen.pin_outputs.push_back(type);
			}
		}
	}

	gen.Create = std::move(Create);
	generators.push_back(std::move(gen));
	return true;
}

seam::nodes::NodeId EventNodeFactory::DrawCreatePopup(PinType input_type, PinType output_type) {
	if (!generators_sorted) {
		std::sort(generators.begin(), generators.end());
	}

	// loop through each Generator that's been registered with the EventNodeFactory,
	// and make a selectable menu item for it
	nodes::NodeId new_node_id = 0;
	for (auto gen : generators) {
		// TODO filter input / output types, only enable items which match the filter

		// MenuItem() will return true if this menu item was selected
		if (ImGui::MenuItem(gen.node_name.data())) {
			new_node_id = gen.node_id;
		}
	}

	return new_node_id;
}
