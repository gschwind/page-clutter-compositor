/*
 * seat.hxx
 *
 *  Created on: 21 déc. 2016
 *      Author: gschwind
 */

#ifndef SRC_SEAT_HXX_
#define SRC_SEAT_HXX_

#include "wayland-interface.hxx"

namespace page {
namespace wl {

using namespace wcxx;

struct wl_seat : private wl_seat_vtable {
	wl_seat(struct wl_client *client, void *data, uint32_t version, uint32_t id);
	virtual ~wl_seat();

	/* wl_seat_vtable */
	virtual void recv_get_pointer(struct wl_client * client, struct wl_resource * resource, uint32_t id) override;
	virtual void recv_get_keyboard(struct wl_client * client, struct wl_resource * resource, uint32_t id) override;
	virtual void recv_get_touch(struct wl_client * client, struct wl_resource * resource, uint32_t id) override;
	virtual void recv_release(struct wl_client * client, struct wl_resource * resource) override;
	virtual void delete_resource(struct wl_resource * resource) override;
};

}
}


#endif /* SRC_SEAT_HXX_ */
