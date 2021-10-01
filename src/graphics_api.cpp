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

#if 0
	CROW_ROUTE(app, "/table/")
		.methods("POST"_method, "OPTIONS"_method)
			([](const crow::request& req) {
				if (req.method == "OPTIONS"_method) {
					HANDLE_OPTIONS;
				} else if (req.method == "POST"_method) {
					nlohmann::json value = nlohmann::json::parse(req.body);

					billiards::graphics::RenderShotParams params;
					billiards::json::ParseResult result;
					if (HAS_OBJECT(value, "params")) {
						params.parse(value["params"], result);
					} else {
						RETURN_ERROR("No params provided");
					}
					if (result.success) {
						RETURN_ERROR("Unable to parse params");
					}

					billiards::utils::DefaultResponse def_resp{
						"Rendered graphics", true, "graphics",
						[&params](billiards::json::SaxWriter& writer) {
							writer.begin_array();
							billiards::graphics::render_table(
								params,
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
#endif

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
