#include <iostream>
#include <opencv2/opencv.hpp>

int main(int, char**) {
    cv::Mat image;
    cv::Vec3b val;

    image = cv::imread("biel.png", cv::IMREAD_GRAYSCALE);
    if(!image.data) std::cout <<"Não abriu o arquivo.png" << std::endl;

    cv::namedWindow("Janela", cv::WINDOW_AUTOSIZE);

    //Obtendo o tamanho da imagem
    int rows = image.rows;
    int cols = image.cols;

    //Encontrar o meio dividindo por 2
    int half_rows = rows / 2;
    int half_cols = cols / 2;

    //Agora inverter os quadrantes da imagem
    cv::Mat tmp;
    cv::Mat quad_1 = image(cv::Rect(0, 0, half_cols, half_rows));
    cv::Mat quad_2 = image(cv::Rect(half_cols, 0, half_cols, half_rows));
    cv::Mat quad_3 = image(cv::Rect(0, half_rows, half_cols, half_rows));
    cv::Mat quad_4 = image(cv::Rect(half_cols, half_rows, half_cols, half_rows));

    quad_1.copyTo(tmp);
    quad_4.copyTo(quad_1);
    tmp.copyTo(quad_4);

    quad_2.copyTo(tmp);
    quad_3.copyTo(quad_2);
    tmp.copyTo(quad_3);

    cv::imshow("Janela", image);
    cv::waitKey();
    return 0;
}