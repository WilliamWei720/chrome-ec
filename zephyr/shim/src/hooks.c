/* Copyright 2020 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <zephyr/kernel.h>

#include <ap_power/ap_power.h>
#include <ap_power/ap_power_events.h>
#include "common.h"
#include "console.h"
#include "ec_tasks.h"
#include "hooks.h"
#include "hook_types.h"
#include "task.h"
#include "timer.h"

/*
 * hook_registry maps each hook_type to the list of handlers for that hook type.
 *
 * Because this structure is not supported by the usual STRUCT_SECTION_FOREACH,
 * this code must manually generate references to the symbols generated by
 * STRUCT_SECTION_ITERABLE_ALTERNATE in zephyr_hooks_shim.h.
 */
#define HOOK_LIST_EXTERNS(type)                          \
	extern const struct zephyr_shim_hook_info        \
		_zephyr_shim_hook_##type##_list_start[]; \
	extern const struct zephyr_shim_hook_info        \
		_zephyr_shim_hook_##type##_list_end[];
FOR_EACH(HOOK_LIST_EXTERNS, (), HOOK_TYPES_LIST)

#define HOOK_LIST_ENTRY(type)                                   \
	[type] = {                                              \
		.start = _zephyr_shim_hook_##type##_list_start, \
		.end = _zephyr_shim_hook_##type##_list_end,     \
	}
static const struct zephyr_shim_hook_list hook_registry[] = { FOR_EACH(
	HOOK_LIST_ENTRY, (, ), HOOK_TYPES_LIST) };
BUILD_ASSERT(ARRAY_SIZE(hook_registry) == HOOK_TYPE_COUNT,
	     "All defined hook types must be represented in hook_registry");
BUILD_ASSERT(NUM_VA_ARGS_LESS_1(HOOK_TYPES_LIST) + 1 == HOOK_TYPE_COUNT,
	     "At least one hook type is missing from HOOK_TYPES_LIST");

static void hook_second_work(struct k_work *work);
static void hook_tick_work(struct k_work *work);

static K_WORK_DELAYABLE_DEFINE(hook_seconds_work_data, hook_second_work);
static K_WORK_DELAYABLE_DEFINE(hook_ticks_work_data, hook_tick_work);

static void work_queue_error(const void *data, int rv)
{
	cprints(CC_HOOK,
		"Warning: deferred call not submitted, "
		"deferred_data=0x%p, err=%d",
		data, rv);
}

static void hook_second_work(struct k_work *work)
{
	int rv;

	hook_notify(HOOK_SECOND);

	rv = k_work_reschedule(&hook_seconds_work_data, K_SECONDS(1));
	if (rv < 0)
		work_queue_error(&hook_seconds_work_data, rv);
}

static void hook_tick_work(struct k_work *work)
{
	int rv;

	hook_notify(HOOK_TICK);

	rv = k_work_reschedule(&hook_ticks_work_data,
			       K_USEC(HOOK_TICK_INTERVAL));
	if (rv < 0)
		work_queue_error(&hook_ticks_work_data, rv);
}

static void check_hook_task_priority(void)
{
	k_tid_t thread = &k_sys_work_q.thread;

	/*
	 * Numerically lower priorities take precedence, so verify the hook
	 * related threads cannot preempt any of the shimmed tasks.
	 */
	if (k_thread_priority_get(thread) < (TASK_ID_COUNT - 1))
		cprintf(CC_HOOK,
			"ERROR: %s has priority %d but must be >= %d\n",
			k_thread_name_get(thread),
			k_thread_priority_get(thread), (TASK_ID_COUNT - 1));
}
DECLARE_HOOK(HOOK_INIT, check_hook_task_priority, HOOK_PRIO_FIRST);

static int zephyr_shim_setup_hooks(const struct device *unused)
{
	int rv;

	/* Startup the HOOK_TICK and HOOK_SECOND recurring work */
	rv = k_work_reschedule(&hook_seconds_work_data, K_SECONDS(1));
	if (rv < 0)
		work_queue_error(&hook_seconds_work_data, rv);

	rv = k_work_reschedule(&hook_ticks_work_data,
			       K_USEC(HOOK_TICK_INTERVAL));
	if (rv < 0)
		work_queue_error(&hook_ticks_work_data, rv);

	return 0;
}

SYS_INIT(zephyr_shim_setup_hooks, APPLICATION, 1);

