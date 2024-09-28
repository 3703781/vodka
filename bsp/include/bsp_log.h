#ifndef __BSP_LOG_H
#define __BSP_LOG_H

// define the maximum number of concurrent subscribers
#define BSP_LOG_MAX_SUBSCRIBERS 64

// maximum length of formatted log message
#define BSP_LOG_MAX_LENGTH 4096

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
typedef void (*bsp_log_msg_fn)(enum bsp_log_lvl level, char* str_level, char *msg);
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
	sys_slist_t _subscribers;
	size_t buf_size;
	char *_buf;
};

extern struct bsp_module bsp_log_mod;

#define bsp_log(...)	      bsp_log_printf(__VA_ARGS__)
#define bsp_log_trace(...)    bsp_log_printf(BSP_LOG_TRACE, __VA_ARGS__)
#define bsp_log_debug(...)    bsp_log_printf(BSP_LOG_DEBUG, __VA_ARGS__)
#define bsp_log_info(...)     bsp_log_printf(BSP_LOG_INFO, __VA_ARGS__)
#define bsp_log_warning(...)  bsp_log_printf(BSP_LOG_WARNING, __VA_ARGS__)
#define bsp_log_error(...)    bsp_log_printf(BSP_LOG_ERROR, __VA_ARGS__)
#define bsp_log_critical(...) bsp_log_printf(BSP_LOG_CRITICAL, __VA_ARGS__)
#define bsp_log_always(...)   bsp_log_printf(BSP_LOG_ALWAYS, __VA_ARGS__)

void bsp_log_init(void);
int bsp_log_subscribe(bsp_log_function_t fn, enum bsp_log_lvl threshold);
int bsp_log_unsubscribe(bsp_log_function_t fn);
const char *bsp_log_get_lvl_name(enum bsp_log_lvl level);
void bsp_log_printf(enum bsp_log_lvl severity, const char *fmt, ...);

#endif
