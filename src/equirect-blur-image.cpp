#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <stdexcept>
#include <fstream>
#include "config.h"
#include "equirect-blur-common.h"

using namespace cv;
using namespace std;

int main(int argc, const char** argv) {
    try {
        string model_arg = "{models-dir m|" + string(MODELS_DATADIR) + "|Path to PCN models}";
        CommandLineParser parser(argc, argv, model_arg + "{help h||}" "{blur b||}" "{output-file o|output.jpg|Output file}" "{@input-file|test.jpg|Input file}");

        if (parser.get<bool>("help")) {
            parser.printMessage();
            return 0;
        }

        String input_file = parser.get<String>("@input-file");
        String output_file = parser.get<String>("output-file");
        String models_dir = parser.get<String>("models-dir");
        bool draw_over_faces = !parser.has("blur");

        Mat im = imread(input_file);
        if (im.empty()) {
            throw runtime_error("Can't open image file " + input_file);
        }

        auto detector = make_shared<PCN>(models_dir + "/PCN.caffemodel",
                                         models_dir + "/PCN-1.prototxt",
                                         models_dir + "/PCN-2.prototxt",
                                         models_dir + "/PCN-3.prototxt",
                                         models_dir + "/PCN-Tracking.caffemodel",
                                         models_dir + "/PCN-Tracking.prototxt");

        detector->SetMinFaceSize(20);
        detector->SetImagePyramidScaleFactor(1.25f);
        detector->SetDetectionThresh(0.37f, 0.43f, 0.85f);
        detector->SetTrackingPeriod(30);
        detector->SetTrackingThresh(0.9f);
        detector->SetVideoSmooth(true);

        // Detect faces in the image
        std::vector<Window> faces = detector->Detect(im);
        if (faces.empty()) {
            // If no faces are detected, copy the input file to the output file
            ifstream src(input_file, std::ios::binary);
            ofstream dst(output_file, std::ios::binary);
            dst << src.rdbuf();
            src.close();
            dst.close();
            return 0; // Return as no further processing required
        }

        float apertures[2] = { X_APERTURE, Y_APERTURE };
        cv::Size image_size(im.cols, im.rows);
        std::vector<Projection> projections;

        for (int phi_step = 0; phi_step < (int)(M_PI / Y_STEP); phi_step++) {
            float phi_full = phi_step * Y_STEP;
            float phi = phi_full <= M_PI / 2 ? phi_full : phi_full - M_PI;

            for (float lambda = 0; lambda < 2 * M_PI; lambda += X_STEP) {
                Projection projection(image_size, apertures, phi, lambda, detector.get());
                projections.push_back(projection);
            }
        }

        cout << "Processing frame" << endl;

        if (equirect_blur_process_frame(im, projections, draw_over_faces)) {
            std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 94};
            if (!imwrite(output_file, im, params)) {
                throw runtime_error("Can't write image file " + output_file);
            }
        } else {
            throw runtime_error("Processing frame failed " + output_file);
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    }
}
