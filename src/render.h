//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDER_H
#define IDEA_RENDER_H

#include "common/config/Table.h"
#include "common/shots/Locations.h"
#include "common/shots/ShotInformation.h"
#include "common/shots/shot_helpers.h"

#include "common/graphics/Lines.h"
#include "common/graphics/Polygon.h"
#include "common/graphics/Circle.h"
#include "common/graphics/Text.h"
#include "common/graphics/Image.h"

#include "./RenderShotParams.h"

namespace billiards::graphics {

	namespace priority {
		constexpr int TABLE_EDGE = -10;
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
	void render_destination(
		const config::Table& table,
		const layout::Locations& locations,
		const shots::ShotInformation& info,
		const shots::Destination& destination,
		const shots::step_type::ShotStepType shot_type,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
	) {
		switch (destination.target->type) {
			case shots::target_type::GOAL_POST: {
				const auto& goal_posts = std::dynamic_pointer_cast<shots::GoalPostTarget>(destination.target);
				render_goalpost(goal_posts->goal_post_1.point(), receiver);
				render_goalpost(goal_posts->goal_post_2.point(), receiver);

				switch (shot_type) {
					case shots::step_type::RAIL:
					case shots::step_type::STRIKE: {
						const auto* ball_type = shots::get_ball_type(table, locations, locations.cue_ball_index());
						render_ghost_ball(goal_posts->goal_post_center.point(), *ball_type, receiver);
					}
					default:
						break;
				}
				break;
			}
			case shots::target_type::UNKNOWN:
			default:
				return;
		}

		switch (shot_type) {
			case shots::step_type::RAIL: {
				const auto& rail_step = info.get_typed_step<shots::RailStep>(destination);
				const auto& rail = table.rail(rail_step->rail);

				auto primitive = std::make_shared<graphics::Lines>();
				primitive->append(rail.segment1, rail.segment2);
				primitive->fill = false;
				primitive->line_width = 0.5;
				primitive->color = graphics::Color{255, 255, 0, 255};
				primitive->priority = priority::GHOST_BALL;
				receiver(primitive);


//				const auto rail_line = geometry::through(s1, s2);
//				const auto reflection = geometry::reflect(src, rail_line);
//				const auto travel_line = geometry::through(src, reflection);
//				const auto bank_line = geometry::parallel_at(rail_line, s1 + in * radius);
//				const auto intersection = geometry::intersection(bank_line, travel_line);
//				return intersection;
				break;
			}
			default:
				break;
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
		bool first_post,
		const std::function<void(const std::shared_ptr<GraphicsPrimitive>&)>& receiver
	) {
		auto primitive = std::make_shared<graphics::Lines>();
		primitive->line_width = 0.1;
		primitive->fill = false;
		primitive->color = graphics::Color{0, 255, 255, 255};
		primitive->priority = priority::SHOT_EDGE;

		geometry::Point previous = shot_info.cueing.cue_location.point();
		for (const auto & destination : shot_info.destinations) {
			geometry::Point current;
			const auto& target = destination.target;
			switch (target->type) {
				case shots::target_type::GOAL_POST: {
					const auto& goal_posts = std::dynamic_pointer_cast<shots::GoalPostTarget>(target);
					if (first_post) {
						current = goal_posts->goal_post_1.point();
					} else {
						current = goal_posts->goal_post_2.point();
					}
					break;
				}
				case shots::target_type::UNKNOWN:
				default:
					throw std::runtime_error{"Not implemented"};
			}

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

		render_shot_edge(params.shot_info, true, receiver);
		render_shot_edge(params.shot_info, false, receiver);

		for (const auto& destination : params.shot_info.destinations) {
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