void hook_notify(enum hook_type type)
{
	const struct zephyr_shim_hook_info *start = hook_registry[type].start;
	const struct zephyr_shim_hook_info *end = hook_registry[type].end;
	int last_prio = HOOK_PRIO_FIRST - 1;

	__ASSERT(type >= 0 && type < HOOK_TYPE_COUNT,
		 "hook type %d is out of range (maximum hook_type value %d)",
		 type, HOOK_TYPE_COUNT);

	while (1) {
		/*
		 * Find the lowest priority that is larger than the last pass'
		 * priority. That is, the next highest uncalled priority.
		 */
		int prio = INT_MAX;

		for (const struct zephyr_shim_hook_info *p = start; p != end;
		     p++) {
			__ASSERT(
				IN_RANGE(p->priority, HOOK_PRIO_FIRST,
					 HOOK_PRIO_LAST),
				"Hook priority %d (handler %p) is out of range",
				p->priority, p->routine);
			if (p->priority > last_prio)
				prio = MIN(prio, p->priority);
		}

		if (prio == INT_MAX) {
			/* No more handlers of higher priority */
			break;
		}
		last_prio = prio;

		/* Call each handler with the located priority */
		for (const struct zephyr_shim_hook_info *p = start; p != end;
		     p++) {
			if (p->priority == prio)
				p->routine();
		}
	};
}

int hook_call_deferred(const struct deferred_data *data, int us)
{
	struct k_work_delayable *work = data->work;
	int rv = 0;

	if (us == -1) {
		k_work_cancel_delayable(work);
	} else if (us >= 0) {
		rv = k_work_reschedule(work, K_USEC(us));
		if (rv < 0) {
			work_queue_error(data, rv);
		}
	} else {
		return EC_ERROR_PARAM2;
	}

	return rv >= 0 ? EC_SUCCESS : rv;
}

/*
 * Shims for interconnecting AP power sequence events with legacy hooks.
 * Depending on whether the power sequence code is running in zephyr or
 * not, a shim is setup to send events either from the legacy hooks to
 * the AP power event callbacks, or vice versa.
 */
#ifdef CONFIG_AP_PWRSEQ

/*
 * Callback handler, dispatch to hooks
 */
static void ev_handler(struct ap_power_ev_callback *cb,
		       struct ap_power_ev_data data)
{
	switch (data.event) {
	default:
		break;

#define CASE_HOOK(h)                           \
	case AP_POWER_##h:                     \
		hook_notify(HOOK_CHIPSET_##h); \
		break

		CASE_HOOK(PRE_INIT);
		CASE_HOOK(STARTUP);
		CASE_HOOK(RESUME);
		CASE_HOOK(SUSPEND);
#if CONFIG_PLATFORM_EC_CHIPSET_RESUME_INIT_HOOK
		CASE_HOOK(RESUME_INIT);
		CASE_HOOK(SUSPEND_COMPLETE);
#endif
		CASE_HOOK(SHUTDOWN);
		CASE_HOOK(SHUTDOWN_COMPLETE);
		CASE_HOOK(HARD_OFF);
		CASE_HOOK(RESET);
	}
}

/*
 * Events are received from the AP power event system and sent to the hooks.
 */
static int zephyr_shim_ap_power_event(const struct device *unused)
{
	static struct ap_power_ev_callback cb;

	/*
	 * Register for all events.
	 */
	ap_power_ev_init_callback(
		&cb, ev_handler,
		AP_POWER_PRE_INIT | AP_POWER_STARTUP | AP_POWER_RESUME |
			AP_POWER_SUSPEND |
#if CONFIG_PLATFORM_EC_CHIPSET_RESUME_INIT_HOOK
			AP_POWER_RESUME_INIT | AP_POWER_SUSPEND_COMPLETE |
#endif
			AP_POWER_SHUTDOWN | AP_POWER_SHUTDOWN_COMPLETE |
			AP_POWER_HARD_OFF | AP_POWER_RESET);
	ap_power_ev_add_callback(&cb);
	return 0;
}

SYS_INIT(zephyr_shim_ap_power_event, APPLICATION, 1);
#else /* !CONFIG_AP_PWRSEQ */

/*
 * Events received from the hooks and sent to the AP power event callbacks.
 */
#define EV_HOOK(h)                                        \
	static void hook_##h(void)                        \
	{                                                 \
		ap_power_ev_send_callbacks(AP_POWER_##h); \
	}                                                 \
	DECLARE_HOOK(HOOK_CHIPSET_##h, hook_##h, HOOK_PRIO_DEFAULT)

EV_HOOK(PRE_INIT);
EV_HOOK(STARTUP);
EV_HOOK(RESUME);
EV_HOOK(SUSPEND);
#if CONFIG_PLATFORM_EC_CHIPSET_RESUME_INIT_HOOK
EV_HOOK(RESUME_INIT);
EV_HOOK(SUSPEND_COMPLETE);
#endif
EV_HOOK(SHUTDOWN);
EV_HOOK(SHUTDOWN_COMPLETE);
EV_HOOK(HARD_OFF);
EV_HOOK(RESET);
#endif /* !CONFIG_AP_PWRSEQ */
