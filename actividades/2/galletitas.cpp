#include <opencv2/core.hpp>
#include <unistd.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <ostream>
#include <string>
#define FONDO 100
#define BLANCO 255
#define ALTO_GALLETITA 600
#define LARGO_GALLETITA 600
#define CANT_GALLETITAS 5
#define UMBRAL_COLOR 5
#define UMBRAL_GALLETITA 0.5
#define ESPACIO_HORIZONTAL 800
#define ESPACIO_VERTICAL 800

struct galletita {
	std::string nombre;
	uchar colorH;
};

// TODO:
// 	- cuenta de las galletitas al final
//	- hacerlo independiente de la grilla de 3x3: recorrer toda la imagen y comparar con lo que ya guardé para saltearme esos píxeles

void inicializar_galletitas(struct galletita galletitas[CANT_GALLETITAS])
{
	galletitas[0] = { "Claro", 				20  };
	galletitas[1] = { "Anillo amarillo", 	35  };
	galletitas[2] = { "Anillo blanco", 		90  };
	galletitas[3] = { "Anillo rosa", 		170 };
	galletitas[4] = { "Melba", 				5   };
}

bool en_umbral(int num1, int num2) { return abs(num1-num2) < UMBRAL_COLOR; }

bool es_galletita(cv::Mat imagen, cv::Mat mapa, int fila, int columna)
{
	int i = fila, j, pixeles_contados = 0, pixeles_correctos = 0;
	bool es_galletita = false;
	uchar * p, * pmapa;
	std::cout << "es_galletita. fila: " << fila << ". columna: " << columna << std::endl;
	std::cout << "el ancho sería: (" << columna-(LARGO_GALLETITA/2) << "," << columna+(LARGO_GALLETITA/2) << ")" << std::endl;
	std::cout << "el largo sería: (" << fila << "," << fila+ALTO_GALLETITA << ")" << std::endl;
	// TODO: guardar el color del pixel acá para comparar luego con el struct galletita
	while (i < fila+ALTO_GALLETITA) {
		j = columna-(LARGO_GALLETITA/2);
		p = imagen.ptr<uchar>(i);
		pmapa = mapa.ptr<uchar>(i);
		// std::cout << "correctos: " << pixeles_correctos << std::endl;
		// std::cout << "contados: " << pixeles_contados << std::endl;
		// std::cout << "proporción: " << 1.0*pixeles_correctos/pixeles_contados << std::endl << std::endl;
		while (j < columna+(LARGO_GALLETITA/2)) {
			if (!en_umbral(p[j], FONDO)) {
				pmapa[j] = 255;
				pixeles_correctos++;
			}
			pixeles_contados++;
			j++;
			// std::cout << "es galletita: " << es_galletita << std::endl;
		}
		i++;
	}
	es_galletita = (1.0*pixeles_correctos/pixeles_contados) > UMBRAL_GALLETITA;
	std::cout << "proporción: " << (1.0*pixeles_correctos/pixeles_contados) << std::endl;
	return es_galletita;
}

int obtener_indice_galletita(struct galletita galletitas[CANT_GALLETITAS], uchar color)
{
	bool encontre = false;
	int i = 0;
	while (!encontre && i < CANT_GALLETITAS) {
		encontre = en_umbral(galletitas[i].colorH, color);
		i++;
	}
	if (!encontre) i = -1;
	return i;
}

int main(int argc, char ** argv)
{
	struct galletita galletitas[CANT_GALLETITAS];
	inicializar_galletitas(galletitas);

	// obtener imagen
	cv::Mat imagen = imread(argv[1], cv::IMREAD_COLOR);
	if(imagen.empty()) {
		std::cout << "No se pudo encontrar la imagen." << std::endl;
		return EXIT_FAILURE;
	}

	int contador[CANT_GALLETITAS];
	int galletitas_encontradas = 0;

	for (int i = 0; i < CANT_GALLETITAS; ++i) {
		contador[i] = 0;
	}

	cv::cvtColor(imagen, imagen, cv::COLOR_BGR2HSV);

	std::string titulos_ventanas[imagen.channels()];
	cv::Mat canales[imagen.channels()];
	cv::split(imagen, canales);

	cv::Mat mapa = cv::Mat::zeros(imagen.rows, imagen.cols, CV_8U);

	cv::Mat cH = canales[0];
	cv::Mat cV = canales[0];

	bool encontre_primera = false;
	int i = 0, j;
	uchar * p;

	std::cout << mapa.rows << " " << mapa.cols << std::endl;

	while (!encontre_primera && i < cH.rows) {
		p = cH.ptr<uchar>(i);
		j = 0;
		while (!encontre_primera && j < cH.cols) {
			encontre_primera = !en_umbral(p[j], FONDO);
			if (encontre_primera) {
				encontre_primera = es_galletita(cH, mapa, i, j);
				if (encontre_primera) {
					std::cout << "encontré en 0,0" << std::endl << std::endl;
					galletitas_encontradas++;
				}
			}
			j++;
		}
		i++;
	}

	bool encontre;
	if (encontre_primera) {
		int fila = 0, columna = 1;
		while (fila < 3) {
			while (columna < 3) {
				encontre = es_galletita(cH, mapa, i+fila*ESPACIO_VERTICAL, j+columna*ESPACIO_HORIZONTAL);
				if (encontre) {
					std::cout << "encontré en " << fila << "," << columna << std::endl << std::endl;
					galletitas_encontradas++;
				}
				columna++;
			}
			fila++;
			columna = 0;
		}
	}

	std::cout << "Enhorabuena: has encontrado " << galletitas_encontradas << " galletitas" << std::endl;

	namedWindow("mapa", cv::WINDOW_NORMAL);
	imshow("mapa", mapa);

	namedWindow("cH", cv::WINDOW_NORMAL);
	imshow("cH", cH);

	cv::waitKey();

	return EXIT_SUCCESS;
}
