#ifndef __PCN_API__
#define __PCN_API__

#include <stddef.h>  // For size_t
#include <stdlib.h>  // For malloc and free
#include "PCN.h"     // Ensure this header defines the struct Window

#ifdef __cplusplus
extern "C" {
#endif

    struct CPoint {
        int x;
        int y;
    };

#define kFeaturePoints 14
    struct CWindow {
        int x, y, width, angle;
        float score;
        struct CPoint points[kFeaturePoints];
    };

    void from_window(struct CWindow* cwin, const struct Window* win);

    void* init_detector(const char *detection_model_path,
                        const char *pcn1_proto, const char *pcn2_proto, const char *pcn3_proto,
                        const char *tracking_model_path, const char *tracking_proto,
                        int min_face_size, float pyramid_scale_factor, float detection_thresh_stage1,
                        float detection_thresh_stage2, float detection_thresh_stage3, int tracking_period,
                        float tracking_thresh, int do_smooth);

    struct CWindow* detect_faces(void* pcn, unsigned char* raw_img, size_t rows, size_t cols, int *lwin);

    struct CWindow* detect_track_faces(void* pcn, unsigned char* raw_img, size_t rows, size_t cols, int *lwin);

    void free_faces(struct CWindow* wins);

    void free_detector(void *pcn);

#ifdef __cplusplus
}
#endif

#endif
