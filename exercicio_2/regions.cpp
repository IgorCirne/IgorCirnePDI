#include <iostream>
#include <opencv2/opencv.hpp>

int main(int, char**) {
  cv::Mat image;
  cv::Vec3b val;

  cv::Point P1, P2;

  image = cv::imread("biel.png", cv::IMREAD_GRAYSCALE);
  if (!image.data) std::cout << "nao abriu o arquivo.png" << std::endl;

  cv::namedWindow("janela", cv::WINDOW_AUTOSIZE);

  std::cout << "Digite as coordenadas para P1: (x e depois y)" << std::endl ;
  std::cin >> P1.x >> P1.y;
  std::cout << "Digite as coordenadas para P2: (x e depois y)" << std::endl ;
  std::cin >> P2.x >> P2.y;

  for (int i = P1.x; i < P2.x; i++) {
    for (int j = P1.y; j < P2.y; j++) {
        image.at<uchar>(i, j) = 255 - image.at<uchar>(i,j);
    }
  }

  cv::imshow("janela", image);
  cv::waitKey();
  return 0;
}
