#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <chrono>
#include <random>

#define STEP 5
#define JITTER 3
#define MAX_RADIUS 10
#define MIN_RADIUS 2

int top_slider = 10;
int top_slider_max = 200;
int point_size = 5; // Tamanho inicial do ponto
int point_size_max = 20; // Tamanho máximo do ponto
cv::Mat image, border, points;

void on_trackbar_canny(int, void*) {
    // Aplica o algoritmo de Canny para detectar bordas
    cv::Canny(image, border, top_slider, 3 * top_slider);
    cv::imshow("Canny", border);

    // Limpar a imagem de pontos anterior
    points.setTo(cv::Scalar(255));

    // Gerar pontos de acordo com as novas bordas
    int width = image.cols;
    int height = image.rows;
    int x, y, gray;

    // Variar os limiares de Canny
    for (int threshold = top_slider; threshold <= top_slider_max; threshold += 10) {
        // Aplica o algoritmo de Canny com o limiar atual
        cv::Canny(image, border, threshold, 3 * threshold);

        // Calcular o tamanho do círculo a partir do slider de tamanho do ponto
        int circle_radius = point_size; // O tamanho do ponto é agora controlado pelo slider

        // Percorrer os pontos e desenhar círculos nas posições de borda
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                // Verificar se o pixel está na borda
                if (border.at<uchar>(j, i) > 0) {
                    // Verificar se a posição está dentro dos limites da imagem
                    x = i + std::rand() % (2 * JITTER) - JITTER;
                    y = j + std::rand() % (2 * JITTER) - JITTER;

                    // Garantir que as posições x e y estão dentro dos limites
                    x = std::min(std::max(x, 0), width - 1);
                    y = std::min(std::max(y, 0), height - 1);

                    // Obter o valor de cinza para o ponto
                    gray = image.at<uchar>(y, x);

                    // Desenhar um círculo com base na intensidade do pixel e no limiar
                    cv::circle(points, cv::Point(x, y), circle_radius, CV_RGB(gray, gray, gray), cv::FILLED, cv::LINE_AA);
                }
            }
        }
    }

    // Exibir imagem final com pontilhismo
    cv::imshow("Pontilhismo com Canny", points);
}

void on_trackbar_size(int, void*) {
    // Recalcular o pontilhismo com o novo tamanho de ponto
    on_trackbar_canny(top_slider, 0);
}

int main(int argc, char** argv) {
    // Carregar a imagem em escala de cinza
    image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    // Inicializar variáveis
    int width = image.cols;
    int height = image.rows;

    points = cv::Mat(height, width, CV_8U, cv::Scalar(255)); // Matriz para os pontos

    // Criar a janela para o controle do limiar
    cv::namedWindow("Canny", 1);
    cv::createTrackbar("Threshold Inferior", "Canny", &top_slider, top_slider_max, on_trackbar_canny);

    // Criar a janela para o controle do tamanho do ponto
    cv::createTrackbar("Tamanho dos Pontos", "Canny", &point_size, point_size_max, on_trackbar_size);

    // Gerar bordas iniciais e imagem pontilhista
    on_trackbar_canny(top_slider, 0);

    // Esperar pela tecla para finalizar
    cv::waitKey();
    
    return 0;
}
