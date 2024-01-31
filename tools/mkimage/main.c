// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2009
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <imagetool.h>
#include <image.h>
#include <mkimage.h>

/* parameters initialized by core will be used by the image type code */
static struct image_tool_params params = {
	.os = IH_OS_LINUX,
	.arch = IH_ARCH_ARM,
	.type = IH_TYPE_S32CCIMAGE,
	.comp = IH_COMP_NONE,
	.addr = 0,
	.ep = 0,
	.imagename = "",
	.imagename2 = "",
};

static void usage(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	fprintf(stderr,
		"Usage: %s -l image\n"
		"          -l ==> list image header information\n",
		params.cmdname);
	fprintf(stderr,
		"       %s [-x] -A arch -a addr -e ep -n name -d data_file[:data_file...] image\n"
		"          -A ==> set architecture to 'arch'\n"
		"          -a ==> set load address to 'addr' (hex)\n"
		"          -e ==> set entry point to 'ep' (hex)\n"
		"          -n ==> set image name to 'name'\n"
		"          -d ==> use image data from 'datafile'\n",
		params.cmdname);

	exit(EXIT_FAILURE);
}

static void process_args(int argc, char **argv)
{
	char *ptr;
	int opt;

	while ((opt = getopt(argc, argv, "a:d:e:ln:")) != -1) {
		switch (opt) {
		case 'a':
			params.addr = strtoull(optarg, &ptr, 16);
			if (*ptr) {
				fprintf(stderr, "%s: invalid load address %s\n", params.cmdname, optarg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			params.datafile = optarg;
			params.dflag = 1;
			break;
		case 'e':
			params.ep = strtoull(optarg, &ptr, 16);
			if (*ptr) {
				fprintf(stderr, "%s: invalid entry point %s\n", params.cmdname, optarg);
				exit(EXIT_FAILURE);
			}
			params.eflag = 1;
			break;
		case 'l':
			params.lflag = 1;
			break;
		case 'n':
			params.imagename = optarg;
			break;
		default:
			usage("Invalid option");
		}
	}

	/* The last parameter is expected to be the imagefile */
	if (optind < argc)
		params.imagefile = argv[optind];

	if (!params.imagefile)
		usage("Missing output filename");
}

static void copy_file(int ifd, const char *datafile, int pad)
{
	int dfd;
	struct stat sbuf;
	unsigned char *ptr;
	int tail;
	int zero = 0;
	uint8_t zeros[4096];
	int offset = 0;
	int size;

	memset(zeros, 0, sizeof(zeros));

	if (params.vflag) {
		fprintf(stderr, "Adding Image %s\n", datafile);
	}

	if ((dfd = open(datafile, O_RDONLY | O_BINARY)) < 0) {
		fprintf(stderr, "%s: Can't open %s: %s\n", params.cmdname, datafile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fstat(dfd, &sbuf) < 0) {
		fprintf(stderr, "%s: Can't stat %s: %s\n", params.cmdname, datafile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	ptr = mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, dfd, 0);
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "%s: Can't read %s: %s\n", params.cmdname, datafile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	size = sbuf.st_size - offset;
	if (write(ifd, ptr + offset, size) != size) {
		fprintf(stderr, "%s: Write error on %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	tail = size % 4;
	if ((pad == 1) && (tail != 0)) {
		if (write(ifd, (char *)&zero, 4 - tail) != 4 - tail) {
			fprintf(stderr, "%s: Write error on %s: %s\n", params.cmdname, params.imagefile,
				strerror(errno));
			exit(EXIT_FAILURE);
		}
	} else if (pad > 1) {
		while (pad > 0) {
			int todo = sizeof(zeros);

			if (todo > pad)
				todo = pad;
			if (write(ifd, (char *)&zeros, todo) != todo) {
				fprintf(stderr, "%s: Write error on %s: %s\n", params.cmdname, params.imagefile,
					strerror(errno));
				exit(EXIT_FAILURE);
			}
			pad -= todo;
		}
	}

	(void)munmap((void *)ptr, sbuf.st_size);
	(void)close(dfd);
}

int main(int argc, char **argv)
{
	int ifd = -1;
	struct stat sbuf;
	char *ptr;
	int retval = 0;
	int pad_len = 0;
	int dfd;
	size_t map_len;

	params.cmdname = *argv;

	process_args(argc, argv);

	if (s32cc_check_params(&params))
		usage("Bad parameters for image type");

	if (!params.eflag) {
		params.ep = params.addr;
	}

	if (params.lflag) {
		ifd = open(params.imagefile, O_RDONLY | O_BINARY);
	} else {
		ifd = open(params.imagefile, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0666);
	}

	if (ifd < 0) {
		fprintf(stderr, "%s: Can't open %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (params.lflag) {
		if (fstat(ifd, &sbuf) < 0) {
			fprintf(stderr, "%s: Can't stat %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
			exit(EXIT_FAILURE);
		}

		ptr = mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
		if (ptr == MAP_FAILED) {
			fprintf(stderr, "%s: Can't read %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
			exit(EXIT_FAILURE);
		}

		retval = s32cc_verify_header((unsigned char *)ptr, sbuf.st_size, &params);
		if(retval)
		{
			fprintf(stderr, "%s: Can't verify head from %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
			exit(EXIT_FAILURE);
		}
		s32cc_print_header(ptr);

		(void)munmap((void *)ptr, sbuf.st_size);
		(void)close(ifd);

		exit(retval);
	}

	dfd = open(params.datafile, O_RDONLY | O_BINARY);
	if (dfd < 0) {
		fprintf(stderr, "%s: Can't open %s: %s\n", params.cmdname, params.datafile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fstat(dfd, &sbuf) < 0) {
		fprintf(stderr, "%s: Can't stat %s: %s\n", params.cmdname, params.datafile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	params.file_size = sbuf.st_size;

	pad_len = s32cc_vrec_header(&params, s32cc_image_type_params_ptr);

	if (write(ifd, s32cc_image_type_params_ptr->hdr, s32cc_image_type_params_ptr->header_size) !=
	    s32cc_image_type_params_ptr->header_size) {
		fprintf(stderr, "%s: Write error on %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	copy_file(ifd, params.datafile, pad_len);

	/* We're a bit of paranoid */
#if defined(_POSIX_SYNCHRONIZED_IO) && !defined(__sun__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&          \
	!defined(__APPLE__)
	(void)fdatasync(ifd);
#else
	(void)fsync(ifd);
#endif

	if (fstat(dfd, &sbuf) < 0) {
		fprintf(stderr, "%s: Can't stat %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
		exit(EXIT_FAILURE);
	}
	params.file_size = sbuf.st_size;

	map_len = sbuf.st_size;
	ptr = mmap(0, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, ifd, 0);
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "%s: Can't map %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	s32cc_set_header(ptr, &sbuf, ifd, &params);
	s32cc_print_header(ptr);

	(void)munmap((void *)ptr, map_len);

	/* We're a bit of paranoid */
#if defined(_POSIX_SYNCHRONIZED_IO) && !defined(__sun__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) &&          \
	!defined(__APPLE__)
	(void)fdatasync(ifd);
#else
	(void)fsync(ifd);
#endif

	close(dfd);

	if (close(ifd)) {
		fprintf(stderr, "%s: Write error on %s: %s\n", params.cmdname, params.imagefile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

