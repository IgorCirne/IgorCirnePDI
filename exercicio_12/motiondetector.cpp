#include <iostream>
#include <opencv2/opencv.hpp>
#include "camera.hpp"

int main(int argc, char** argv) {
  cv::Mat image, gray_image, histCurrent, histPrevious;
  int camera;
  cv::VideoCapture cap;    //Adicionar o caminho para o dispositivo... EX: cv::VideoCapture cap("/dev/video2");  // Altere conforme necessário
  int nbins = 64;
  float range[] = {0, 255};
  const float* histrange = {range};
  bool uniform = true;
  bool accumulate = false;
  int key;

  // Modifique o valor para ajustar o alarme de movimento
  double alarm = 0.5;


  camera = cameraEnumerator();
  cap.open(camera);

  if (!cap.isOpened()) {
      std::cout << "Câmera indisponível";
      return -1;
  }
  
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

  std::cout << "Câmera configurada: largura = " << cap.get(cv::CAP_PROP_FRAME_WIDTH)
                               << ", altura = " << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;

  while(true){
    cap >> image;
    if (image.empty()) {
      std::cout << "Erro ao capturar imagem" << sdt::endl;
      break;
    }

    std::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

    cv::calcHist(&gray_image, 1, 0, cv::Mat(), histCurrent, 1, &nbins, &histrange, uniform, accumulate);
    cv::normalize(histCurrent, histCurrent, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    if (!histPrevious.empty()) {
      double similarity = cv::compareHist(histPrevious, histCurrent, cv::HISTCMP_CORREL);

      std::cout << "Similaridade: " << similarity << std::endl;

      if (similarity < alarm) {
        std::cout << "ALARME ATIVADO!!!" << std::endl;
      }
    }

    histPrevious = histCurrent.clone();

    cv::imshow("Imagem capturada", gray_image);

    key = cv::waitKey(30);
    if (key == 27) break;
  }

  return 0;
}
