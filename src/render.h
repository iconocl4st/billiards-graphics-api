//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDER_H
#define IDEA_RENDER_H

#include "billiards_common/config/Table.h"
#include "billiards_common/shots/Locations.h"
#include "billiards_common/shots/ShotInformation.h"
#include "billiards_common/shots/shot_helpers.h"

#include "billiards_common/graphics/Lines.h"
#include "billiards_common/graphics/Polygon.h"
#include "billiards_common/graphics/Circle.h"
#include "billiards_common/graphics/Text.h"
#include "billiards_common/graphics/Image.h"

#include "RenderShotParams.h"

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

	inline
	void render_goalpost(
		const geometry::Point& location,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
	) {
		auto primitive = std::make_shared<graphics::Circle>();
		primitive->center = location;
		primitive->radius = 0.25;
		primitive->fill = true;
		primitive->color = graphics::Color{0, 255, 0, 255};
		primitive->priority = priority::GOAL_POST;
		receiver(primitive);
	}
	void render_ghost_ball(
		const geometry::Point& location,
		const config::BallInfo& ball_type,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
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
		const config::Table& table,
		const layout::Locations& locations,
		const shots::ShotInformation& info,
		const shots::StepInfo& destination,
		const shots::step_type::ShotStepType shot_type,
		const std::shared_ptr<shots::GoalPostTarget>& target,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
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
		const config::Table& table,
		const layout::Locations& locations,
		const shots::ShotInformation& info,
		const shots::StepInfo& destination,
		const shots::step_type::ShotStepType shot_type,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
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
			bool has_graphics = false;
			auto primitive = std::make_shared<graphics::Lines>();
			const auto aim_dir = destination.rolling_calculation->aim_dir;
			const auto aim_dir_norm = aim_dir.norm();
			if (aim_dir_norm > TOLERANCE) {
				primitive->append(
					destination.rolling_calculation->loc,
					destination.rolling_calculation->loc + aim_dir * 5 / aim_dir_norm);
				has_graphics = true;
			}
			const auto tan_dir = destination.rolling_calculation->tan_dir;
			const auto tan_dir_norm = tan_dir.norm();
			if (tan_dir_norm > TOLERANCE) {
				primitive->append(
					destination.rolling_calculation->loc,
					destination.rolling_calculation->loc + tan_dir * 5 / tan_dir_norm);
				has_graphics = true;
			}
			primitive->fill = false;
			primitive->line_width = 0.1;
			primitive->color = graphics::Color{255, 0, 0, 255};
			primitive->priority = priority::GLANCE_CALC;
			if (has_graphics) {
				receiver(primitive);
			}
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

	void render_pocket(
		const config::Pocket& pocket,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
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

	void render_ball(
		const layout::LocatedBall& located_ball,
		const config::BallInfo& ball_type,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
	) {
		auto primitive = std::make_shared<graphics::Circle>();
		primitive->center = located_ball.location;
		primitive->radius = ball_type.radius;
		primitive->fill = true;
		primitive->color = ball_type.color;
		primitive->priority = priority::BALL;

		receiver(primitive);
	}

	void render_table_edge(
		const config::Table& table,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
	) {
		auto edges = std::make_shared<graphics::Lines>();
		edges->line_width = 0.1;
		edges->fill = false;
		edges->color = graphics::Color{0, 0, 255, 255};
		edges->append(0, 0, table.dims.width, 0);
		edges->append(table.dims.width, 0, table.dims.width, table.dims.height);
		edges->append(table.dims.width, table.dims.height, 0, table.dims.height);
		edges->append(0, table.dims.height, 0, 0);
		edges->priority = priority::TABLE_EDGE;

		receiver(edges);
	}

	void render_shot_edge(
		const shots::ShotInformation& shot_info,
		int post_index,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
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

	void render_shot(
		const RenderShotParams& params,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
	) {
		render_table_edge(params.table, receiver);

		for (const auto& pocket : params.table.pockets) {
			render_pocket(pocket, receiver);
		}

		render_shot_edge(params.shot_info, 0, receiver);
		render_shot_edge(params.shot_info, 2, receiver);

		for (const auto& destination : params.shot_info.infos) {
			render_destination(
				params.table, params.locations, params.shot_info,
				destination, params.shot_info.get_shot_type(destination), receiver);
		}

		int index = 0;
		for (const auto& located_ball : params.locations.balls) {
			const auto* ball_type = shots::get_ball_type(params.table, params.locations, index);
			render_ball(located_ball, *ball_type, receiver);
			index++;
		}
	}
}
#endif //IDEA_RENDER_H
