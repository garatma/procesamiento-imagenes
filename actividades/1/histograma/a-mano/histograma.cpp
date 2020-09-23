#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#define MAX_COLOR 256
#define ANCHO_BARRA 2
#define FILAS_IMAGEN 350

int main(int argc, char ** argv) {

    int histograma[MAX_COLOR];

    // chequeo de argumentos
    if (argc == 1) {
        std::cout << "Debe indicar el camino a una imagen.";
        return EXIT_FAILURE;
    }

    int i;

    // setear en cero el histograma
    for (i = 0; i < MAX_COLOR; i++) {
        histograma[i] = 0;
    }

    // obtener imagen
    cv::Mat imagen = imread(argv[1], cv::IMREAD_GRAYSCALE);
    if(imagen.empty()) {
        std::cout << "No se pudo encontrar la imagen.";
        return EXIT_FAILURE;
    }

    // solo aceptar imágenes de 8-bits
    CV_Assert(imagen.depth() == CV_8U);

    int filas = imagen.rows;
    int columnas = imagen.cols;
    int j;

    // completar el histograma, con los píxeles de la imagen
    for(i = 0; i < filas; i++)
        for (j = 0; j < columnas; j++)
            histograma[int(imagen.at<uchar>(i,j))]++;

    char ventana_histograma[] = "Histograma";
    int maximo = *std::max_element(histograma, histograma+MAX_COLOR);

    // volcar la información del arreglo histograma a una foto
    cv::Mat imagen_histograma = cv::Mat::zeros(FILAS_IMAGEN, MAX_COLOR*2,  CV_8U);
    for (i = 0; i < MAX_COLOR; i++)
        cv::rectangle(imagen_histograma,
            cv::Point(i*ANCHO_BARRA,FILAS_IMAGEN),
            cv::Point(i*ANCHO_BARRA+ANCHO_BARRA, FILAS_IMAGEN-(histograma[i]*FILAS_IMAGEN/maximo)),
            cv::Scalar(255),
            cv::FILLED,
            cv::LINE_4);

    // mostrar foto y esperar cualquier input para cerrarla
    imshow(ventana_histograma, imagen_histograma);
    cv::waitKey();

    return EXIT_SUCCESS;
}
