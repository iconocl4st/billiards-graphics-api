//
// Created by thallock on 9/19/21.
//

#ifndef IDEA_RENDER_H
#define IDEA_RENDER_H

#include "common/config/Table.h"
#include "./RenderParams.h"
#include "common/graphics/GraphicsPrimitive.h"

namespace billiards::graphics {
	void render_table(
		const RenderParams& params,
		const config::Table& table,
		std::function<void(std::shared_ptr<GraphicsPrimitive>)> receiver
	) {

		for (const auto& pocket : table.pockets) {

		}
	}
}
#endif //IDEA_RENDER_H
