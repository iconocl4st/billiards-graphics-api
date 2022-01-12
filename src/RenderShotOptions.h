//
// Created by thallock on 11/20/21.
//

#ifndef IDEA_RENDERSHOTOPTIONS_H
#define IDEA_RENDERSHOTOPTIONS_H

#include "billiards_common/utils/Serializable.h"

namespace billiards::graphics {

	class RenderShotOptions : public json::Serializable {
	public:
		bool draw_lines;
		bool use_dots;
		bool show_labels;

		RenderShotOptions(
			bool draw_lines,
			bool use_dots,
			bool show_labels
		)
			: draw_lines{draw_lines}
			, use_dots{use_dots}
			, show_labels{show_labels}
		{}

		RenderShotOptions() : RenderShotOptions{true, false, true} {}

		~RenderShotOptions() override = default;

		void parse(const nlohmann::json& value, json::ParseResult& result) override {
			if (HAS_BOOL(value, "draw-lines")) {
				draw_lines = value["draw-lines"].get<bool>();
			} else {
				draw_lines = true;
			}
			if (HAS_BOOL(value, "use-dots")) {
				use_dots = value["use-dots"].get<bool>();
			} else {
				use_dots = false;
			}
			if (HAS_BOOL(value, "show-labels")) {
				show_labels = value["show-labels"].get<bool>();
			} else {
				show_labels = false;
			}
		}

		void to_json(json::SaxWriter& writer) const override {
			writer.begin_object();
			writer.field("draw-lines", draw_lines);
			writer.field("use-dots", use_dots);
			writer.field("show-labels", use_dots);
			writer.end_object();
		}
	};
}

#endif //IDEA_RENDERSHOTOPTIONS_H
