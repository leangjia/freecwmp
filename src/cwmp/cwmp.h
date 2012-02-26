/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011 Luka Perkov <freecwmp@lukaperkov.net>
 */

#ifndef _FREECWMP_CWMP_H__
#define _FREECWMP_CWMP_H__

#include <libubox/uloop.h>

enum cwmp_event_code 
{ 
 	BOOTSTRAP = 0, 
	BOOT, 
	PERIODIC, 
	SCHEDULED, 
	VALUE_CHANGE, 
	KICKED, 
	CONNECTION_REQUEST, 
	TRANSFER_COMPLETE, 
	DIAGNOSTICS_COMPLETE, 
	REQUEST_DOWNLOAD, 
	AUTONOMOUS_TRANSFER_COMPLETE 
}; 
											 
static struct cwmp
{
	enum cwmp_event_code event_code;
	struct uloop_timeout connection_request_t;
	struct uloop_timeout periodic_inform_t;
	int8_t periodic_inform_enabled;
	int64_t periodic_inform_interval;
	struct uloop_timeout acs_error_t;
	int8_t retry_count;
	int8_t acs_reload_required;
	int8_t acs_connection_required;
} cwmp;

int8_t cwmp_init(void);
int8_t cwmp_exit(void);
int8_t cwmp_reload_http_client(void);
int8_t cwmp_reload_xml(void);
int8_t cwmp_periodic_inform(void);
int8_t cwmp_inform(void);
int8_t cwmp_handle_messages(void);
int8_t cwmp_connection_request(void);
int8_t cwmp_set_parameter_handler(char *name, char *value);
int8_t cwmp_get_parameter_handler(char *name, char **value);
int8_t cwmp_download_handler(char *url, char *size);
int8_t cwmp_reboot_handler(void);
int8_t cwmp_factory_reset_handler(void);
int8_t cwmp_reload_changes(void);
char * cwmp_get_event_code(void);
int cwmp_get_retry_count(void);

#endif
