#include <algorithm>
#include <iterator>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/resource.h>

#define   BLANCO               255
#define   GRIS                 50
#define   ALTO_GALLETITA       600
#define   LARGO_GALLETITA      600
#define   CANT_GALLETITAS      6
#define   ESPACIO_HORIZONTAL   800
#define   ESPACIO_VERTICAL     800
#define   ARRIBA               0
#define   ABAJO                1
#define   CENTRO               2
#define   IZQUIERDA            3
#define   DERECHA              4
#define   MAX_DIVISOR		   200
#define   MAX_UMBRAL_COLOR     50
#define   MAX_UMBRAL_FONDO	   50

struct galletita {
	std::string nombre;
	uchar colorH;
	int cantidad;
};

cv::Mat imagen, mapa, canal_h;

std::string ventana_mapa = "Procesamiento pixeles",
	ventana_resultados = "Resultados";

int fondo, umbral_galletita, divisor_umbral = 72, umbral_color = 25, umbral_fondo = 15;
bool opcion_o = false;

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

	p = canal_h.ptr<uchar>(fila);
	pmapa = mapa.ptr<uchar>(fila);

	if (!es(p[columna], fondo, umbral_fondo) && pmapa[columna] == 0) {
		contador++;
		pmapa[columna] = BLANCO;
		(*suma) += int(p[columna]);

		if (contador > umbral_galletita) return contador;

		if (pos_pixel_anterior != ABAJO && fila+1 < canal_h.rows) {
			pmapa = mapa.ptr<uchar>(fila+1);
			if (pmapa[columna] == 0)
				contador += es_galletita(fila+1, columna, ARRIBA, suma);
		}

		if (contador == umbral_galletita) return contador;

		if (pos_pixel_anterior != IZQUIERDA && 0 < columna && pmapa[columna-1] == 0)
			contador += es_galletita(fila, columna-1, DERECHA, suma);

		if (contador == umbral_galletita) return contador;

		if (pos_pixel_anterior != DERECHA && columna+1 < canal_h.cols && pmapa[columna+1] == 0)
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
		encontre = es(galletitas[i].colorH, color, umbral_color);
		i++;
	}
	if (encontre) i--;
	return i;
}

void encontrar_galletitas(int, void*)
{
	// datos sobre las galletitas
	struct galletita galletitas[CANT_GALLETITAS];
	inicializar_galletitas(galletitas);
	for (int i = 0; i < CANT_GALLETITAS; ++i) galletitas[i].cantidad = 0;

	// un mapa que indique qué píxeles se leyeron (en BLANCO)
	mapa = cv::Mat::zeros(canal_h.rows, canal_h.cols, CV_8U);

	if (!divisor_umbral) divisor_umbral++;
	umbral_galletita = imagen.cols*imagen.rows*imagen.channels()/divisor_umbral;

	uchar * pmapa, * p;
	double suma, pixeles_galletita;
	int galletitas_encontradas = 0, indice;

	// recorrer imagen buscando píxeles de color distinto al fondo y ver cuántos
	// hay de estos de manera contigua para determinar si es una galletita o no
	for (int i = 1; i < canal_h.rows; ++i) {
		p = canal_h.ptr<uchar>(i);
		pmapa = mapa.ptr<uchar>(i);
		for (int j = 0; j < canal_h.cols; ++j) {
			if (!pmapa[j] && !es(p[j], fondo, umbral_fondo)) {
				// si no se procesó el pixel y no es fondo
				suma = 0;
				pixeles_galletita = es_galletita(i, j, CENTRO, &suma);
				if (pixeles_galletita > umbral_galletita) {
					// la cantidad de píxeles contiguos del mismo color es mayor
					// que el mínimo necesario para concluir que es una galletita
					galletitas_encontradas++;
					indice = obtener_indice_galletita(galletitas, (int) suma/pixeles_galletita);
					galletitas[indice].cantidad++;
				}
			}
		}
	}

	std::string resultados = "Resultados: ";
	cv::Mat imagen_resultados = cv::Mat::zeros(400, 400, CV_8U);
	cv::putText(imagen_resultados, resultados, cv::Point(0, 25),
		cv::FONT_HERSHEY_DUPLEX, 1, BLANCO, 1, cv::LINE_8, false
	);

	int i;
	for (i = 0; i < CANT_GALLETITAS; ++i) {
		resultados = "    ";
		resultados = resultados + galletitas[i].nombre;
		resultados = resultados + ": ";
		resultados = resultados + std::to_string(galletitas[i].cantidad);
		cv::putText(imagen_resultados, resultados, cv::Point(0, 75+50*i),
			cv::FONT_HERSHEY_DUPLEX, 1, BLANCO, 1, cv::LINE_8, false
		);
	}


	resultados = "    Total: ";
	resultados = resultados + std::to_string(galletitas_encontradas);
	cv::putText(imagen_resultados, resultados, cv::Point(0, 75+50*i),
			cv::FONT_HERSHEY_DUPLEX, 1, BLANCO, 1, cv::LINE_8, false
	);

	namedWindow(ventana_resultados, cv::WINDOW_AUTOSIZE);
	imshow(ventana_resultados, imagen_resultados);
	cv::moveWindow(ventana_resultados, 200, 0);

	if (opcion_o) {
		namedWindow(ventana_mapa, cv::WINDOW_FREERATIO);
		cv::resizeWindow(ventana_mapa, 500, 500);
		imshow(ventana_mapa, mapa);
		cv::moveWindow(ventana_mapa, 610, 0);
	}
}

