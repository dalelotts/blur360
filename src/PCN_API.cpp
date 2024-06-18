#include "PCN_API.h"
#include <opencv2/opencv.hpp>
#include <vector>

void from_window(struct CWindow* cwin, const struct Window* win) {
    cwin->x = win->x;
    cwin->y = win->y;
    cwin->width = win->width;
    cwin->angle = win->angle;
    cwin->score = win->score;
    for (size_t f = 0; f < win->points14.size(); f++) {
        cwin->points[f].x = win->points14[f].x;
        cwin->points[f].y = win->points14[f].y;
    }
}

extern "C" {

void* init_detector(const char *detection_model_path,
                    const char *pcn1_proto, const char *pcn2_proto, const char *pcn3_proto,
                    const char *tracking_model_path, const char *tracking_proto,
                    int min_face_size, float pyramid_scale_factor, float detection_thresh_stage1,
                    float detection_thresh_stage2, float detection_thresh_stage3, int tracking_period,
                    float tracking_thresh, int do_smooth) {
    PCN* detector = new PCN(detection_model_path, pcn1_proto, pcn2_proto, pcn3_proto,
                            tracking_model_path, tracking_proto);

    // Detection settings
    detector->SetMinFaceSize(min_face_size);
    detector->SetImagePyramidScaleFactor(pyramid_scale_factor);
    detector->SetDetectionThresh(detection_thresh_stage1,
                                 detection_thresh_stage2,
                                 detection_thresh_stage3);
    // Tracking settings
    detector->SetTrackingPeriod(tracking_period);
    detector->SetTrackingThresh(tracking_thresh);
    detector->SetVideoSmooth(static_cast<bool>(do_smooth));
    return static_cast<void*>(detector);
}

struct CWindow* detect_faces(void* pcn, unsigned char* raw_img, size_t rows, size_t cols, int *lwin) {
    PCN* detector = static_cast<PCN*>(pcn);
    cv::Mat img(rows, cols, CV_8UC3, raw_img);
    std::vector<struct Window> faces = detector->Detect(img);

    *lwin = faces.size();
    struct CWindow* wins = static_cast<struct CWindow*>(malloc(sizeof(struct CWindow) * (*lwin)));
    for (int i = 0; i < *lwin; i++) {
        from_window(&wins[i], &faces[i]);
    }
    return wins;
}

struct CWindow* detect_track_faces(void* pcn, unsigned char* raw_img, size_t rows, size_t cols, int *lwin) {
    PCN* detector = static_cast<PCN*>(pcn);
    cv::Mat img(rows, cols, CV_8UC3, raw_img);
    std::vector<struct Window> faces = detector->DetectTrack(img);

    *lwin = faces.size();
    struct CWindow* wins = static_cast<struct CWindow*>(malloc(sizeof(struct CWindow) * (*lwin)));
    for (int i = 0; i < *lwin; i++) {
        from_window(&wins[i], &faces[i]);
    }
    return wins;
}

void free_faces(struct CWindow* wins) {
    free(wins);
}

void free_detector(void *pcn) {
    PCN* detector = static_cast<PCN*>(pcn);
    delete detector;
}

}
