#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "dict.h"

#define MAX_LINE_LENGTH 1024
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

static char *argv0;
static dictionary *variable_dict;
static char *variable_pat[4] = { "@([[:graph:]]+)@", "\\$\\{([[:graph:]]+)\\}", "\\$CACHE\\{([[:graph:]]+)\\}",
				 "\\$ENV\\{([[:graph:]]+)\\}" };
static char *cmakedef_pat = "^[[:space:]]*#[[:space:]]*(cmakedefine)[[:space:]]+([[:graph:]]+).*$";
static char *cmakedef01_pat = "^[[:space:]]*#[[:space:]]*(cmakedefine01)[[:space:]]+([[:graph:]]+)[[:space:]]*$";
static regex_t variable_reg[ARRAY_SIZE(variable_pat)];
static regex_t cmakedef_reg;
static regex_t cmakedef01_reg;

struct match_data {
	size_t full_s;
	size_t full_e;
	size_t full_len;
	size_t name_s;
	size_t name_e;
	size_t name_len;
	size_t value_len;
	const char *full;
	const char *name;
	const char *value;
	const char *line;
};

static inline char *get_value(const char *name)
{
	return dictionary_get(variable_dict, name, NULL);
}

static int init_match_data(struct match_data *match_data, regmatch_t *regmatch, const char *const line)
{
	if (regmatch->rm_eo == 0)
		return EXIT_FAILURE;
	match_data->full_s = regmatch[0].rm_so;
	match_data->full_e = regmatch[0].rm_eo;
	match_data->name_s = regmatch[1].rm_so;
	match_data->name_e = regmatch[1].rm_eo;
	match_data->full_len = match_data->full_e - match_data->full_s;
	match_data->name_len = match_data->name_e - match_data->name_s;
	match_data->full = strndup(line + match_data->full_s, match_data->full_len);
	match_data->name = strndup(line + match_data->name_s, match_data->name_len);
	match_data->value = get_value(match_data->name);
	match_data->value_len = match_data->value != NULL ? strlen(match_data->value) : 0;
	match_data->line = line;
	return EXIT_SUCCESS;
}

static void deinit_match_data(struct match_data *match_data)
{
	if (match_data == NULL)
		return;
	if (match_data->name != NULL)
		free((void *)match_data->name);
	if (match_data->full != NULL)
		free((void *)match_data->full);
	memset(match_data, 0, sizeof(*match_data));
}

static inline void sum_match_data(struct match_data *array, size_t count, struct match_data *out)
{
	memset(out, 0, sizeof(*out));
	while (count--) {
		out->full_len += array[count].full_len;
		out->name_len += array[count].name_len;
		out->value_len += array[count].value_len;
	}
}

static int cmp(const void *a, const void *b)
{
	return ((struct match_data *)a)->full_e - ((struct match_data *)b)->full_e;
}