int main(int argc, char ** argv)
{
	// incrementar el tamaño de la stack para soportar fotos de mayor resolución
	struct rlimit rlp;
	rlp.rlim_cur = RLIM_INFINITY;
	rlp.rlim_max = RLIM_INFINITY;
	int ret = setrlimit(RLIMIT_STACK, &rlp);

	// obtener imagen
	if (argc == 3) {
		opcion_o = !strcmp("-o", argv[1]);
		if (!opcion_o)
			std::cout << std::endl << "Ejecute con la opción -o para mostrar el procesamiento de píxeles" << std::endl;
		imagen = imread(argv[2], cv::IMREAD_COLOR);
	}
	else if (argc == 2) {
		std::cout << std::endl << "Ejecute con la opción -o para mostrar el procesamiento de píxeles" << std::endl;
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

	// convertir de BGR a HSV
	cv::cvtColor(imagen, imagen, cv::COLOR_BGR2HSV);

	// dividir en canales
	cv::Mat canales[imagen.channels()];
	cv::split(imagen, canales);

	// canal H
	canal_h = canales[0];

	// variables necesarias
	bool encontre = false;
	uchar * p, * pmapa;
	int pixeles_galletita, indice, galletitas_encontradas = 0;

	// establecer color del fondo a partir del promedio de tonalidad de los
	// píxeles de la primera fila
	double suma = 0;
	p = canal_h.ptr<uchar>(0);
	for (int j = 0; j < canal_h.cols; ++j) suma += p[j];
	fondo = suma/imagen.cols;

	cv::cvtColor(imagen, imagen, cv::COLOR_HSV2BGR);
	encontrar_galletitas(0,0);

	const char * barra_galletita = "Umbral píxeles galletita",
		* barra_color = "Umbral color galletita",
		* barra_fondo = "Umbral color fondo";

	cv::createTrackbar(barra_galletita, ventana_resultados, &divisor_umbral, MAX_DIVISOR, encontrar_galletitas);
	cv::createTrackbar(barra_color, ventana_resultados, &umbral_color, MAX_UMBRAL_COLOR, encontrar_galletitas);
	cv::createTrackbar(barra_fondo, ventana_resultados, &umbral_fondo, MAX_UMBRAL_FONDO, encontrar_galletitas);

	cv::waitKey();

	return EXIT_SUCCESS;
}
