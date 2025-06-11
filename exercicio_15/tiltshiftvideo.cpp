#include <iostream>
#include <opencv2/opencv.hpp>

int focus_height_slider = 50;
int focus_height_slider_max = 100;

int blur_decay_slider = 50;
int blur_decay_slider_max = 100;

int vertical_position_slider = 50;
int vertical_position_slider_max = 100;

cv::Mat apply_tiltshift_effect(const cv::Mat& frame) {
    int focus_height = (frame.rows * focus_height_slider) / focus_height_slider_max;
    int focus_center = (frame.rows * vertical_position_slider) / vertical_position_slider_max;
    int top_limit = std::max(0, focus_center - focus_height / 2);
    int bottom_limit = std::min(frame.rows, focus_center + focus_height / 2);

    int decay = (blur_decay_slider * 50) / blur_decay_slider_max;
    cv::Mat top_blur, bottom_blur, result = frame.clone();

    // Aplicar desfoque nas regiões superior e inferior
    cv::GaussianBlur(frame(cv::Rect(0, 0, frame.cols, top_limit)), top_blur, cv::Size(decay | 1, decay | 1), 0);
    cv::GaussianBlur(frame(cv::Rect(0, bottom_limit, frame.cols, frame.rows - bottom_limit)), bottom_blur, cv::Size(decay | 1, decay | 1), 0);

    top_blur.copyTo(result(cv::Rect(0, 0, frame.cols, top_limit)));
    bottom_blur.copyTo(result(cv::Rect(0, bottom_limit, frame.cols, frame.rows - bottom_limit)));

    return result;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_video> <output_video>" << std::endl;
        return -1;
    }

    cv::VideoCapture capture(argv[1]);
    if (!capture.isOpened()) {
        std::cerr << "Error opening video file " << argv[1] << std::endl;
        return -1;
    }

    int frame_width = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(capture.get(cv::CAP_PROP_FPS));

    // Criar o vídeo de saída com a mesma resolução, mas reduzir o FPS para o efeito de stop motion
    cv::VideoWriter output(argv[2], cv::VideoWriter::fourcc('M','J','P','G'), fps / 4, cv::Size(frame_width, frame_height));

    if (!output.isOpened()) {
        std::cerr << "Error opening output video file " << argv[2] << std::endl;
        return -1;
    }

    int frame_counter = 0;
    cv::Mat frame, processed_frame;

    while (capture.read(frame)) {
        if (frame_counter % 4 == 0) {  // Descartar 3 em cada 4 quadros para efeito de stop motion
            processed_frame = apply_tiltshift_effect(frame);
            output.write(processed_frame);
            cv::imshow("Tilt-Shift Effect", processed_frame);

            if (cv::waitKey(10) == 27) { // Pressionar 'ESC' para sair
                break;
            }
        }
        frame_counter++;
    }

    capture.release();
    output.release();
    cv::destroyAllWindows();

    std::cout << "Processed video saved as " << argv[2] << std::endl;

    return 0;
}

// g++ tiltshiftvideo.cpp -o tiltshiftvideo -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc
// ./tiltshiftvideo input_video.mp4 output_video.mp4
