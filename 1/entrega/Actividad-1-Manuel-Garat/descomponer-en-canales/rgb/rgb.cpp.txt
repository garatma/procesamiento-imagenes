#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#define OFFSET 400

void extraer_canal(cv::Mat entrada, cv::Mat salida, int canal) {
    uchar * fila_entrada, * fila_salida;
    int k;

    for(int i = 0; i < entrada.rows; i++) {
        fila_entrada = entrada.ptr<uchar>(i);
        fila_salida = salida.ptr<uchar>(i);
        k = 0;
        for (int j = 0; j < entrada.cols * entrada.channels(); j += entrada.channels())
            fila_salida[k++] = fila_entrada[j+canal];
    }
}

void sumar_canal(cv::Mat entrada, cv::Mat salida, int canal) {
    uchar * fila_entrada, * fila_salida;
    int k;

    for (int i = 0; i < entrada.rows; i++) {
        fila_entrada = entrada.ptr<uchar>(i);
        fila_salida = salida.ptr<uchar>(i);
        k = 0;
        for (int j = 0; j < entrada.cols; j++) {
            fila_salida[k++*salida.channels()+canal] = fila_entrada[j];
        }
    }
}

int main(int argc, char ** argv) {
    cv::Mat imagen = imread(argv[1], cv::IMREAD_COLOR);
    if(imagen.empty()) {
        std::cout << "No se pudo encontrar la imagen." << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat imagenes_canales[imagen.channels()];
    std::string ventanas_canales[imagen.channels()];

    ventanas_canales[0] = "Canal azul";
    ventanas_canales[1] = "Canal verde";
    ventanas_canales[2] = "Canal rojo";

    for (int i = 0; i < imagen.channels(); i++) {
        imagenes_canales[i] = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8U);
        extraer_canal(imagen, imagenes_canales[i], i);
        namedWindow(ventanas_canales[i], cv::WINDOW_NORMAL);
        imshow(ventanas_canales[i], imagenes_canales[i]);
        cv::moveWindow(ventanas_canales[i], i*OFFSET, 0);
    }

    cv::Mat imagen_reconstruida = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8UC3);

    for (int i = 0; i < imagen.channels(); i++) {
        sumar_canal(imagenes_canales[i], imagen_reconstruida, i);
    }

    namedWindow("Imagen reconstruida", cv::WINDOW_NORMAL);
    imshow("Imagen reconstruida", imagen_reconstruida);

    cv::waitKey();

    return EXIT_SUCCESS;
}
