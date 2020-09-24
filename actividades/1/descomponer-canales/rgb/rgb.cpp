#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#define MAX_COLOR 256
#define LARGO_FOTO 256*2
#define ALTURA_FOTO 256
#define OFFSET 150

// B G R
const cv::Scalar arreglo_colores[3] = {
    cv::Scalar(255, 0, 0),
    cv::Scalar(0, 255, 0),
    cv::Scalar(0, 0, 255)
};

void histograma_canal(cv::Mat entrada, cv::Mat salida, int canal) {
    int histograma[MAX_COLOR];

    // setear en cero el histograma
    for (int i = 0; i < MAX_COLOR; i++) {
        histograma[i] = 0;
    }

    // solo aceptar imágenes de 8-bits
    CV_Assert(entrada.depth() == CV_8U);

    int filas_entrada = entrada.rows;
    int columnas_entrada = entrada.cols;

    // completar el histograma, con los píxeles de la imagen
    for(int i = 0; i < filas_entrada; i++)
        for (int j = 0; j < columnas_entrada; j += 3)
            histograma[int(entrada.at<uchar>(i,j+canal))]++;

    int maximo = *std::max_element(histograma, histograma+MAX_COLOR);

    // volcar la información del arreglo histograma a una foto
    // for (int i = 0; i < MAX_COLOR; i++) {
    //     cv::rectangle(
    //         salida,
    //         cv::Point(i*ANCHO_BARRA,ALTURA_FOTO),
    //         cv::Point(i*ANCHO_BARRA+ANCHO_BARRA, ALTURA_FOTO-(histograma[i]*ALTURA_FOTO/maximo)),
    //         arreglo_colores[canal],
    //         1,
    //         cv::LINE_8
    //     );
    // }

    cv::Point anterior, siguiente;
    anterior = cv::Point(0,ALTURA_FOTO-1);
    for (int i = 0; i < MAX_COLOR; i++) {
        // cv::rectangle(imagen_histograma,
        //     cv::Point(i*ANCHO_BARRA,ALTURA_FOTO),
        //     cv::Point(i*ANCHO_BARRA+ANCHO_BARRA, ALTURA_FOTO-(histograma[i]*ALTURA_FOTO/maximo)),
        //     cv::Scalar(COLOR_HISTOGRAMA),
        //     1,
        //     cv::LINE_8
        // );
        siguiente = cv::Point(i*LARGO_FOTO/MAX_COLOR, ALTURA_FOTO-1-(histograma[i]*ALTURA_FOTO/maximo));
        cv::line(
            salida,
            anterior,
            siguiente,
            arreglo_colores[canal],
            1,
            cv::LINE_8,
            0
        );
        anterior = siguiente;
    }
}

int main(int argc, char ** argv) {
    // obtener imagen
    cv::Mat imagen = imread(argv[1], cv::IMREAD_COLOR);
    if(imagen.empty()) {
        std::cout << "No se pudo encontrar la imagen." << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat imagen_histogramas = cv::Mat::zeros(ALTURA_FOTO, LARGO_FOTO, CV_8UC3);;
    std::string ventana_histogramas = "Descomposicion de canales";

    for (int i = 0; i < 3; i++) {
        histograma_canal(imagen, imagen_histogramas, i);
    }

    imshow(ventana_histogramas, imagen_histogramas);
    cv::waitKey();

    return EXIT_SUCCESS;
}
