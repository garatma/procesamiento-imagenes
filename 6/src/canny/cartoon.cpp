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
		std::cout << std::endl << "Ejecute el programa con la opción '-f | --filtros' para mostrar los filtros que se generan." << std::endl;
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

	// (2) - detectar bordes de la imagen.
	cv::Mat canny;
	cv::Canny(filtrada, canny, 10, 100);

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
	uchar * fila_resultado, * fila_canny;
	for (int i = 0; i < canny.rows; ++i) {
		fila_resultado = resultado.ptr<uchar>(i);
		fila_canny = canny.ptr<uchar>(i);
		for (int j = 0; j < canny.cols; ++j) {
			if (fila_canny[j] > 5) {
				for (int k = 0; k < resultado.channels(); ++k)
					fila_resultado[j*resultado.channels()+k] = 0;
			}
		}
	}

	// mostrar imagen original.
	cv::namedWindow("Imagen entrada", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Imagen entrada", 500, 500);
	cv::moveWindow("Imagen entrada", 500, 0);
	imshow("Imagen entrada", imagen);

	// mostrar resultado.
	cv::namedWindow("Resultado", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Resultado", 500, 500);
	cv::moveWindow("Resultado", 0, 0);
	imshow("Resultado", resultado);

	// si lo requieren, mostrar los filtros.
	if (opcion_filtros) {
		cv::namedWindow("Filtrada", cv::WINDOW_FREERATIO);
		cv::resizeWindow("Filtrada", 500, 500);
		cv::moveWindow("Filtrada", 0, 500);
		imshow("Filtrada", filtrada);

		cv::namedWindow("Canny", cv::WINDOW_FREERATIO);
		cv::resizeWindow("Canny", 500, 500);
		cv::moveWindow("Canny", 500, 500);
		imshow("Canny", canny);

		cv::namedWindow("Bilateral", cv::WINDOW_FREERATIO);
		cv::resizeWindow("Bilateral", 500, 500);
		cv::moveWindow("Bilateral", 1000, 500);
		imshow("Bilateral", bilateral);
	}

	// esperar input para slair.
	cv::waitKey();

	return EXIT_SUCCESS;
}
