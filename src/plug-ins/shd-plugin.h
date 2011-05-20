/**
 * The Shadow Simulator
 *
 * Copyright (c) 2010-2011 Rob Jansen <jansen@cs.umn.edu>
 * Copyright (c) 2006-2009 Tyson Malchow <tyson.malchow@gmail.com>
 *
 * This file is part of Shadow.
 *
 * Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHD_PLUGIN_H_
#define SHD_PLUGIN_H_

#include <stdarg.h>
#include <sys/time.h>
#include <netinet/in.h>

#include "log_codes.h"
#include "snricall_codes.h"

/**
 * SNRI R2 - Standard Network Routing Interface Revision 2
 * Loadable module header
 */

/* snricall function pointer */
typedef int (*_snricall_fpref_t)(int, ...);
extern _snricall_fpref_t _snricall_fpref;
#define _snricall (*_snricall_fpref)

/* This is the function pointer that will need to be passed for timer callback. When your
 * timer expires, SNRI will call the function you pass with this signature.
 */
typedef void (*snri_timer_callback_fp)(
			int,  			/* timer id */
			void * 			/* saved argument */
		);

/* signature for the functions used to create a timer */
typedef int (*snri_create_timer_fp)(int, snri_timer_callback_fp, void*);

/**
 * @param t Will be filled with the current system time
 */
int snri_gettime(struct timeval *t);

/**
 * Creates a timer that will expire after the given delay
 *
 * @param milli_delay Number of milliseconds after which this timer will expire
 * @param callback_function The function that will be called when this timer expires
 */
int snri_timer_create(int milli_delay, snri_timer_callback_fp callback_function, void * cb_arg);

/**
 * Destroys the timer with the given ID, preventing it from executing
 */
int snri_timer_destroy(int timer_id);

/**
 * Schedules this node for deletion
 */
int snri_exit(void);

/**
 * Logs some message at the given log_level. Will only be printed if logged
 * at a lower level than the configured system log level.
 *
 * this function takes the args:
 * int log_level, char* format, ...
 */
#define snri_log(log_level, format, ...) _snricall(SNRICALL_LOG, log_level, format, ## __VA_ARGS__)
#define snri_logdebug(format, ...) _snricall(SNRICALL_LOG, LOG_DEBUG, format, ## __VA_ARGS__)

/**
 * Logs some binary data. Level indicates the logging level; you should use 0 for critical messages, and increasingly
 * high numbers for higher and higher verbosities.
 */
int snri_log_binary(int level, char * data, int data_size);

/**
 * resolves name to an address and stores the result in addr_out. name MUST be a
 * NULL terminated string or the behavior is undefined.
 */
int snri_resolve_name(char* name, in_addr_t* addr_out);

/**
 * resolves address to a name and stores the result in name_out. name_out should
 * point to a buffer of length name_out_len to avoid overflows. if the buffer name_out
 * points to is smaller than the actual hostname, this will return an error.
 */
int snri_resolve_addr(in_addr_t addr, char* name_out, int name_out_len);

/**
 * resolves the node given by addr and returns the minimum of its configured
 * upload and download bandwidth in killobytes per second into bw_KBps_out.
 * if addr is not mapped, bw_KBps_out will be set to 0. always returns SNRI_SUCCESS.
 */
int snri_resolve_minbw(in_addr_t addr, unsigned int* bw_KBps_out);

/**
 * get local node's ip address
 *
 * @return IP of this node
 */
int snri_getip(in_addr_t* addr_out);

/**
 * returns the hostname of the caller. if the buffer name_out points to is
 * smaller than the actual hostname, this will return an error.
 */
int snri_gethostname(char* name_out, int name_out_len);

/*
 * returns 1 if the virtual socket referred to by sockd exists and is
 * ready for reading, -1 on error, 0 otherwise.
 */
int snri_socket_is_readable(int sockd);

/*
 * returns 1 if the virtual socket referred to by sockd exists and is
 * ready for writing, -1 on error, 0 otherwise.
 */
int snri_socket_is_writable(int sockd);

/*
 * set the callback that will called whenever libevent loopexit function gets called
 */
int snri_set_loopexit_fn(snri_timer_callback_fp fn);

/**
 * Registers the set of globals for this module.
 * This should be done when the module is created and gets the _snri_mod_init function called
 *
 * @param num_globals Number of globals to register
 * The rest of the params should follow this form:
 * 		global variable pointer
 * 		size of data
 *
 * For instance, you may have the following globals:
 *   int a; char b;
 *
 * So, you'd call:
 *
 *  snri_regsiter_globals(2, &a, sizeof(a), &b, sizeof(b));
 *
 * Simple!
 */
#define snri_register_globals(...) _snricall(SNRICALL_REGISTER_GLOBALS, __VA_ARGS__)

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK 04000
#endif

#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC 01000000
#endif

#endif /* SHD_PLUGIN_H_ */