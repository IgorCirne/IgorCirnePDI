#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>

double alfa;
int alfa_slider = 0;
int alfa_slider_max = 100;

int top_slider = 0;
int top_slider_max = 100;

int focus_height_slider = 50;
int focus_height_slider_max = 100;

int blur_decay_slider = 50;
int blur_decay_slider_max = 100;

int vertical_position_slider = 50;
int vertical_position_slider_max = 100;

cv::Mat image1, image2, blended, focused;
cv::Mat imageTop; 

char TrackbarName[50];

void apply_blur_with_decay() {
    // Definir a altura e a posição do centro da área em foco
    int focus_height = (image1.rows * focus_height_slider) / focus_height_slider_max;
    int focus_center = (image1.rows * vertical_position_slider) / vertical_position_slider_max;

    // Calcular os limites superior e inferior da área em foco
    int top_limit = std::max(0, focus_center - focus_height / 2);
    int bottom_limit = std::min(image1.rows, focus_center + focus_height / 2);

    // Aplicar desfoque nas regiões superior e inferior com intensidade de decaimento
    int decay = (blur_decay_slider * 50) / blur_decay_slider_max;
    cv::Mat top_blur, bottom_blur;
    cv::GaussianBlur(image1(cv::Rect(0, 0, image1.cols, top_limit)), top_blur, cv::Size(decay | 1, decay | 1), 0);
    cv::GaussianBlur(image1(cv::Rect(0, bottom_limit, image1.cols, image1.rows - bottom_limit)), bottom_blur, cv::Size(decay | 1, decay | 1), 0);

    // Copiar as regiões borradas e a região de foco para uma nova imagem
    focused = image1.clone();
    top_blur.copyTo(focused(cv::Rect(0, 0, image1.cols, top_limit)));
    bottom_blur.copyTo(focused(cv::Rect(0, bottom_limit, image1.cols, image1.rows - bottom_limit)));
}

void on_trackbar_blend(int, void*) {
    alfa = (double)alfa_slider / alfa_slider_max;
    cv::addWeighted(focused, 1 - alfa, imageTop, alfa, 0.0, blended);
    cv::imshow("addweighted", blended);
}

void on_trackbar_line(int, void*) {
    apply_blur_with_decay();
    image1.copyTo(imageTop);
    int limit = top_slider * image2.rows / 100;
    if (limit > 0) {
        cv::Mat tmp = image2(cv::Rect(0, 0, image2.cols, limit));
        tmp.copyTo(imageTop(cv::Rect(0, 0, image2.cols, limit)));
    }
    on_trackbar_blend(alfa_slider, 0);
}

void on_trackbar_focus_height(int, void*) {
    apply_blur_with_decay();
    on_trackbar_line(top_slider, 0);
}

void on_trackbar_blur_decay(int, void*) {
    apply_blur_with_decay();
    on_trackbar_line(top_slider, 0);
}

void on_trackbar_vertical_position(int, void*) {
    apply_blur_with_decay();
    on_trackbar_line(top_slider, 0);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <image1> <image2>" << std::endl;
        return -1;
    }
    
    image1 = cv::imread(argv[1]);
    image2 = cv::imread(argv[2]);
    image2.copyTo(imageTop);

    if (image1.empty() || image2.empty()) {
        std::cerr << "Could not open or find the images!" << std::endl;
        return -1;
    }

    cv::namedWindow("addweighted", 1);

    // Trackbar para alpha
    std::sprintf(TrackbarName, "Alpha x %d", alfa_slider_max);
    cv::createTrackbar(TrackbarName, "addweighted", &alfa_slider, alfa_slider_max, on_trackbar_blend);
    on_trackbar_blend(alfa_slider, 0);

    // Trackbar para linha de escaneamento
    std::sprintf(TrackbarName, "Scanline x %d", top_slider_max);
    cv::createTrackbar(TrackbarName, "addweighted", &top_slider, top_slider_max, on_trackbar_line);
    on_trackbar_line(top_slider, 0);

    // Trackbar para altura da região em foco
    std::sprintf(TrackbarName, "Focus Height x %d", focus_height_slider_max);
    cv::createTrackbar(TrackbarName, "addweighted", &focus_height_slider, focus_height_slider_max, on_trackbar_focus_height);
    on_trackbar_focus_height(focus_height_slider, 0);

    // Trackbar para intensidade de decaimento do desfoque
    std::sprintf(TrackbarName, "Blur Decay x %d", blur_decay_slider_max);
    cv::createTrackbar(TrackbarName, "addweighted", &blur_decay_slider, blur_decay_slider_max, on_trackbar_blur_decay);
    on_trackbar_blur_decay(blur_decay_slider, 0);

    // Trackbar para posição vertical do centro da região em foco
    std::sprintf(TrackbarName, "Vertical Position x %d", vertical_position_slider_max);
    cv::createTrackbar(TrackbarName, "addweighted", &vertical_position_slider, vertical_position_slider_max, on_trackbar_vertical_position);
    on_trackbar_vertical_position(vertical_position_slider, 0);

    cv::waitKey(0);

    // Salvar a imagem final em um arquivo
    cv::imwrite("blended_output.jpg", blended);

    return 0;
}
