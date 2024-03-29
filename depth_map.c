/*
 *
 * Stereoscopic Vision
 *
 * Copyright (c) 2021 Robert Clayton and/or the Regents of the University of California.
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */


#include "calc_depth_optimized.h"
#include "utils.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define USAGE "\
USAGE: %s [options] \n\
\n\
REQUIRED \n\
    -l [LEFT_IMAGE]       The left image\n\
    -r [RIGHT_IMAGE]      The right image\n\
    -w [WIDTH_PIXELS]     The width of the smallest feature\n\
    -h [HEIGHT_PIXELS]    The height of the smallest feature\n\
    -t [MAX_DISPLACE]     The threshold for maximum displacement\n\
\n\
OPTIONAL \n\
    -o [OUTPUT_IMAGE]     Draw output to this file\n\
    -v                    Print the output to stdout as readable bytes\n\
"

int main(int argc, char **argv) {
    char *left_file = NULL;
    char *right_file = NULL;
    char *out_file = NULL;

    int feature_width = -1;
    int feature_height = -1;
    int maximum_displacement = -1;

    bool verbose = false;
    for (size_t i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'l':
                    if (argc > i + 1) left_file = argv[++i];
                    break;
                case 'r':
                    if (argc > i + 1) right_file = argv[++i];
                    break;
                case 'o':
                    if (argc > i + 1) out_file = argv[++i];
                    break;
                case 'w':
                    if (argc > i + 1) feature_width = atoi(argv[++i]);
                    break;
                case 'h':
                    if (argc > i + 1) feature_height = atoi(argv[++i]);
                    break;
                case 't':
                    if (argc > i + 1) maximum_displacement = atoi(argv[++i]);
                    break;
                case 'v':
            verbose = true;
                    break;
                default:
                    printf("Unknown option: %s\n", argv[i]);
                    exit(EINVAL);
            }
        }
    }
    if (left_file == NULL || right_file == NULL
            || feature_width == -1 || feature_height == -1
            || maximum_displacement == -1) {
        printf(USAGE, argv[0]);
        exit(EINVAL);
    }
    Image left_image = load_image(left_file);
    Image right_image = load_image(right_file);
    if (left_image.height != right_image.height
            || left_image.width != right_image.width) {
        printf("The two images do not have the same dimensions.\n");
        exit(EINVAL);
    }

    Image depth;
    depth.width = left_image.width;
    depth.height = right_image.height;
    depth.data = (unsigned char*) malloc(depth.width * depth.height);
    if (!depth.data) {
        allocation_failed();
    }
    srand((unsigned int) time(NULL));
    for (size_t i = 0; i < depth.width * depth.height; i++) {
        depth.data[i] = rand();
    }

    float* depth_result = malloc(
            sizeof(float) * depth.width * depth.height);
    float* left_image_data = malloc(
            sizeof(float) * left_image.width * left_image.height);
    float* right_image_data = malloc(
            sizeof(float) * right_image.width * right_image.height);

    char_to_float(left_image_data, left_image.data,
            left_image.width * left_image.height);
    char_to_float(right_image_data, right_image.data,
            right_image.width * right_image.height);

    calc_depth_optimized(depth_result, left_image_data, right_image_data,
            left_image.width, left_image.height, feature_width, feature_height,
            maximum_displacement);

    float_to_char(depth.data, depth_result, depth.width * depth.height,
            maximum_displacement);

    if (out_file != NULL) {
        save_image_with_depth(out_file, left_image.data, depth.data,
                left_image.width, left_image.height, feature_width,
                feature_height);
    }

    free(left_image.data);
    free(right_image.data);

    if (verbose) {
        print_image(depth.data, depth.width, depth.height);
    }

    free(depth.data);
    return 0;
}
