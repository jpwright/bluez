/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2014  Intel Corporation. All rights reserved.
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/* Control PDU ids */
#define AVRCP_GET_CAPABILITIES		0x10
#define AVRCP_LIST_PLAYER_ATTRIBUTES	0X11
#define AVRCP_LIST_PLAYER_VALUES	0x12
#define AVRCP_GET_CURRENT_PLAYER_VALUE	0x13
#define AVRCP_SET_PLAYER_VALUE		0x14
#define AVRCP_GET_PLAYER_ATTRIBUTE_TEXT	0x15
#define AVRCP_GET_PLAYER_VALUE_TEXT	0x16
#define AVRCP_DISPLAYABLE_CHARSET	0x17
#define AVRCP_CT_BATTERY_STATUS		0x18
#define AVRCP_GET_ELEMENT_ATTRIBUTES	0x20
#define AVRCP_GET_PLAY_STATUS		0x30
#define AVRCP_REGISTER_NOTIFICATION	0x31
#define AVRCP_REQUEST_CONTINUING	0x40
#define AVRCP_ABORT_CONTINUING		0x41
#define AVRCP_SET_ABSOLUTE_VOLUME	0x50
#define AVRCP_SET_BROWSED_PLAYER	0x70
#define AVRCP_GET_FOLDER_ITEMS		0x71
#define AVRCP_CHANGE_PATH		0x72
#define AVRCP_GET_ITEM_ATTRIBUTES	0x73
#define AVRCP_PLAY_ITEM			0x74
#define AVRCP_SEARCH			0x80
#define AVRCP_ADD_TO_NOW_PLAYING	0x90
#define AVRCP_GENERAL_REJECT		0xA0

/* Notification events */
#define AVRCP_EVENT_STATUS_CHANGED		0x01
#define AVRCP_EVENT_TRACK_CHANGED		0x02
#define AVRCP_EVENT_TRACK_REACHED_END		0x03
#define AVRCP_EVENT_TRACK_REACHED_START		0x04
#define AVRCP_EVENT_PLAYBACK_POS_CHANGED	0x05
#define AVRCP_EVENT_SETTINGS_CHANGED		0x08
#define AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED	0x0a
#define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED	0x0b
#define AVRCP_EVENT_UIDS_CHANGED		0x0c
#define AVRCP_EVENT_VOLUME_CHANGED		0x0d
#define AVRCP_EVENT_LAST			AVRCP_EVENT_VOLUME_CHANGED

/* Status codes */
#define AVRCP_STATUS_INVALID_COMMAND		0x00
#define AVRCP_STATUS_INVALID_PARAM		0x01
#define AVRCP_STATUS_PARAM_NOT_FOUND		0x02
#define AVRCP_STATUS_INTERNAL_ERROR		0x03
#define AVRCP_STATUS_SUCCESS			0x04
#define AVRCP_STATUS_OUT_OF_BOUNDS		0x0b
#define AVRCP_STATUS_INVALID_PLAYER_ID		0x11
#define AVRCP_STATUS_PLAYER_NOT_BROWSABLE	0x12
#define AVRCP_STATUS_NO_AVAILABLE_PLAYERS	0x15
#define AVRCP_STATUS_ADDRESSED_PLAYER_CHANGED	0x16

/* Capabilities for AVRCP_GET_CAPABILITIES pdu */
#define CAP_COMPANY_ID				0x02
#define CAP_EVENTS_SUPPORTED			0x03

/* Player Attributes */
#define AVRCP_ATTRIBUTE_ILEGAL			0x00
#define AVRCP_ATTRIBUTE_EQUALIZER		0x01
#define AVRCP_ATTRIBUTE_REPEAT_MODE		0x02
#define AVRCP_ATTRIBUTE_SHUFFLE			0x03
#define AVRCP_ATTRIBUTE_SCAN			0x04
#define AVRCP_ATTRIBUTE_LAST			AVRCP_ATTRIBUTE_SCAN

/* Company IDs for vendor dependent commands */
#define IEEEID_BTSIG		0x001958

struct avrcp;

struct avrcp_control_handler {
	uint8_t id;
	uint8_t code;
	uint8_t rsp;
	ssize_t (*func) (struct avrcp *session, uint8_t transaction,
			uint16_t params_len, uint8_t *params, void *user_data);
};

struct avrcp_passthrough_handler {
	uint8_t op;
	bool (*func) (struct avrcp *session, bool pressed, void *user_data);
};

typedef void (*avrcp_destroy_cb_t) (void *user_data);

static inline uint32_t ntoh24(const uint8_t src[3])
{
	return src[0] << 16 | src[1] << 8 | src[2];
}

static inline void hton24(uint8_t dst[3], uint32_t src)
{
	dst[0] = (src & 0xff0000) >> 16;
	dst[1] = (src & 0x00ff00) >> 8;
	dst[2] = (src & 0x0000ff);
}

struct avrcp *avrcp_new(int fd, size_t imtu, size_t omtu, uint16_t version);
void avrcp_shutdown(struct avrcp *session);
void avrcp_set_destroy_cb(struct avrcp *session, avrcp_destroy_cb_t cb,
							void *user_data);
void avrcp_set_control_handlers(struct avrcp *session,
				const struct avrcp_control_handler *handlers,
				void *user_data);
void avrcp_set_passthrough_handlers(struct avrcp *session,
			const struct avrcp_passthrough_handler *handlers,
			void *user_data);
int avrcp_init_uinput(struct avrcp *session, const char *name,
							const char *address);
int avrcp_send(struct avrcp *session, uint8_t transaction, uint8_t code,
					uint8_t subunit, uint8_t pdu_id,
					uint8_t *params, size_t params_len);
int avrcp_get_capabilities(struct avrcp *session, uint8_t param,
					avctp_rsp_cb func, void *user_data);
int avrcp_register_notification(struct avrcp *session, uint8_t event,
					uint32_t interval, avctp_rsp_cb func,
					void *user_data);
int avrcp_list_player_attributes(struct avrcp *session, avctp_rsp_cb func,
							void *user_data);
int avrcp_get_player_attribute_text(struct avrcp *session, uint8_t *attributes,
					uint8_t attr_len, avctp_rsp_cb func,
					void *user_data);
int avrcp_set_player_value(struct avrcp *session, uint8_t *attributes,
					uint8_t attr_count, uint8_t *values,
					avctp_rsp_cb func, void *user_data);
int avrcp_get_current_player_value(struct avrcp *session, uint8_t *attrs,
					uint8_t attr_count, avctp_rsp_cb func,
					void *user_data);
int avrcp_get_play_status(struct avrcp *session, avctp_rsp_cb func,
							void *user_data);

int avrcp_get_play_status_rsp(struct avrcp *session, uint8_t transaction,
				uint32_t position, uint32_t duration,
				uint8_t status);
int avrcp_get_element_attrs_rsp(struct avrcp *session, uint8_t transaction,
					uint8_t *params, size_t params_len);
int avrcp_register_notification_rsp(struct avrcp *session, uint8_t transaction,
					uint8_t code, uint8_t *params,
					size_t params_len);
