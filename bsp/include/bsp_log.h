#ifndef __BSP_LOG_H
#define __BSP_LOG_H

#define BSP_LOG_DECLARE()                                                            \
	struct bsp_module *__log_mod = bsp_module_find("LOG");                       \
	struct bsp_log_des *__log_des = (struct bsp_log_des *)__log_mod->descriptor; \
	__log_des->owner = BSP_MODULE_THIS;
#define BSP_LOG_TRC(fmt, ...)                                                \
	__log_des->_level = BSP_LOG_LVL_TRACE;                               \
	__log_des->_prefix_func = (char *)__func__;                          \
	__log_des->_prefix_file_line = __FILE__ ":" STRINGIFY(__LINE__); \
	bsp_log(__log_des, fmt "\r\n", ##__VA_ARGS__)

#define BSP_LOG_DBG(fmt, ...)                                                \
	__log_des->_level = BSP_LOG_LVL_DEBUG;                               \
	__log_des->_prefix_func = (char *)__func__;                          \
	__log_des->_prefix_file_line = __FILE__ ":" STRINGIFY(__LINE__); \
	bsp_log(__log_des, fmt "\r\n", ##__VA_ARGS__)

#define BSP_LOG_INF(fmt, ...)                                                \
	__log_des->_level = BSP_LOG_LVL_INFO;                                \
	__log_des->_prefix_func = (char *)__func__;                          \
	__log_des->_prefix_file_line = __FILE__ ":" STRINGIFY(__LINE__); \
	bsp_log(__log_des, fmt "\r\n", ##__VA_ARGS__)

#define BSP_LOG_WRN(fmt, ...)                                                \
	__log_des->_level = BSP_LOG_LVL_WARNING;                             \
	__log_des->_prefix_func = (char *)__func__;                          \
	__log_des->_prefix_file_line = __FILE__ ":" STRINGIFY(__LINE__); \
	bsp_log(__log_des, fmt "\r\n", ##__VA_ARGS__)

#define BSP_LOG_ERR(fmt, ...)                                                \
	__log_des->_level = BSP_LOG_LVL_ERROR;                               \
	__log_des->_prefix_func = (char *)__func__;                          \
	__log_des->_prefix_file_line = __FILE__ ":" STRINGIFY(__LINE__); \
	bsp_log(__log_des, fmt "\r\n", ##__VA_ARGS__)

#define BSP_LOG_CRT(fmt, ...)                                                \
	__log_des->_level = BSP_LOG_LVL_CRITICAL;                            \
	__log_des->_prefix_func = (char *)__func__;                          \
	__log_des->_prefix_file_line = __FILE__ ":" STRINGIFY(__LINE__); \
	bsp_log(__log_des, fmt "\r\n", ##__VA_ARGS__)

#define BSP_LOG_ALW(fmt, ...)                                                \
	__log_des->_level = BSP_LOG_LVL_ALWAYS;                              \
	__log_des->_prefix_func = (char *)__func__;                          \
	__log_des->_prefix_file_line = __FILE__ ":" STRINGIFY(__LINE__); \
	bsp_log(__log_des, fmt "\r\n", ##__VA_ARGS__)

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
struct bsp_log_subs;
struct bsp_log_des;

typedef void (*bsp_log_msg_fn)(struct bsp_log_des *des, struct bsp_log_subs *subs);
struct bsp_log_subs {
	sys_snode_t node;
	bsp_log_msg_fn message;
	enum bsp_log_lvl threshold;
	uint8_t support_color;
};

/**
 * @brief Structure representing the log descriptor
 * 
 * This structure contains various parameters and configurations related to the logging function.
 */
struct bsp_log_des {
	size_t msg_size; //!< max size of _msg in bytes
	size_t prefix_size; //!< max size of _prefix_cl and _prefix_bw in bytes
	float (*get_timestamp)(void); //!< a function that returns timestamp in seconds

	struct bsp_module *owner;

	char *_msg; //!< the current log message
	char *_prefix_file_line;
	char *_prefix_func;
	char *_prefix_cl; //!< to store the prefix of the current log message in the form of "\e[32m[0.000775]\e[0m \e[33mmodule\e[0m \e[33mWARNING\e[0m: "
	char *_prefix_bw; //!< to store a prefix of the current log message in the form of "[0.000775] module WARNING: "

	int _msg_strlen; //!< length of the log currently stored in _msg, excluding the null byte used to end output to strings
	int _prefix_cl_strlen; //!< length of the prefix currently stored in _prefix_cl, excluding the null byte used to end output to strings
	int _prefix_bw_strlen; //!< length of the prefix currently stored in _prefix_bw, excluding the null byte used to end output to strings

	enum bsp_log_lvl _level; //!< error level of the current log
	const char *_level_name; //!< name of error level of the current log
	const char *_color; //!< corrsponding color of the error level of the current log

	sys_slist_t _subscribers; //!< a single linked list that output the log
	float _time_stamp; //!< the timestamp in seconds of the current message
	uint8_t _flag; //!< bitfield of flags, see @ref bsp_log_flag
};

/**
 * @defgroup bsp_log_flag definition of _flag field of struct bsp_log_des
 *
 * bit        7    6    5    4    3    2    1    0
 * _flag      -    -    -    -    -    -    BW   CL
 * 
 * BW and CL stand for black & white and color respectively. If there is any
 * subscriber that support color output, bit CL is set to indicate that _prefix_cl
 * is used to store the colorful prefix. Similarly, bit BW indicats that _prefix_bw
 * is to store a plain prefix
 * 
 * @{
 */

#define BSP_LOG_FLAG_CL_POS ((uint32_t)0)
#define BSP_LOG_FLAG_BW_POS ((uint32_t)1)
#define BSP_LOG_FLAG_CL_MSK (1 << BSP_LOG_FLAG_CL_POS)
#define BSP_LOG_FLAG_BW_MSK (1 << BSP_LOG_FLAG_BW_POS)

/**
 * @brief Get whether CL bit is set
 * @param des a pointer to the struct bsp_log_des object
 */
#define BSP_LOG_FLAG_NEED_CL(des) (!!((des)->_flag & BSP_LOG_FLAG_CL_MSK))

/**
 * @brief Get whether BW bit is set
 * @param des a pointer to the struct bsp_log_des object
 */
#define BSP_LOG_FLAG_NEED_BW(des) (!!((des)->_flag & BSP_LOG_FLAG_BW_MSK))

/**
 * @brief Set CL bit
 * @param des a pointer to the struct bsp_log_des object
 */
#define BSP_LOG_FLAG_SET_CL(des) (((des)->_flag |= BSP_LOG_FLAG_CL_MSK))

/**
 * @brief Set BW bit
 * @param des a pointer to the struct bsp_log_des object
 */
#define BSP_LOG_FLAG_SET_BW(des) (((des)->_flag |= BSP_LOG_FLAG_BW_MSK))

/**
 * @brief Clear CL bit
 * @param des a pointer to the struct bsp_log_des object
 */
#define BSP_LOG_FLAG_CLR_CL(des) (((des)->_flag &= ~BSP_LOG_FLAG_CL_MSK))

/**
 * @brief Clear BW bit
 * @param des a pointer to the struct bsp_log_des object
 */
#define BSP_LOG_FLAG_CLR_BW(des) (((des)->_flag &= ~BSP_LOG_FLAG_BW_MSK))
/**
 * @}
 */
extern struct bsp_module bsp_log_mod;

int bsp_log_subscribe(struct bsp_log_des *des, bsp_log_msg_fn fn, enum bsp_log_lvl threshold, uint8_t support_color);
int bsp_log_unsubscribe(struct bsp_log_des *des, bsp_log_msg_fn fn);
void bsp_log(struct bsp_log_des *des, const char *fmt, ...);

#endif
