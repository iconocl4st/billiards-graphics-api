//
// Created by thallock on 9/14/21.
//

#include <iostream>
#include <pthread.h>


#include "common/3rd_party/crow_all.h"
#include "common/utils/crow_common.h"
#include "common/config/ports.h"

#include "./render.h"


int main(int argc, char **argv) {
	crow::SimpleApp app;

	DO_STATUS_ENDPOINT();

	CROW_ROUTE(app, "/table/")
		.methods("POST"_method, "OPTIONS"_method)
			([](const crow::request& req) {
				if (req.method == "OPTIONS"_method) {
					HANDLE_OPTIONS;
				} else if (req.method == "POST"_method) {
					nlohmann::json value = nlohmann::json::parse(req.body);

					billiards::graphics::RenderParams params;
					if (value.contains("params") && value["params"].is_object()) {
						params.parse(value["params"]);
					}

					billiards::config::Table table;
					if (value.contains("table") && value["table"].is_object()) {
						table.parse(value["table"]);
					}

					billiards::utils::DefaultResponse def_resp{
						"Rendered graphics", true, "graphics",
						[&params, &table](billiards::json::SaxWriter& writer) {
							writer.begin_array();
							billiards::graphics::render_table(
								params, table,
								[&writer](std::shared_ptr<const billiards::graphics::GraphicsPrimitive> ptr) {
									ptr->to_json(writer);
								});
							writer.end_array();
						}};

					crow::response resp{billiards::json::dump(def_resp)};
					resp.set_header("Content-Type", "application/json");
					resp.set_header("Mime-type", "application/json");
					resp.add_header("Access-Control-Allow-Origin", "*");
					return resp;
				} else {
					return crow::response(404);
				}
			});

	CROW_ROUTE(app, "/shot-info/")
		.methods("POST"_method, "OPTIONS"_method)
			([](const crow::request& req) {
				if (req.method == "OPTIONS"_method) {
					HANDLE_OPTIONS;
				} else if (req.method == "POST"_method) {
					try {
						nlohmann::json value = nlohmann::json::parse(req.body);

						billiards::graphics::RenderParams params;
						if (value.contains("params") && value["params"].is_object()) {
							params.parse(value["params"]);
						}

						billiards::config::Table table;
						if (value.contains("table") && value["table"].is_object()) {
							table.parse(value["table"]);
						}

						billiards::layout::Locations locations;
						if (value.contains("locations") && value["locations"].is_object()) {
							locations.parse(value["locations"]);
						}

						billiards::shots::ShotInformation shot_info;
						if (value.contains("shot-info") && value["shot-info"].is_object()) {
							shot_info.parse(value["shot-info"]);
						}

						billiards::utils::DefaultResponse def_resp{
							"Rendered graphics", true, "graphics",
							[&params, &table, &locations, &shot_info](billiards::json::SaxWriter& writer) {
								writer.begin_array();
								billiards::graphics::render_shot(
									params, table, locations, shot_info,
									[&writer](const std::shared_ptr<billiards::graphics::GraphicsPrimitive>& ptr) {
										ptr->to_json(writer);
									});
								writer.end_array();
							}};

						crow::response resp{billiards::json::dump(def_resp)};
						resp.set_header("Content-Type", "application/json");
						resp.set_header("Mime-type", "application/json");
						resp.add_header("Access-Control-Allow-Origin", "*");
						return resp;
					} catch (std::exception& e) {
						std::cerr << e.what();
					}
					RETURN_ERROR("Unable to render shot");
				} else {
					return crow::response(404);
				}
			});

	app.port(billiards::config::ports::GRAPHICS_API_PORT).run();
	return 0;
}
