#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <template_image> <target_image>" << std::endl;
    return -1;
  }

  // Carrega as imagens fornecidas nos argumentos
  cv::Mat templateImage = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  cv::Mat targetImage = cv::imread(argv[2], cv::IMREAD_GRAYSCALE);

  if (templateImage.empty() || targetImage.empty()) {
    std::cerr << "Error: Could not load one or both images." << std::endl;
    return -1;
  }

  // Redimensiona as imagens para melhorar o desempenho
  cv::resize(templateImage, templateImage, cv::Size(), 0.5, 0.5);
  cv::resize(targetImage, targetImage, cv::Size(), 0.5, 0.5);

  // Detecta o template na imagem alvo
  cv::Mat result;
  cv::matchTemplate(targetImage, templateImage, result, cv::TM_CCOEFF_NORMED);

  // Encontra a posição com maior correspondência
  double minVal, maxVal;
  cv::Point minLoc, maxLoc;
  cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

  // Calcula o retângulo correspondente ao template
  cv::Rect matchRect(maxLoc.x, maxLoc.y, templateImage.cols, templateImage.rows);

  // Mostra os resultados no terminal
  std::cout << "Position of template (x, y): (" << maxLoc.x << ", " << maxLoc.y << ")" << std::endl;

  // Recorta a região correspondente ao template encontrado
  cv::Mat croppedResult = targetImage(matchRect);

  // Salva somente a parte correspondente ao template encontrado
  cv::imwrite("resultado.jpg", croppedResult);

  // Exibe a imagem processada (opcional, pode ser removido para execução em modo headless)
  cv::imshow("Matched Image", croppedResult);
  cv::waitKey(0);

  return 0;
}
