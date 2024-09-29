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

typedef void (*bsp_log_msg_fn)(enum bsp_log_lvl level, char *str_level, char *msg);
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
	void (*log)(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, ...);
	void (*trace)(struct bsp_log_des *des, const char *fmt, ...);
	void (*debug)(struct bsp_log_des *des, const char *fmt, ...);
	void (*info)(struct bsp_log_des *des, const char *fmt, ...);
	void (*warning)(struct bsp_log_des *des, const char *fmt, ...);
	void (*error)(struct bsp_log_des *des, const char *fmt, ...);
	void (*critical)(struct bsp_log_des *des, const char *fmt, ...);
	void (*always)(struct bsp_log_des *des, const char *fmt, ...);
	sys_slist_t _subscribers;
	char *_buf;
};

extern struct bsp_module bsp_log_mod;


#endif
