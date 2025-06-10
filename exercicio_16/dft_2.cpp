#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

void swapQuadrants(cv::Mat& image) {
  cv::Mat tmp, A, B, C, D;

  // se a imagem tiver tamanho ímpar, recorta a região para o maior
  // tamanho par possível (-2 = 1111...1110)
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

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <arquivo YAML>" << std::endl;
    return EXIT_FAILURE;
  }

  cv::Mat image, padded, complexImage;
  std::vector<cv::Mat> planos;

  // Carrega a matriz armazenada no arquivo YAML
  cv::FileStorage fs(argv[1], cv::FileStorage::READ);
  if (!fs.isOpened()) {
    std::cerr << "Erro ao abrir o arquivo YAML: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }
  fs["mat"] >> image;
  fs.release();

  if (image.empty()) {
    std::cerr << "Erro: matriz vazia no arquivo YAML." << std::endl;
    return EXIT_FAILURE;
  }

  // Normaliza a imagem para o intervalo [0, 255] e converte para 8 bits
  cv::normalize(image, image, 0, 255, cv::NORM_MINMAX);
  image.convertTo(image, CV_8U);

  // Expande a imagem para o melhor tamanho para DFT
  int dft_M = cv::getOptimalDFTSize(image.rows);
  int dft_N = cv::getOptimalDFTSize(image.cols);
  cv::copyMakeBorder(image, padded, 0, dft_M - image.rows, 0,
                     dft_N - image.cols, cv::BORDER_CONSTANT,
                     cv::Scalar::all(0));

  // Prepara a matriz complexa
  planos.push_back(cv::Mat_<float>(padded));
  planos.push_back(cv::Mat::zeros(padded.size(), CV_32F));

  // Combina os planos em uma estrutura de dados complexa
  cv::merge(planos, complexImage);

  // Calcula a DFT
  cv::dft(complexImage, complexImage);
  swapQuadrants(complexImage);

  // Divide os planos real e imaginário
  cv::split(complexImage, planos);

  // Calcula o espectro de magnitude e fase
  cv::Mat magn, fase;
  cv::cartToPolar(planos[0], planos[1], magn, fase, false);
  cv::normalize(fase, fase, 0, 1, cv::NORM_MINMAX);

  // Calcula o espectro de magnitude
  cv::magnitude(planos[0], planos[1], magn);

  // Adiciona constante para evitar log(0) e calcula logaritmo
  magn += cv::Scalar::all(1);
  cv::log(magn, magn);
  cv::normalize(magn, magn, 0, 1, cv::NORM_MINMAX);

  // Exibe as imagens processadas
  cv::imshow("Imagem Original", image);
  cv::imshow("Espectro de Magnitude", magn);
  cv::imshow("Espectro de Fase", fase);

  cv::waitKey();
  return EXIT_SUCCESS;
}