#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int main(int argc, char ** argv)
{
	cv::Mat imagen, transformada;
	// obtener imagen
	if (argc > 1) {
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

	// transformación afín
	int ancho = imagen.cols*imagen.channels(), alto = imagen.rows;
	cv::Point2f src[3], dst[3];
	src[0] = cv::Point2f(0,0);   dst[0] = cv::Point2f(0,0);
	src[1] = cv::Point2f(0,ancho);  dst[1] = cv::Point2f(0,ancho);
	src[2] = cv::Point2f(alto,ancho); dst[2] = cv::Point2f(alto,ancho);
	cv::Mat transformacion_afin = cv::getAffineTransform(src, dst);
	cv::warpAffine(imagen, transformada, transformacion_afin, cv::Size(imagen.cols, imagen.rows));

	// rotación de 90° horariamente
	// cv::rotate(transformada, transformada, 0);

	// traslación 50 píxeles a la derecha, 20 abajo
	cv::Mat arreglo_traslacion = (cv::Mat_<float>(2,3) << 1, 0, 50, 0, 1, 20);
	// cv::warpAffine(transformada, transformada, arreglo_traslacion, cv::Size(imagen.rows, imagen.cols));

	namedWindow("Imagen original", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Imagen original", 500, 400);
	cv::moveWindow("Imagen original", 50, 100);
	imshow("Imagen original", imagen);

	namedWindow("Imagen transformada", cv::WINDOW_NORMAL);
	cv::moveWindow("Imagen transformada", 600, 100);
	imshow("Imagen transformada", transformada);

	cv::waitKey();

	return EXIT_SUCCESS;
}
