//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDER_H
#define IDEA_RENDER_H

#include "billiards_common/shots/Locations.h"
#include "billiards_common/shots/ShotInformation.h"
#include "billiards_common/shots/shot_calculation/shot.h"
#include "billiards_common/shots/shot_helpers.h"

#include "billiards_common/graphics/Lines.h"
#include "billiards_common/graphics/Polygon.h"
#include "billiards_common/graphics/Circle.h"
#include "billiards_common/graphics/Text.h"
#include "billiards_common/graphics/Image.h"

//#include "billiards_common/config/RenderLocation.h"

#include "RenderShotParams.h"
#include "RenderLayoutParams.h"

namespace billiards::graphics {

	namespace priority {
		constexpr int TABLE_EDGE = -10;
		constexpr int GLANCE_CALC = -5;
		constexpr int POCKET = 0;
		constexpr int SHOT_EDGE = 10;
		constexpr int GHOST_BALL = 11;
		constexpr int GOAL_POST = 20;
		constexpr int BALL = 30;
	}

	typedef std::function<void(const std::shared_ptr<GraphicsPrimitive>&)> GraphicsReceiver;

	inline
	void render_goalpost(const geometry::Point& location, const GraphicsReceiver& receiver) {
		auto primitive = std::make_shared<graphics::Circle>();
		primitive->center = location;
		primitive->radius = 0.25;
		primitive->fill = true;
		primitive->color = graphics::Color{0, 255, 0, 255};
		primitive->priority = priority::GOAL_POST;
		receiver(primitive);
	}

	inline
	void render_ghost_ball(
		const geometry::Point& location,
		const config::BallInfo& ball_type,
		const GraphicsReceiver& receiver
	) {
		auto primitive = std::make_shared<graphics::Circle>();
		primitive->center = location;
		primitive->radius = ball_type.radius;
		primitive->fill = false;
		primitive->line_width = 0.1;
		primitive->color = graphics::Color{255, 255, 255, 255};
		primitive->priority = priority::GHOST_BALL;

		receiver(primitive);
	}

	inline
	void render_destination_target(
		const config::PoolConfiguration& table,
		const layout::Locations& locations,
		const shots::ShotInformation& info,
		const shots::StepInfo& destination,
		const shots::step_type::ShotStepType shot_type,
		const std::shared_ptr<shots::GoalPostTarget>& target,
		const GraphicsReceiver& receiver
	) {
		render_goalpost(target->posts[0].get(), receiver);
		render_goalpost(target->posts[2].get(), receiver);

		switch (shot_type) {
			case shots::step_type::RAIL:
			case shots::step_type::STRIKE:
			case shots::step_type::KISS:
			{
				const auto* ball_type = shots::get_ball_type(table, locations, locations.cue_ball_index());
				render_ghost_ball(destination.target->posts[1].get(), *ball_type, receiver);
			}
			default:
				break;
		}
	}

	inline
	void render_destination(
		const config::PoolConfiguration& table,
		const layout::Locations& locations,
		const shots::ShotInformation& info,
		const shots::StepInfo& destination,
		const shots::step_type::ShotStepType shot_type,
		const GraphicsReceiver& receiver
	) {
		if (destination.target) {
			render_destination_target(
				table,
				locations,
				info,
				destination,
				shot_type,
				destination.target,
				receiver
			);
		}

		// Render glance calculation
		if (destination.rolling_calculation) {
			auto primitive = std::make_shared<graphics::Lines>();
			primitive->append(
				destination.rolling_calculation->loc,
				destination.rolling_calculation->aim_dir);
			primitive->append(
				destination.rolling_calculation->loc,
				destination.rolling_calculation->tan_dir);
			primitive->fill = false;
			primitive->line_width = 0.1;
			primitive->color = graphics::Color{255, 0, 0, 255};
			primitive->priority = priority::GLANCE_CALC;
			receiver(primitive);
		}

		// Render rail
		if (shot_type == shots::step_type::RAIL) {
			const auto& rail_step = info.get_typed_step<shots::RailStep>(destination);
			const auto& rail = table.rail(rail_step->rail);

			auto primitive = std::make_shared<graphics::Lines>();
			primitive->append(rail.segment1, rail.segment2);
			primitive->fill = false;
			primitive->line_width = 0.5;
			primitive->color = graphics::Color{255, 255, 0, 255};
			primitive->priority = priority::GHOST_BALL;
			receiver(primitive);
		}
	}

	inline
	void render_pocket(
		const config::Pocket& pocket,
		const GraphicsReceiver& receiver
	) {
		auto primitive = std::make_shared<graphics::Polygon>();
		primitive->vertices.emplace_back(pocket.inner_segment_1);
		primitive->vertices.emplace_back(pocket.outer_segment_1);
		primitive->vertices.emplace_back(pocket.outer_segment_2);
		primitive->vertices.emplace_back(pocket.inner_segment_2().point());
		primitive->fill = true;
		primitive->color = graphics::Color{255, 0, 0, 255};
		primitive->priority = priority::POCKET;

		receiver(primitive);
	}

	inline
	void render_ball(
		const layout::LocatedBall& located_ball,
		const config::BallInfo& ball_type,
		const bool use_dots,
		const GraphicsReceiver& receiver
	) {
		auto primitive = std::make_shared<graphics::Circle>();
		primitive->center = located_ball.location;
		if (use_dots) {
			primitive->radius = ball_type.radius / 4;
		} else {
			primitive->radius = ball_type.radius;
		}
		primitive->fill = true;
		primitive->color = ball_type.color;
		primitive->priority = priority::BALL;

		receiver(primitive);
	}

