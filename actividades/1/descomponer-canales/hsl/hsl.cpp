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

    // obtener imagen
    cv::Mat imagen = imread(argv[1], cv::IMREAD_COLOR);
    if(imagen.empty()) {
        std::cout << "No se pudo encontrar la imagen." << std::endl;
        return EXIT_FAILURE;
    }

    cv::Mat imagenes_canales[imagen.channels()];
    std::string ventanas_canales[imagen.channels()];
    cv::Mat imagenes_canales_cambiados[imagen.channels()];

    // obtener canales de la imagen y guardarlos como GBR en lugar de RGB
    for (int i = 0; i < imagen.channels(); i++) {
        imagenes_canales_cambiados[i] = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8U);
        extraer_canal(imagen, imagenes_canales_cambiados[i], (i+1)%imagen.channels());
    }

    cv::cvtColor(imagen, imagen, cv::COLOR_BGR2HSV, 3);

    ventanas_canales[0] = "Canal H";
    ventanas_canales[1] = "Canal S";
    ventanas_canales[2] = "Canal L";

    // obtener canales de la imagen HSL y mostrar cada uno por separado
    for (int i = 0; i < imagen.channels(); i++) {
        imagenes_canales[i] = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8U);
        extraer_canal(imagen, imagenes_canales[i], i);
        namedWindow(ventanas_canales[i], cv::WINDOW_NORMAL);
        imshow(ventanas_canales[i], imagenes_canales[i]);
    }

    cv::Mat imagen_reconstruida = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8UC3);
    cv::Mat imagen_reconstruida_cambiados = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8UC3);

    // reconstruir la foto:
    // - correctamente: de H,S,L a HSL
    // - incorrectamente: de R,G,B a GBR
    for (int i = 0; i < imagen.channels(); i++) {
        sumar_canal(imagenes_canales[i], imagen_reconstruida, i);
        sumar_canal(imagenes_canales_cambiados[i], imagen_reconstruida_cambiados, i);
    }

    // graficar la foto en HSL como si fuera RGB
    namedWindow("Canales HSL como RGB", cv::WINDOW_NORMAL);
    imshow("Canales HSL como RGB", imagen_reconstruida);

    // convertir de HSL a RGB
    cv::cvtColor(imagen_reconstruida, imagen_reconstruida, cv::COLOR_HSV2BGR, 3);

    // graficar la imagen con los canales mezclados
    namedWindow("Imagen GBR", cv::WINDOW_NORMAL);
    imshow("Imagen GBR", imagen_reconstruida_cambiados);

    // graficar la imagen reconstruida correctamente
    namedWindow("Imagen reconstruida", cv::WINDOW_NORMAL);
    imshow("Imagen reconstruida", imagen_reconstruida);

    // esperar cualquier input para salir
    cv::waitKey();

    return EXIT_SUCCESS;
}
