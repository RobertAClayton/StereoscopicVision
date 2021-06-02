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


#ifndef CALC_DEPTH_OPTIMIZED_H
#define CALC_DEPTH_OPTIMIZED_H

 void calc_depth_optimized(float *depth, float *left, float *right,
        int image_width, int image_height, int feature_width,
        int feature_height, int maximum_displacement);

#endif
