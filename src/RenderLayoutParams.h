//
// Created by thallock on 11/13/21.
//

#ifndef IDEA_RENDERLAYOUTPARAMS_H
#define IDEA_RENDERLAYOUTPARAMS_H


#include "billiards_common/layouts/DetailedLayout.h"
#include "RenderShotOptions.h"

namespace billiards::graphics {
	class RenderLayoutParams : public json::Serializable {
	public:
		layout::DetailedLayout layout;
		RenderShotOptions options;

		RenderLayoutParams() = default;
		~RenderLayoutParams() override = default;

		void to_json(json::SaxWriter& writer) const override {
			writer.begin_object();
			writer.key("layout");
			layout.to_json(writer);
			writer.key("options");
			options.to_json(writer);
			writer.end_object();
		};

		void parse(const nlohmann::json& value, json::ParseResult& status) override {
			REQUIRE_CHILD(status, value, "layout", layout, "Must have a table to render");
			REQUIRE_CHILD(status, value, "options", options, "Must have a options");
		};
	};
}
#endif //IDEA_RENDERLAYOUTPARAMS_H
