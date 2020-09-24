#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#define MAX_COLOR 256
#define ANCHO_BARRA 2
#define FILAS_IMAGEN 350

int main(int argc, char ** argv) {

    // chequeo de argumentos
    if (argc == 1) {
        std::cout << "Debe indicar el camino a una imagen.";
        return EXIT_FAILURE;
    }

    // obtener imagen
    cv::Mat imagen = imread(argv[1], cv::IMREAD_GRAYSCALE);
    if(imagen.empty()) {
        std::cout << "No se pudo encontrar la imagen.";
        return EXIT_FAILURE;
    }

    // solo aceptar imágenes de 8-bits
    // CV_Assert(imagen.depth() == CV_8U);

    cv::Mat canal, histograma;
    cv::extractChannel(imagen, canal, 0);
    float rango[] = { 0, 256 }; //the upper boundary is exclusive
    const float* rangoHist = { rango };
    int size_histograma = 256;
    cv::calcHist(&canal, 1, 0, cv::Mat(), histograma, 1, &size_histograma, &rangoHist, true, false);

    int ancho_histograma = 256*2, alto_histograma = 256;
    int bin_w = cvRound((double) ancho_histograma/size_histograma );
    cv::Mat imagen_histograma(alto_histograma, ancho_histograma, CV_8U, cv::Scalar(0));

    cv::normalize(histograma, histograma, 0, imagen_histograma.rows, cv::NORM_MINMAX, -1, cv::Mat());

    for(int i = 1; i < size_histograma; i++) {
        line(imagen_histograma, cv::Point( bin_w*(i-1), alto_histograma - cvRound(histograma.at<float>(i-1)) ),
              cv::Point( bin_w*(i), alto_histograma - cvRound(histograma.at<float>(i)) ),
              cv::Scalar(255), 2, 8, 0
        );
    }

    cv::imshow("Histograma", imagen_histograma );
    cv::waitKey();

    return EXIT_SUCCESS;
}
