#ifndef __GST_EQUIRECT_BLUR_H__
#define __GST_EQUIRECT_BLUR_H__

#include <gst/gst.h>
#include <gst/video/gstvideofilter.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include "PCN.h"
#include "equirect-blur-common.h"

G_BEGIN_DECLS

#define GST_TYPE_EQUIRECT_BLUR (gst_equirect_blur_get_type())
#define GST_EQUIRECT_BLUR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_EQUIRECT_BLUR, GstEquirectBlur))

typedef struct _GstEquirectBlur GstEquirectBlur;
typedef struct _GstEquirectBlurClass GstEquirectBlurClass;

struct _GstEquirectBlur {
    GstVideoFilter parent;
    gchar *models_dir;
    gboolean draw_over_faces;
    gint width, height;
    cv::Mat cvMat;
    gboolean update_projections;
    std::vector<Projection> projections;
};

struct _GstEquirectBlurClass {
    GstVideoFilterClass parent_class;
};

GType gst_equirect_blur_get_type(void);
gboolean gst_equirect_blur_register(void);

G_END_DECLS

#endif /* __GST_EQUIRECT_BLUR_H__ */
