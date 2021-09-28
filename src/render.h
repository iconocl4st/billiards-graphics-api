//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDER_H
#define IDEA_RENDER_H

#include "common/config/Table.h"
#include "common/shots/Locations.h"
#include "common/shots/ShotInformation.h"

#include "common/graphics/Lines.h"
#include "common/graphics/Polygon.h"
#include "common/graphics/Circle.h"
#include "common/graphics/Text.h"
#include "common/graphics/Image.h"

#include "./RenderParams.h"

namespace billiards::graphics {
	void render_table(
		const RenderParams& params,
		const config::Table& table,
		std::function<void(std::shared_ptr<GraphicsPrimitive>)> receiver
	) {

		for (const auto& pocket : table.pockets) {

		}
	}

	void render_shot(
		const RenderParams& params,
		const config::Table& table,
		const layout::Locations& locations,
		const shots::ShotInformation& shotInfo,
		std::function<void(std::shared_ptr<GraphicsPrimitive>)> receiver
	) {
		// Drawing simple graphics for now...

		for (const auto& located_ball : locations.balls) {
			auto primitive = std::make_shared<graphics::Circle>();
			primitive->center = located_ball.location;
			primitive->radius = table.balls[0].radius;
			primitive->fill = true;
			primitive->color = graphics::Color{255, 255, 255, 255};
			primitive->priority = -1;

			receiver(primitive);
		}
	}
}
#endif //IDEA_RENDER_H
