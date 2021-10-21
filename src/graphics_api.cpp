//
// Created by thallock on 9/14/21.
//

#include <iostream>
#include <pthread.h>


#include "billiards_common/3rd_party/crow_all.h"
#include "billiards_common/utils/crow_common.h"
#include "billiards_common/config/ports.h"

#include "render.h"


int main(int argc, char **argv) {
	crow::SimpleApp app;

	DO_STATUS_ENDPOINT();

	CROW_ROUTE(app, "/table-boundary/")
		.methods("POST"_method, "OPTIONS"_method)
			([](const crow::request& req) {
				if (req.method == "OPTIONS"_method) {
					HANDLE_OPTIONS;
				} else if (req.method == "POST"_method) {
					nlohmann::json value = nlohmann::json::parse(req.body);

					billiards::config::Table table;
					billiards::json::ParseResult result;
					if (HAS_OBJECT(value, "params") && HAS_OBJECT(value["params"], "table")) {
						table.parse(value["params"]["table"], result);
					} else {
						RETURN_ERROR("No table param provided");
					}
					if (!result.success) {
						std::cerr << "Invalid request:" << std::endl;
						std::cerr << result.error_msg.str() << std::endl;
						RETURN_ERROR("Unable to parse table");
					}

					billiards::utils::DefaultResponse def_resp{
						"Rendered graphics", true, "graphics",
						[&](billiards::json::SaxWriter& writer) {
							writer.begin_array();
							billiards::graphics::render_table_edge(
								table,
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

						billiards::graphics::RenderShotParams params;
						billiards::json::ParseResult status;
						if (HAS_OBJECT(value, "params")) {
							params.parse(value["params"], status);
						} else {
							RETURN_ERROR("No parameters");
						}
						if (!status.success) {
							RETURN_ERROR("Unable to parse params");
						}

						billiards::utils::DefaultResponse def_resp{
							"Rendered graphics", true, "graphics",
							[&params](billiards::json::SaxWriter& writer) {
								writer.begin_array();
								billiards::graphics::render_shot(
									params,
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