static char *do_replace_variable(char *const line)
{
	regmatch_t matches[16];
	struct match_data match_data[ARRAY_SIZE(variable_pat)][ARRAY_SIZE(matches)];
	size_t flat_count = ARRAY_SIZE(match_data) * ARRAY_SIZE(match_data[0]);

	memset(match_data, 0, sizeof(struct match_data) * flat_count);

	for (size_t i = 0; i < ARRAY_SIZE(variable_pat); i++) {
		memset(matches, 0, sizeof(matches));
		char *tmp_line = line;
		size_t j = 0;

		while (regexec(&variable_reg[i], tmp_line, 2, matches, 0) == 0) {
			size_t offset = tmp_line - line;
			struct match_data *m = &(match_data[i][j]);
			matches->rm_so += offset;
			matches->rm_eo += offset;
			init_match_data(m, matches, tmp_line);
			tmp_line += matches[0].rm_eo;
			j++;
		}
	}
	qsort(match_data, flat_count, sizeof(match_data[0][0]), cmp);
	struct match_data match_data_sum;
	struct match_data *md = &match_data[0][0];
	sum_match_data(md, flat_count, &match_data_sum);
	char *new_line =
		malloc(strlen(line) + 1 + MAX((long)match_data_sum.value_len - (long)match_data_sum.full_len, 0));
	char *new_line_end = new_line;
	new_line[0] = '\0';
	for (size_t i = 0; i < flat_count; i++) {
		if (md[i].full_e == 0)
			continue;
		size_t left = md[i].full_s;
		if (new_line[0] == '\0') {
			memcpy(new_line_end, line, left);
			new_line_end += left;
		}

		if (md[i].value != NULL) {
			memcpy(new_line_end, md[i].value, md[i].value_len);
			new_line_end += md[i].value_len;
		} else {
			memcpy(new_line_end, md[i].full, md[i].full_len);
			new_line_end += md[i].full_len;
		}
		size_t right = ((i == flat_count - 1) ? strlen(line) + 1 : md[i + 1].full_s) - md[i].full_e;
		memcpy(new_line_end, line + md[i].full_e, right);
		new_line_end += right;
	}

	if (new_line_end == new_line) {
		strcpy(new_line, line);
	} else {
		*new_line_end = '\0';
	}

	for (size_t i = 0; i < flat_count; i++)
		deinit_match_data(&md[i]);

	return new_line;
}

static char *do_replace_cmakedef(char *const line)
{
	regmatch_t matches[3];
	char *name = NULL;
	size_t name_len = 0;
	char *new_line, *new_line_end;
	size_t line_len = strlen(line);

	if (regexec(&cmakedef_reg, line, 3, matches, 0) == 0) {
		//
		// +--------| ^[[:space:]]*#[[:space:]]*(cmakedefine)[[:space:]]+([[:graph:]]+).*$ |--------+
		// |                                                                                        |
		// |  0     1  2  3  4  5  6  7  8  9  10 11  12    13  14  15  16  17   18 19 20 21 22 23  |
		// |                                                                                        |
		// |  #     c  m  a  k  e  d  e  f  i  n  e  <sp>   n   a   m   e  <sp>  x  x  x  x  \n \0  |
		// |  ^     ^                                 ^     ^               ^                    ^  |
		// |  s <1> s             <- 11 ->            e <1> s    <- 4 ->    e      <- 6 ->       e  |
		// |  |     `-----------matches[1]------------'     `---matches[2]--'                    |  |
		// |  `----------------------------------matches[0]--------------------------------------'  |
		// |                                                                                        |
		// +----------------------------------------------------------------------------------------+

		name_len = matches[2].rm_eo - matches[2].rm_so;
		name = strndup(line + matches[2].rm_so, name_len);
		if (get_value(name) != NULL) {
			new_line_end = new_line = malloc(line_len - 5);
			memcpy(new_line_end, line, matches[1].rm_so);
			new_line_end += matches[1].rm_so;
			memcpy(new_line_end, "define", 6);
			new_line_end += 6;
			memcpy(new_line_end, line + matches[1].rm_eo, line_len - matches[1].rm_eo + 1);
		} else {
			new_line_end = new_line = malloc(name_len + 14);
			memcpy(new_line_end, "/* #undef ", 10);
			new_line_end += 10;
			memcpy(new_line_end, name, name_len);
			new_line_end += name_len;
			memcpy(new_line_end, " */\n", 5);
		}
	} else if (regexec(&cmakedef01_reg, line, 3, matches, 0) == 0) {
		//
		// +--| ^[[:space:]]*#[[:space:]]*(cmakedefine01)[[:space:]]+([[:graph:]]+)[[:space:]]*$ |---+
		// |                                                                                         |
		// |  0     1  2  3  4  5  6  7  8  9  10 11 12 13  14    15  16  17  18  19 20 21 22 23 24  |
		// |                                                                                         |
		// |  #     c  m  a  k  e  d  e  f  i  n  e  0  1  <sp>   n   a   m   e   .  .  .  .  \n \0  |
		// |  ^     ^                                       ^     ^               ^               ^  |
		// |  s <1> s              <- 13 ->                 e <1> s    <- 4 ->    e   <- 5 ->     e  |
		// |  |     `-------------matches[1]----------------'     `---matches[2]--'               |  |
		// |  `------------------------------matches[0]-------------------------------------------'  |
		// |                                                                                         |
		// +-----------------------------------------------------------------------------------------+
		name_len = matches[2].rm_eo - matches[2].rm_so;
		name = strndup(line + matches[2].rm_so, name_len);
		new_line_end = new_line = malloc(line_len);
		memcpy(new_line_end, "#define ", 8);
		new_line_end += 8;
		memcpy(new_line_end, name, name_len);
		new_line_end += name_len;
		memcpy(new_line_end, get_value(name) != NULL ? " 1" : " 0", 2);
		new_line_end += 2;
		memcpy(new_line_end, line + matches[2].rm_eo, line_len + 1 - matches[2].rm_eo);
	} else {
		return strdup(line);
	}
	free(name);
	return new_line;
}

