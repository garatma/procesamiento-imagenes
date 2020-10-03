#include <algorithm>
#include <iterator>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <string.h>

#define   BLANCO               255
#define   GRIS                 50
#define   ALTO_GALLETITA       600
#define   LARGO_GALLETITA      600
#define   CANT_GALLETITAS      6
#define   UMBRAL_COLOR         25
#define   UMBRAL_FONDO		   15
#define   ESPACIO_HORIZONTAL   800
#define   ESPACIO_VERTICAL     800
#define   ARRIBA               0
#define   ABAJO                1
#define   CENTRO               2
#define   IZQUIERDA            3
#define   DERECHA              4

struct galletita {
	std::string nombre;
	uchar colorH;
	int cantidad;
};

cv::Mat * copia_imagen, * copia_mapa;

int fondo, umbral_galletita;

// TODO:
//	- poner sliders para ajustar el umbral_galletita y el umbral_color

void inicializar_galletitas(struct galletita galletitas[CANT_GALLETITAS])
{
	galletitas[0] = { "Marrones", 			15,		0};
	galletitas[1] = { "Anillo amarillo", 	35,		0};
	galletitas[2] = { "Anillo blanco", 		90,		0};
	galletitas[3] = { "Anillo rosa", 		167,	0};
	galletitas[4] = { "Melba", 				5, 		0};
	galletitas[5] = { "No identificada",	255,	0};
}

bool es(int num1, int num2, int umbral) { return abs(num1-num2) < umbral; }

int es_galletita(int fila, int columna, int pos_pixel_anterior, double * suma)
{
	uchar * p, * pmapa;
	int contador = 0;

	p = (*copia_imagen).ptr<uchar>(fila);
	pmapa = (*copia_mapa).ptr<uchar>(fila);

	if (!es(p[columna], fondo, UMBRAL_FONDO) && pmapa[columna] == 0) {
		contador++;
		pmapa[columna] = BLANCO;
		(*suma) += int(p[columna]);

		// if (pos_pixel_anterior != ARRIBA && 0 < fila) {
		// 	pmapa = (*copia_mapa).ptr<uchar>(fila-1);
		// 	if (pmapa[columna] == 0)
		// 		contador += es_galletita(fila-1, columna, ABAJO, suma);
		// }

		if (contador > umbral_galletita) return contador;

		if (pos_pixel_anterior != ABAJO && fila+1 < (*copia_imagen).rows) {
			pmapa = (*copia_mapa).ptr<uchar>(fila+1);
			if (pmapa[columna] == 0)
				contador += es_galletita(fila+1, columna, ARRIBA, suma);
		}

		if (contador == umbral_galletita) return contador;

		if (pos_pixel_anterior != IZQUIERDA && 0 < columna && pmapa[columna-1] == 0)
			contador += es_galletita(fila, columna-1, DERECHA, suma);

		if (contador == umbral_galletita) return contador;

		if (pos_pixel_anterior != DERECHA && columna+1 < (*copia_imagen).cols && pmapa[columna+1] == 0)
			contador += es_galletita(fila, columna+1, IZQUIERDA, suma);

		if (contador == umbral_galletita) return contador;
	}

	return contador;
}

int obtener_indice_galletita(struct galletita galletitas[CANT_GALLETITAS], int color)
{
	bool encontre = false;
	int i = 0;
	while (!encontre && i < CANT_GALLETITAS-1) {
		encontre = es(galletitas[i].colorH, color, UMBRAL_COLOR);
		i++;
	}
	if (encontre) i--;
	return i;
}

int main(int argc, char ** argv)
{
	struct galletita galletitas[CANT_GALLETITAS];
	inicializar_galletitas(galletitas);

	cv::Mat imagen;

	if (argc > 1)
		imagen = imread(argv[1], cv::IMREAD_COLOR);

	if(imagen.empty()) {
		std::cout << "No se pudo encontrar la imagen." << std::endl;
		return EXIT_FAILURE;
	}

	for (int i = 0; i < CANT_GALLETITAS; ++i) {
		galletitas[i].cantidad = 0;
	}

	cv::cvtColor(imagen, imagen, cv::COLOR_BGR2HSV);

	std::string titulos_ventanas[imagen.channels()];
	cv::Mat canales[imagen.channels()];
	cv::split(imagen, canales);

	cv::Mat mapa = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8U);

	cv::Mat cH = canales[0];
	cv::Mat cV = canales[0];

	bool encontre = false;
	int i = 0, j;
	uchar * p, *pmapa;

	// establecer color del fondo
	double suma = 0;
	p = cH.ptr<uchar>(0);
	for (int j = 0; j < cH.cols; ++j) suma += p[j];
	fondo = suma/imagen.cols;

	int pixeles_galletita;

	umbral_galletita = imagen.cols*imagen.rows/72;

	copia_imagen = &cH;
	copia_mapa = &mapa;
	int indice, galletitas_encontradas = 0;

	for (int i = 1; i < cH.rows; ++i) {
		p = cH.ptr<uchar>(i);
		pmapa = mapa.ptr<uchar>(i);
		j = 0;
		for (int j = 0; j < cH.cols; ++j) {
			if (!pmapa[j] && !es(p[j], fondo, UMBRAL_FONDO)) {
				suma = 0;
				pixeles_galletita = es_galletita(i, j, CENTRO, &suma);
				if (pixeles_galletita > umbral_galletita) {
					galletitas_encontradas++;

					indice = obtener_indice_galletita(galletitas, (int) suma/pixeles_galletita);
					galletitas[indice].cantidad++;
				}
			}
		}
	}

	std::cout << "Se encontraron: " << galletitas_encontradas << " galletitas" << std::endl;
	std::cout << "Resultado: " << std::endl;

	for (int i = 0; i < CANT_GALLETITAS; ++i) {
		std::cout << "\t" << galletitas[i].nombre << ": " << galletitas[i].cantidad << std::endl;
	}

	if (argc > 2 && (strcmp(argv[2], "-o") || strcmp(argv[2], "--output"))) {

		cv::cvtColor(imagen, imagen, cv::COLOR_HSV2BGR);
		namedWindow("imagen", cv::WINDOW_NORMAL);
		imshow("imagen", imagen);
		cv::moveWindow("imagen", 400, 400);

		namedWindow("mapa", cv::WINDOW_NORMAL);
		imshow("mapa", mapa);
		cv::moveWindow("mapa", 800, 400);

		cv::waitKey();
	}

	return EXIT_SUCCESS;
}
