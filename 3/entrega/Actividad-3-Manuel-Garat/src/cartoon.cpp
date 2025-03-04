// #include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#define CUANTIZADOR 30

int main(int argc, char ** argv)
{
	bool opcion_filtros = false;
	cv::Mat imagen;

	// procesar entrada.
	if (argc > 2) {
		opcion_filtros = !strcmp(argv[1], "-f") || !strcmp(argv[1], "--filtros");
		imagen = imread(argv[2], cv::IMREAD_COLOR);
	}
	else if (argc == 2) {
		imagen = imread(argv[1], cv::IMREAD_COLOR);
	}
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
	cv::medianBlur(imagen, filtrada, 9);

	// (2) - detectar bordes a la imagen en escala de grises.
	cv::Mat laplaciano, grises;
	cv::cvtColor(filtrada, grises, cv::COLOR_BGR2GRAY);
	cv::Laplacian(grises, laplaciano, CV_8U, 1, 1, 1);

	// (3) - reducir la cantidad de colores en (1).
	cv::Mat bilateral;
	cv::bilateralFilter(filtrada, bilateral, 15, 50, 50);
	uchar * fila_bilateral;
	for (int i = 0; i < bilateral.rows; ++i) {
		fila_bilateral = bilateral.ptr<uchar>(i);
		for (int j = 0; j < bilateral.cols*bilateral.channels(); ++j)
			fila_bilateral[j] = floor(fila_bilateral[j]/CUANTIZADOR)*CUANTIZADOR;
	}
	cv::medianBlur(bilateral, bilateral, 7);

	// (4) - agregar a (3) los bordes obtenidos en (2).
	cv::Mat resultado = bilateral.clone();
	uchar * fila_resultado, * fila_laplaciano;
	for (int i = 0; i < laplaciano.rows; ++i) {
		fila_resultado = resultado.ptr<uchar>(i);
		fila_laplaciano = laplaciano.ptr<uchar>(i);
		for (int j = 0; j < laplaciano.cols; ++j) {
			if (fila_laplaciano[j] > 5) {
				for (int k = 0; k < resultado.channels(); ++k)
					fila_resultado[j*resultado.channels()+k] = 0;
			}
		}
	}

	// mostrar imagen original.
	cv::namedWindow("Imagen entrada", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Imagen entrada", 500, 500);
	imshow("Imagen entrada", imagen);

	// si lo requieren, mostrar los filtros, sino un mensaje.
	if (opcion_filtros) {
		cv::namedWindow("Filtrada", cv::WINDOW_FREERATIO);
		cv::resizeWindow("Filtrada", 500, 500);
		imshow("Filtrada", filtrada);

		cv::namedWindow("Laplaciano", cv::WINDOW_FREERATIO);
		cv::resizeWindow("Laplaciano", 500, 500);
		imshow("Laplaciano", laplaciano);

		cv::namedWindow("Bilateral", cv::WINDOW_FREERATIO);
		cv::resizeWindow("Bilateral", 500, 500);
		imshow("Bilateral", bilateral);
	}
	else std::cout << std::endl << "Ejecute el programa con la opción '-f | --filtros' para mostrar los filtros que se generan." << std::endl;

	// mostrar resultado.
	cv::namedWindow("Resultado", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Resultado", 500, 500);
	imshow("Resultado", resultado);

	// esperar input para slair.
	cv::waitKey();

	return EXIT_SUCCESS;
}