static inline void print_usage(void)
{
	fprintf(stderr, "Usage: %s <input_file.in> <output_file> <variable1=value1> [variable2=value2] ...\n", argv0);
	exit(EXIT_FAILURE);
}

static inline void print_dictionary(void)
{
	for (int i = 0; i < variable_dict->n; i++) {
		char *p = dictionary_get(variable_dict, variable_dict->key[i], "default");
		printf("%s: %s\n", variable_dict->key[i], p);
	}
}

static inline void do_init_regrex(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(variable_pat); i++) {
		if (regcomp(&variable_reg[i], variable_pat[i], REG_EXTENDED) != 0)
			goto err;
	}

	if (regcomp(&cmakedef_reg, cmakedef_pat, REG_EXTENDED) != 0)
		goto err;

	if (regcomp(&cmakedef01_reg, cmakedef01_pat, REG_EXTENDED) != 0)
		goto err;

	return;
err:
	perror("Error compiling regular expression");
	exit(EXIT_FAILURE);
}

static inline void do_deinit_regrex(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(variable_pat); i++)
		regfree(&variable_reg[i]);

	regfree(&cmakedef_reg);
	regfree(&cmakedef01_reg);
}

int main(int argc, char *argv[])
{
	argv0 = strdup(argv[0]);
	if (argv0 == NULL)
		exit(EXIT_FAILURE);

	if (argc < 2)
		print_usage();

	variable_dict = dictionary_new(64);

	FILE *input_file = fopen(argv[1], "r");
	if (input_file == NULL) {
		perror("Error opening input file");
		exit(EXIT_FAILURE);
	}

	FILE *output_file = fopen(argv[2], "w");
	if (output_file == NULL) {
		perror("Error opening output file");
		fclose(input_file);
		exit(EXIT_FAILURE);
	}
	do_init_regrex();

	char line_array[MAX_LINE_LENGTH];
	char *line = (char *)line_array;

	for (int i = 3; i < argc; i++) {
		char *equals_sign = strchr(argv[i], '=');
		if (equals_sign != NULL) {
			*equals_sign = '\0';
			char *variable_name = argv[i];
			char *variable_value = equals_sign + 1;
			dictionary_set(variable_dict, variable_name, strdup(variable_value));
		} else {
			fprintf(stderr, "variable error:%d\n", i);
			print_usage();
		}
	}

	while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
		char *new_line = do_replace_variable(line);
		char *new_new_line = do_replace_cmakedef(new_line);

		fputs(new_new_line, output_file);
		free(new_line);
		free(new_new_line);
	}

	do_deinit_regrex();

	fclose(input_file);
	fclose(output_file);

	free(argv0);

	return 0;
}
