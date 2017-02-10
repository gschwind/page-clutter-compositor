/*
 * seat.hxx
 *
 *  Created on: 21 déc. 2016
 *      Author: gschwind
 */

#ifndef SRC_SEAT_HXX_
#define SRC_SEAT_HXX_

#include "config.hxx"
#include <list>
#include <memory>

#include "wayland-interface.hxx"

#include "libpage/page-types.hxx"
#include "wl-types.hxx"

namespace page {
namespace wl {

using namespace std;
using namespace wayland_cxx_wrapper;

struct wl_seat : private wl_seat_vtable {
	page_seat * seat;

	list<shared_ptr<wl_pointer>> pointers;
	list<shared_ptr<wl_keyboard>> keyboards;
	list<shared_ptr<wl_touch>> touchs;

	wl_seat(page_seat * seat, struct wl_client *client, uint32_t version, uint32_t id);
	virtual ~wl_seat();

	static wl_seat * get(struct wl_resource * r);

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
