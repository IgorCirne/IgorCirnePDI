#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv) {
  cv::Mat image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  
  if (!image.data) {
    std::cout << "imagem nao carregou corretamente\n";
    return -1;
  }

  int width = image.cols, height = image.rows;
  int nobjects = 0, nobjects_holes = 0, nobjects_no_holes = 0;
  cv::Point p;

  std::cout << width << " x " << height << " pixels" << std::endl;

  p.x = 0;
  p.y = 0;
  
  // Passo 1: Apagar as bolhas que tocam as bordas

  for (int i = 0; i < height; i++) {
      if (image.at<uchar>(i, 0) == 255) {
        p.x = 0;p.y = i;
        cv::floodFill(image, p, 0);
      }
      if (image.at<uchar>(i,width-1) == 255){
        p.x = width - 1; p.y = i;
        cv::floodFill(image, p, 0);
      }
  }

  for (int j = 0; j < width; j++) {
      if (image.at<uchar>(0, j) == 255) {
        p.x = j;p.y = 0;
        cv::floodFill(image, p, 0);
      }
      if (image.at<uchar>(height-1, j) == 255){
        p.x = j;p.y = height-1;
        cv::floodFill(image, p, 0);
      }
  }

  // Passo 2: Contar bolhas
  int label = 2;

  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
      if(image.at<uchar>(i,j) == 255){
        p.y = i; p.x = j;
        cv::floodFill(image, p, label++);
      }
    }
  }

  nobjects = label - 2;

   // Passo 3: Identificar bolhas com buracos
  std::vector<bool> has_hole(label, false);
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (image.at<uchar>(i, j) == 0) { // Encontrou um buraco
        bool found_object = false;
        for (int dx = -1; dx <= 1 && !found_object; dx++) {
          for (int dy = -1; dy <= 1 && !found_object; dy++) {
            int nx = j + dx, ny = i + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height && image.at<uchar>(ny, nx) > 1) {
              found_object = true;
              p.y = i; p.x = j;
              int obj_label = image.at<uchar>(ny, nx);
              cv::floodFill(image, p, obj_label);
              has_hole[obj_label] = true; // Marca a bolha como contendo um buraco
            }
          }
        }
      }
    }
  }

  // Contar bolhas com e sem buracos
  for (int i = 2; i < label; i++) {
    if (has_hole[i])
      nobjects_holes++;
    else
      nobjects_no_holes++;
  }

  std::cout << "Total de bolhas: " << nobjects << "\n";
  std::cout << "Bolhas com buracos: " << nobjects_holes << "\n";
  std::cout << "Bolhas sem buracos: " << nobjects_no_holes << "\n";
  
  cv::imshow("image", image);
  cv::imwrite("labeling.png", image);
  cv::waitKey();
  return 0;
}
