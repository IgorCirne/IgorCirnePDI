#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

void swapQuadrants(cv::Mat& image) {
  cv::Mat tmp, A, B, C, D;

  // se a imagem tiver tamanho impar, recorta a regiao para o maior
  // tamanho par possivel (-2 = 1111...1110)
  image = image(cv::Rect(0, 0, image.cols & -2, image.rows & -2));

  int centerX = image.cols / 2;
  int centerY = image.rows / 2;

  // rearranja os quadrantes da transformada de Fourier de forma que 
  // a origem fique no centro da imagem
  // A B   ->  D C
  // C D       B A
  A = image(cv::Rect(0, 0, centerX, centerY));
  B = image(cv::Rect(centerX, 0, centerX, centerY));
  C = image(cv::Rect(0, centerY, centerX, centerY));
  D = image(cv::Rect(centerX, centerY, centerX, centerY));

  // swap quadrants (Top-Left with Bottom-Right)
  A.copyTo(tmp);
  D.copyTo(A);
  tmp.copyTo(D);

  // swap quadrant (Top-Right with Bottom-Left)
  C.copyTo(tmp);
  B.copyTo(C);
  tmp.copyTo(B);
}

void makeFilterInteractive(const cv::Mat& image, cv::Mat& filter, int& radius) {
    cv::Mat_<float> filter2D(image.rows, image.cols);
    int centerX = image.cols / 2;
    int centerY = image.rows / 2;

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (pow(i - centerY, 2) + pow(j - centerX, 2) <= pow(radius, 2)) {
                filter2D.at<float>(i, j) = 1;
            } else {
                filter2D.at<float>(i, j) = 0;
            }
        }
    }

    cv::Mat planes[] = {cv::Mat_<float>(filter2D), cv::Mat::zeros(filter2D.size(), CV_32F)};
    cv::merge(planes, 2, filter);
}

int main(int argc, char** argv) {
    cv::Mat image, padded, complexImage;
    std::vector<cv::Mat> planos;

    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <imagem_entrada>" << std::endl;
        return EXIT_FAILURE;
    }

    image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Erro ao carregar a imagem " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    int dft_M = cv::getOptimalDFTSize(image.rows);
    int dft_N = cv::getOptimalDFTSize(image.cols);
    cv::copyMakeBorder(image, padded, 0, dft_M - image.rows, 0, dft_N - image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    planos.push_back(cv::Mat_<float>(padded));
    planos.push_back(cv::Mat::zeros(padded.size(), CV_32F));
    cv::merge(planos, complexImage);
    cv::dft(complexImage, complexImage);

    swapQuadrants(complexImage);

    int radius = 20; // Valor inicial do raio
    cv::Mat filter;

    cv::namedWindow("Filtro Homomorfico", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Raio", "Filtro Homomorfico", &radius, 100, [](int, void*) {});

    while (true) {
        makeFilterInteractive(complexImage, filter, radius);
        cv::Mat filtered;
        cv::mulSpectrums(complexImage, filter, filtered, 0);

        swapQuadrants(filtered);
        cv::idft(filtered, filtered);
        cv::split(filtered, planos);

        cv::Rect roi(0, 0, image.cols, image.rows);
        cv::Mat result = planos[0](roi);

        cv::normalize(result, result, 0, 1, cv::NORM_MINMAX);
        cv::imshow("Filtro Homomorfico", result);

        char key = (char)cv::waitKey(30);
        if (key == 27) break; // Pressione ESC para sair
    }

    return EXIT_SUCCESS;
}