	inline
	void render_dimension(
		const geometry::Dimensions& dims,
		const GraphicsReceiver& receiver
	) {
		auto edges = std::make_shared<graphics::Lines>();
		edges->line_width = 0.1;
		edges->fill = false;
		edges->color = graphics::Color{0, 0, 255, 255};
		edges->append(0, 0, dims.width, 0);
		edges->append(dims.width, 0, dims.width, dims.height);
		edges->append(dims.width, dims.height, 0, dims.height);
		edges->append(0, dims.height, 0, 0);
		edges->priority = priority::TABLE_EDGE;

		receiver(edges);
	}

	inline
	void render_dimension(
		const config::TableDimensions& table,
		const GraphicsReceiver& receiver
	) {
		render_dimension(table.dims(), receiver);
	}

	inline
	void render_table_edge(const config::PoolConfiguration& table, const GraphicsReceiver& receiver) {
		render_dimension(table.dims, receiver);
	}

	inline
	void render_text(
		const geometry::Point& location, const std::string& text,
		const GraphicsReceiver& receiver
	) {
		auto g = std::make_shared<graphics::Text>();
		g->color = graphics::Color{0, 255, 255, 255};
		g->location = location;
		g->text = text;
		g->font_size = 18;
//		std::string font_family;
		receiver(g);
	}

//	inline
//	void render_location(
//		const project::RenderLocation& location,
//		const GraphicsReceiver& receiver
//	) {
//		// TODO
////		render_dimension(location.table_dims, receiver);
////
//////		std::cout << "Original: " << location.of << std::endl;
//////		std::cout << "Unmapped: " << location.unmap(location.of) << std::endl;
//////		std::cout << "Mapped unmapped: " << location.map(location.unmap(location.of)) << std::endl;
////
////		const auto offset = geometry::Point{0, 0};
////		const auto right = geometry::Point{location.table_dims.width, 0};
////		const auto up = geometry::Point{0, location.table_dims.height};
////		const auto alpha = 0.8;
////
////		render_text(location.table_dims.scale_to_center(offset, alpha), "Offset", receiver);
////		render_text(location.table_dims.scale_to_center(right, alpha), "Right", receiver);
////		render_text(location.table_dims.scale_to_center(up, alpha), "Up", receiver);
//	}

	inline
	void render_pocket_configs(
			const config::PoolConfiguration& table,
			const GraphicsReceiver& receiver
	) {
		render_dimension(table.dims, receiver);

		int index = 0;
		for (const auto& pocket : table.pockets) {
			render_pocket(pocket, receiver);

			std::stringstream ss;
			ss << "Pocket " << index;
			render_text(
				table.dims.dims().scale_to_center(pocket.center(), 0.75),
				ss.str(),
				receiver);
			++index;
		}
	}

	inline
	void render_shot_edge(
		const shots::ShotInformation& shot_info,
		int post_index,
		const GraphicsReceiver& receiver
	) {
		auto primitive = std::make_shared<graphics::Lines>();
		primitive->line_width = 0.1;
		primitive->fill = false;
		primitive->color = graphics::Color{0, 255, 255, 255};
		primitive->priority = priority::SHOT_EDGE;

		geometry::Point previous = shot_info.cueing.cue_location;
		for (const auto & destination : shot_info.infos) {
			geometry::Point current;
			if (!destination.target) {
				continue;
			}
			const auto& target = destination.target;
			current = target->posts[post_index].point();
			primitive->segments.emplace_back(std::pair{previous, current});
			previous = current;
		}

		receiver(primitive);
	}

	void render_locations(
		const config::PoolConfiguration& table,
		const layout::Locations& locations,
		const bool use_dots,
		const GraphicsReceiver& receiver
	) {
		int index = 0;
		for (const auto& located_ball : locations.balls) {
			const auto* ball_type = shots::get_ball_type(table, locations, index);
			render_ball(located_ball, *ball_type, use_dots, receiver);
			index++;
		}
	}

	void render_shot_info(
			const RenderShotOptions& options,
			const config::PoolConfiguration& table,
			const layout::Locations& locations,
			const shots::ShotInformation& shot_info,
			const GraphicsReceiver& receiver
	) {
		if (options.draw_lines) {
			render_shot_edge(shot_info, 0, receiver);
			render_shot_edge(shot_info, 2, receiver);
		}

		for (const auto& destination : shot_info.infos) {
			render_destination(
				table, locations, shot_info,
				destination, shot_info.get_shot_type(destination), receiver);
		}
	}

	void render_shot(
		const RenderShotParams& params,
		const GraphicsReceiver& receiver
	) {
		render_table_edge(params.table, receiver);
		for (const auto& pocket : params.table.pockets) {
			render_pocket(pocket, receiver);
		}
		render_shot_info(params.options, params.table, params.locations, params.shot_info, receiver);
		render_locations(params.table, params.locations, params.options.use_dots, receiver);
	}

	void render_layout(
		const RenderLayoutParams& params,
		const GraphicsReceiver& receiver
	) {
		render_table_edge(params.layout.config, receiver);
		for (const auto& pocket : params.layout.config.pockets) {
			render_pocket(pocket, receiver);
		}

		for (const auto& info : params.layout.infos) {
			render_shot_info(params.options, params.layout.config, params.layout.layout.locations, info, receiver);
		}
		render_locations(params.layout.config, params.layout.layout.locations, params.options.use_dots, receiver);
	}
}
#endif //IDEA_RENDER_H
