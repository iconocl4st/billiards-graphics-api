//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDERSHOTPARAMS_H
#define IDEA_RENDERSHOTPARAMS_H

#include "common/shots/ShotInformation.h"

namespace billiards::graphics {
	class RenderShotParams : public json::Serializable {
	public:
		config::Table table;
		billiards::layout::Locations locations;
		billiards::shots::ShotInformation shot_info;

		RenderShotParams() = default;
		~RenderShotParams() = default;

		void to_json(json::SaxWriter& writer) const override {
			writer.begin_object();
			writer.key("table");
			table.to_json(writer);
			writer.end_object();
		};

		void parse(const nlohmann::json& value, json::ParseResult& status) override {
			REQUIRE_CHILD(status, value, "table", table, "Must have a table to render");
			REQUIRE_CHILD(status, value, "locations", locations, "Must have a table to render");
			REQUIRE_CHILD(status, value, "shot-info", shot_info, "Must have a table to render");
		};
	};
}

#endif //IDEA_RENDERSHOTPARAMS_H
