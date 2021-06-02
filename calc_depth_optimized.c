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


#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "calc_depth_naive.h"
#include "calc_depth_optimized.h"
#include "utils.h"

inline float isquare_euclidean_distance(float a, float b) {
    int diff = a - b;
    return diff * diff;
}

inline void calc_depth_optimized(float *depth, float *left, float *right,
        int image_width, int image_height, int feature_width,
        int feature_height, int maximum_displacement) {
    // Naive implementation
    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            if (y < feature_height || y >= image_height - feature_height
                    || x < feature_width || x >= image_width - feature_width) {
                depth[y * image_width + x] = 0;
                continue;
            }
            float min_diff = -1;
            int min_dy = 0;
            int min_dx = 0;            
            for (int dy = -maximum_displacement; dy <= maximum_displacement; dy++) {
                for (int dx = -maximum_displacement; dx <= maximum_displacement; dx++) {
                    
		    if (y + dy - feature_height < 0
                            || y + dy + feature_height >= image_height
                            || x + dx - feature_width < 0
                            || x + dx + feature_width >= image_width) {
                        continue;
                    }
                    float squared_diff = 0;
		    __m128 register sd = _mm_setzero_ps();
                    for (int register box_y = -feature_height; box_y <= feature_height; box_y++) {
		        float register *left_y_image_width = left + ((y+box_y) * image_width) + x;
            		float register *right_y_image_width = right + ((y+dy+box_y) * image_width) + x + dx;
			int wCutoff = 2*feature_width/4*4 - feature_width;
			for (int register box_x = -feature_width; box_x < wCutoff; box_x += 4) {
                            __m128 register res = _mm_sub_ps( _mm_loadu_ps(left_y_image_width + box_x),
                            _mm_loadu_ps(right_y_image_width + box_x) );
                            res = _mm_mul_ps( res, res );
                            sd = _mm_add_ps( sd, res );
                        }
                        //Tail Case:
                        for (int box_x = wCutoff; box_x <= feature_width; box_x++)
                        {
                            squared_diff += isquare_euclidean_distance(
                                    *(left_y_image_width + box_x),
                                    *(right_y_image_width + box_x));    
                        }
                    }
		    float qDiff[4];
                    _mm_store_ps(qDiff, sd); 
                    squared_diff += qDiff[0] + qDiff[1] + qDiff[2] + qDiff[3];


                    if (min_diff == -1 || min_diff > squared_diff
                            || (min_diff == squared_diff
                            && displacement_naive(dx, dy) < displacement_naive(min_dx, min_dy))) {
                        min_diff = squared_diff;
                        min_dx = dx;
                        min_dy = dy;
                    }
                }
            }
            if (min_diff != -1) {
                if (maximum_displacement == 0) {
                    depth[y * image_width + x] = 0;
                } else {
                    depth[y * image_width + x] = displacement_naive(min_dx, min_dy);
                }
            } else {
                depth[y * image_width + x] = 0;
            }
        }
    }
}
