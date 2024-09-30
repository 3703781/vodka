#include <bsp.h>

enum bsp_log_color {
	BSP_LOG_COLOR_NRM = 0,
	BSP_LOG_COLOR_RED,
	BSP_LOG_COLOR_GRN,
	BSP_LOG_COLOR_YEL,
	BSP_LOG_COLOR_BLU,
	BSP_LOG_COLOR_MAG,
	BSP_LOG_COLOR_CYN,
	BSP_LOG_COLOR_WHT,
	BSP_LOG_COLOR_NUM
};

static const char *lvl_name[BSP_LOG_LVL_NUM] = { "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "ALWAYS" };
static const enum bsp_log_color lvl_color[BSP_LOG_LVL_NUM] = { BSP_LOG_COLOR_NRM, BSP_LOG_COLOR_NRM, BSP_LOG_COLOR_NRM,
							       BSP_LOG_COLOR_YEL, BSP_LOG_COLOR_MAG, BSP_LOG_COLOR_RED,
							       BSP_LOG_COLOR_CYN };
static const char *color_str[BSP_LOG_COLOR_NUM] = { "\e[0m",  "\e[31m", "\e[32m", "\e[33m",
						    "\e[34m", "\e[35m", "\e[36m", "\e[37m" };

int bsp_log_init(void *des);
int bsp_log_uninit(void *des);
static void *set_default_des(void *des);

struct bsp_module bsp_log_mod = { .name = "LOG",
				  .state = BSP_MODULE_STATE_COMING,
				  .prepare = set_default_des,
				  .setup = bsp_log_init,
				  .desetup = bsp_log_uninit,
				  .descriptor = NULL,
				  .version = "0.1" };
BSP_MODULE_DECLARE(bsp_log_mod);
/**
 * @brief Initialize uninitialized members in the log descriptor with default values.
 * 
 * @param des log descriptor, set to NULL to alloc the memory of the descriptor automatically.
 * @return struct bsp_log_des * Pointer to descriptor on success, otherwise negative error code.
 */
static void *set_default_des(void *des)
{
	struct bsp_log_des *int_des = (struct bsp_log_des *)des;

	if (int_des == NULL) {
		int_des = (struct bsp_log_des *)calloc(sizeof(struct bsp_log_des), 1);
		if (IS_ERR_OR_NULL(int_des))
			return ERR_PTR(-ENOMEM);
	}

	/* set to default value */
	SET_VAL_IF_ZERO(int_des->msg_size, 4096);
	SET_VAL_IF_ZERO(int_des->prefix_size, 128);
	SET_VAL_IF_ZERO(int_des->_flag, 0);
	SET_VAL_IF_NULL(int_des->_msg, calloc(int_des->msg_size, 1));
	if (IS_ERR_OR_NULL(int_des->_msg))
		return ERR_PTR(-ENOMEM);
	SET_VAL_IF_NULL(int_des->_prefix_cl, calloc(int_des->prefix_size, 1));
	if (IS_ERR_OR_NULL(int_des->_prefix_cl))
		return ERR_PTR(-ENOMEM);
	SET_VAL_IF_NULL(int_des->_prefix_bw, calloc(int_des->prefix_size, 1));
	if (IS_ERR_OR_NULL(int_des->_prefix_bw))
		return ERR_PTR(-ENOMEM);
	sys_slist_init(&int_des->_subscribers);
	SET_VAL_IF_NULL(int_des->get_timestamp, bsp_debug_tsg_get_ms);
	SET_VAL_IF_NULL(int_des->_color, color_str[(size_t)BSP_LOG_COLOR_NRM]);

	return int_des;
}

static void vlog(struct bsp_log_des *des, const char *fmt, va_list arg)
{
	struct bsp_log_subs *subs = NULL;
	const char * const mod_name = des->owner == NULL ? "": des->owner->name;
	des->_level_name = lvl_name[(size_t)des->_level];
	des->_time_stamp = des->get_timestamp();
	des->_color = color_str[(size_t)lvl_color[(size_t)des->_level]];

	des->_msg_strlen = vsnprintf(des->_msg, des->msg_size, fmt, arg);
	if (des->_msg_strlen <= 0)
		return;
	if (BSP_LOG_FLAG_NEED_CL(des)) {
		des->_prefix_cl_strlen = snprintf(des->_prefix_cl, des->prefix_size,
						  "\e[32m[%f] \e[33m%s\e[0m %s%s\e[0m: ", des->_time_stamp, mod_name,
						  des->_color, des->_level_name);
		if (des->_prefix_cl_strlen <= 0)
			return;
	} else if (BSP_LOG_FLAG_NEED_BW(des)) {
		des->_prefix_bw_strlen = snprintf(des->_prefix_bw, des->prefix_size, "[%f] %s %s: ", des->_time_stamp,
						  mod_name, des->_level_name);
		if (des->_prefix_bw_strlen <= 0)
			return;
	}

	SYS_SLIST_FOR_EACH_CONTAINER(&des->_subscribers, subs, node) {
		if (des->_level >= subs->threshold)
			subs->message(des, subs);
	}
}

void bsp_log_log(struct bsp_log_des *des, enum bsp_log_lvl level, const char *fmt, ...)
{
	va_list ap;
	des->_level = level;
	va_start(ap, fmt);
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_trace(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_TRACE;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_debug(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_DEBUG;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_info(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_INFO;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_warning(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_WARNING;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_error(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_ERROR;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_critical(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_CRITICAL;
	vlog(des, fmt, ap);
	va_end(ap);
}

void bsp_log_always(struct bsp_log_des *des, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	des->_level = BSP_LOG_LVL_ALWAYS;
	vlog(des, fmt, ap);
	va_end(ap);
}

static void tty1(struct bsp_log_des *des, struct bsp_log_subs *subs)
{
	static struct bsp_module *mod = NULL;
	if (mod == NULL)
		mod = bsp_module_find("TTY1");

	if (mod && mod->state == BSP_MODULE_STATE_LIVE) {
		struct bsp_tty_des *tty = (struct bsp_tty_des *)mod->descriptor;
		if (subs->support_color)
			tty->ops.write(tty, des->_prefix_cl, des->_prefix_cl_strlen);
		else
			tty->ops.write(tty, des->_prefix_bw, des->_prefix_bw_strlen);

		tty->ops.write(tty, des->_msg, des->_msg_strlen);
	}
}

static void itm_ch1(struct bsp_log_des *des, struct bsp_log_subs *subs)
{
	const char *const prefix = subs->support_color ? des->_prefix_cl : des->_prefix_bw;
	int str_len = subs->support_color ? des->_prefix_cl_strlen : des->_prefix_bw_strlen;
	for (size_t i = 0; i < str_len; i++) {
		ITM_SendChar(prefix[i], 1);
	}
	for (size_t i = 0; i < des->_msg_strlen; i++) {
		ITM_SendChar(des->_msg[i], 1);
	}
}

int bsp_log_init(void *des)
{
	struct bsp_log_des *int_des = (struct bsp_log_des *)des;
	if (int_des == NULL)
		return -ENXIO;
	if (bsp_log_subscribe(int_des, tty1, BSP_LOG_LVL_INFO, 1))
		return -EIO;
	if (bsp_log_subscribe(int_des, itm_ch1, BSP_LOG_LVL_INFO, 0))
		return -EIO;

	return 0;
}

/**
 * @brief Uninitialize log module
 * @param des 
 * @return 
 */
int bsp_log_uninit(void *des)
{
	struct bsp_log_des *int_des = (struct bsp_log_des *)des;
	free(int_des->_msg);
	int_des->_msg = NULL;
	free(int_des->_prefix_bw);
	int_des->_prefix_bw = NULL;
	free(int_des->_prefix_cl);
	int_des->_prefix_cl = NULL;
	sys_snode_t *node;
	struct bsp_log_subs *subs;
	while ((node = sys_slist_get(&int_des->_subscribers)) != NULL) {
		free(SYS_SLIST_CONTAINER(node, subs, node));
	}

	return 0;
}

/**
 * @brief Search the subscribers table to install or update fn
 * 
 */
int bsp_log_subscribe(struct bsp_log_des *des, bsp_log_msg_fn fn, enum bsp_log_lvl threshold, uint8_t support_color)
{
	struct bsp_log_subs *subscriber = malloc(sizeof(struct bsp_log_subs));
	if (subscriber == NULL)
		return -ENOMEM;
	subscriber->message = fn;
	subscriber->threshold = threshold;
	subscriber->support_color = support_color;
	sys_slist_append(&des->_subscribers, &subscriber->node);
	if (support_color)
		BSP_LOG_FLAG_SET_CL(des);
	else
		BSP_LOG_FLAG_SET_BW(des);

	return 0;
}

/**
 * @brief search the subscribers table to remove
 */
int bsp_log_unsubscribe(struct bsp_log_des *des, bsp_log_msg_fn fn)
{
	struct bsp_log_subs *subs = NULL;
	SYS_SLIST_FOR_EACH_CONTAINER(&des->_subscribers, subs, node) {
		if (subs->message == fn)
			break;
	}
	if (subs == NULL)
		return -ENOENT;
	sys_slist_find_and_remove(&des->_subscribers, &subs->node);
	free(subs);

	return 0;
}
