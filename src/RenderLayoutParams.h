//
// Created by thallock on 11/13/21.
//

#ifndef IDEA_RENDERLAYOUTPARAMS_H
#define IDEA_RENDERLAYOUTPARAMS_H

#include "billiards_common/layouts/Layout.h"
#include "billiards_common/shots/ShotInformation.h"

namespace billiards::graphics {
	class RenderLayoutParams : public json::Serializable {
	public:
		config::Table table;
		layout::Layout layout;
		std::vector<shots::ShotInformation> infos;

		RenderLayoutParams() = default;
		~RenderLayoutParams() override = default;

		void to_json(json::SaxWriter& writer) const override {
			writer.begin_object();
			writer.key("table");
			table.to_json(writer);
			writer.key("layout");
			layout.to_json(writer);
			writer.key("infos");
			writer.begin_array();
			for (const auto& info : infos) {
				info.to_json(writer);
			}
			writer.end_array();
			writer.end_object();
		};

		void parse(const nlohmann::json& value, json::ParseResult& status) override {
			REQUIRE_CHILD(status, value, "table", table, "Must have a table to render");
			REQUIRE_CHILD(status, value, "layout", layout, "Must have a layout");
			ENSURE_ARRAY(status, value, "infos", "Must have the shot infos");
			for (const auto& elem : value["infos"]) {
				infos.emplace_back();
				infos.back().parse(elem, status);
				if (!status.success) {
					return;
				}
			}
		};
	};
}
#endif //IDEA_RENDERLAYOUTPARAMS_H
