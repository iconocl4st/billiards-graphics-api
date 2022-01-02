//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDERSHOTPARAMS_H
#define IDEA_RENDERSHOTPARAMS_H

#include "billiards_common/shots/ShotInformation.h"
#include "billiards_common/config/PoolConfiguration.h"
#include "RenderShotOptions.h"

namespace billiards::graphics {
	class RenderShotParams : public json::Serializable {
	public:
		config::PoolConfiguration table;
		billiards::layout::Locations locations;
		// This could just be the shot...
		billiards::shots::ShotInformation shot_info;
		graphics::RenderShotOptions options;

		RenderShotParams() = default;
		~RenderShotParams() override = default;

		void to_json(json::SaxWriter& writer) const override {
			writer.begin_object();
			writer.key("table");
			table.to_json(writer);
			writer.key("options");
			options.to_json(writer);
			writer.end_object();
		};

		void parse(const nlohmann::json& value, json::ParseResult& status) override {
			REQUIRE_CHILD(status, value, "table", table, "Must have a table to render");
			REQUIRE_CHILD(status, value, "locations", locations, "Must have a table to render");
			REQUIRE_CHILD(status, value, "shot-info", shot_info, "Must have a table to render");
			REQUIRE_CHILD(status, value, "options", options, "Must have a options");
		};
	};
}

#endif //IDEA_RENDERSHOTPARAMS_H
