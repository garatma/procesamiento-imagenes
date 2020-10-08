// #include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#define CUANTIZADOR 24

int main(int argc, char ** argv)
{
	cv::Mat imagen;
	if (argc == 2) imagen = imread(argv[1], cv::IMREAD_COLOR);
	else {
		std::cout << "Debe especificar la ruta a una imagen." << std::endl;
		return EXIT_FAILURE;
	}

	if(imagen.empty()) {
		std::cout << "No se pudo encontrar la imagen." << std::endl;
		return EXIT_FAILURE;
	}

	// (1) - aplicar filtro de la mediana.
	cv::Mat filtrada;
	cv::medianBlur(imagen, filtrada, 7);

	// (2) - detectar bordes a la imagen en escala de grises. ¿operaciones para obtenerlo?
	cv::Mat laplaciano, grises;
	cv::cvtColor(imagen, grises, cv::COLOR_BGR2GRAY);
	cv::Laplacian(grises, laplaciano, CV_8U);
	// cv::dilate(laplaciano, laplaciano, 7);

	// (3) - reducir la cantidad de colores en (1).
	cv::Mat bilateral;
	cv::bilateralFilter(filtrada, bilateral, 9, 50, 50);
	uchar * fila_bilateral;
	for (int i = 0; i < bilateral.rows; ++i) {
		fila_bilateral = bilateral.ptr<uchar>(i);
		for (int j = 0; j < bilateral.cols*bilateral.channels(); ++j)
			fila_bilateral[j] = floor(fila_bilateral[j]/CUANTIZADOR)*CUANTIZADOR;
	}
	cv::medianBlur(bilateral, bilateral, 7);

	// (4) - agregar a (3) los bordes obtenidos en (2).
	cv::Mat resultado = bilateral.clone();

	// por ahora: si el píxel de (2) no es negro, pintar el de (3) en negro
	uchar * fila_resultado, * fila_laplaciano;
	for (int i = 0; i < laplaciano.rows; ++i) {
		fila_resultado = resultado.ptr<uchar>(i);
		fila_laplaciano = laplaciano.ptr<uchar>(i);
		for (int j = 0; j < laplaciano.cols; ++j) {
			fila_laplaciano[j] = 255-fila_laplaciano[j];
			if (fila_laplaciano[j] < 255-20) {
				for (int k = 0; k < resultado.channels(); ++k) {
					fila_resultado[j*resultado.channels()+k] = 0;
					fila_laplaciano[j] = 0;
				}
			}
			else fila_laplaciano[j] = 255;
		}
	}

	// TODO: poner en un único for-for los dos for-for que hay

	cv::namedWindow("Imagen entrada", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Imagen entrada", 500, 500);
	imshow("Imagen entrada", imagen);

	cv::namedWindow("Filtrada", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Filtrada", 500, 500);
	imshow("Filtrada", filtrada);

	cv::namedWindow("Laplaciano", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Laplaciano", 500, 500);
	imshow("Laplaciano", laplaciano);

	cv::namedWindow("Bilateral", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Bilateral", 500, 500);
	imshow("Bilateral", bilateral);

	cv::namedWindow("Resultado", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Resultado", 500, 500);
	imshow("Resultado", resultado);

	cv::waitKey();

	return EXIT_SUCCESS;
}
