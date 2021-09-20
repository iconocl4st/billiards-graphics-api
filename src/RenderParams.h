//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDERPARAMS_H
#define IDEA_RENDERPARAMS_H

#include "common/utils/Serializable.h"

namespace billiards::gphx {
	class RenderParams : public json::Serializable {
	public:

		void to_json(json::SaxWriter& writer) const override {
			writer.begin_object();
			writer.end_object();
		};

		void parse(const nlohmann::json& value) override {

		};
	};
}

#endif //IDEA_RENDERPARAMS_H
