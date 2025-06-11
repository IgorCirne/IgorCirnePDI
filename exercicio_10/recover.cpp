#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Uso: " << argv[0] << " <imagem_esteganografada>" << std::endl;
    return -1;
  }

  cv::Mat imagemEsteganografada, imagemRecuperada;
  cv::Vec3b valEsteganografada, valRecuperada;
  int nbits = 3;

  imagemEsteganografada = cv::imread(argv[1], cv::IMREAD_COLOR);

  if (imagemEsteganografada.empty()) {
    std::cout << "imagem nao carregou corretamente" << std::endl;
    return -1;
  }

  imagemRecuperada = imagemEsteganografada.clone();

  for (int i = 0; i < imagemEsteganografada.rows; i++) {
    for (int j = 0; j < imagemEsteganografada.cols; j++) {
      valEsteganografada = imagemEsteganografada.at<cv::Vec3b>(i, j);
      valRecuperada[0] = (valEsteganografada[0] & ((1 << nbits) - 1)) << (8 - nbits);
      valRecuperada[1] = (valEsteganografada[1] & ((1 << nbits) - 1)) << (8 - nbits);
      valRecuperada[2] = (valEsteganografada[2] & ((1 << nbits) - 1)) << (8 - nbits);
      imagemRecuperada.at<cv::Vec3b>(i, j) = valRecuperada;
    }
  }

  imwrite("imagem_recuperada.png", imagemRecuperada);
  return 0;
}
