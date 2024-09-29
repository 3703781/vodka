#ifndef __BSP_LOG_H
#define __BSP_LOG_H

enum bsp_log_lvl {
	BSP_LOG_LVL_TRACE = 0,
	BSP_LOG_LVL_DEBUG,
	BSP_LOG_LVL_INFO,
	BSP_LOG_LVL_WARNING,
	BSP_LOG_LVL_ERROR,
	BSP_LOG_LVL_CRITICAL,
	BSP_LOG_LVL_ALWAYS,
	BSP_LOG_LVL_NUM,
};

typedef void (*bsp_log_msg_fn)(struct bsp_log_des *des);
struct bsp_log_subs {
	sys_snode_t node;
	bsp_log_msg_fn message;
	enum bsp_log_lvl threshold;
};

/**
 * @brief Structure representing the log descriptor
 * 
 * This structure contains various parameters and configurations related to the logging function.
 */
struct bsp_log_des {
	size_t buf_size;
	uint64_t (*get_timestamp)(void);
	int _str_len;
	enum bsp_log_lvl _level;
	const char *_level_name;
	const char *_color;
	sys_slist_t _subscribers;
	char *_buf;
	char *_buf2;
	uint64_t _time_stamp;
};

extern struct bsp_module bsp_log_mod;

#endif
