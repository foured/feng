#include "collision_data.h"

#include "../logging/logging.h"

namespace feng::phys {

	// COLLISION_CONTACT------------------------------------------------------------------------------------------------

	uint32_t collision_contact::type_to_int() const {
		return static_cast<int32_t>(type);
	}

	collision_contact collision_contact::overlap(const collision_contact& c1, const collision_contact& c2) {
		// poins are points
		if (c1.type == collision_contact_type::point) {
			return c1;
		}

		if (c2.type == collision_contact_type::point) {
			return c2;
		}

		// edge - edge
		collision_contact cc;
		if (c1.type == collision_contact_type::edge && c2.type == collision_contact_type::edge) {
			LOG_WARNING("Untested part. If not working, try unkoment edge-cross part in collides_shape");
			// mb have to add penetration
			cc.type = collision_contact_type::edge;
			cc.data = edge::overlap(std::get<edge>(c1.data), std::get<edge>(c1.data));
			return cc;
		}

		// edge - polygon
		if (c1.type == collision_contact_type::edge && c2.type == collision_contact_type::polygon) {
			return overlap_edge_and_polygon(c1, c2);
		}

		if (c1.type == collision_contact_type::polygon && c2.type == collision_contact_type::edge) {
			return overlap_edge_and_polygon(c2, c1);
		}

		// both polygons
		std::optional<polygon> rp = c1.get<polygon>().sutherland_hodgman_clip(c2.get<polygon>());

		if (rp.has_value()) {
			cc.data = rp.value();
			cc.type = collision_contact_type::polygon;
		}
		else {
			THROW_ERROR("Polygons are not overlapping.");
		}

		return cc;
	}

	collision_contact collision_contact::overlap_edge_and_polygon(const collision_contact& ed,
		const collision_contact& pl) {
		std::optional<edge> res = std::get<polygon>(pl.data).cyrus_beck_clip(std::get<edge>(ed.data), 1.0f / 100000.0f);

		collision_contact cc;

		if (res.has_value()) {
			cc.data = res.value();
			cc.type = collision_contact_type::edge;
		}
		else {
			THROW_ERROR("Edge is not overlapping with polygon.");
		}

		return cc;
	}

	// COLLISION DATA---------------------------------------------------------------------------------------------------

	void collision_data::invert() {
		axis *= -1;
	}

}