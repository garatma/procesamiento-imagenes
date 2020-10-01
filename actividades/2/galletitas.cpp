#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#define FONDO 100
#define BLANCO 255
#define ALTO_GALLETITA 600
#define LARGO_GALLETITA 600
#define CANT_GALLETITAS 5
#define UMBRAL_COLOR 15
#define UMBRAL_GALLETITA 0.5
#define ESPACIO_HORIZONTAL 250
#define ESPACIO_VERTICAL 250

struct galletita {
	std::string nombre;
	uchar colorH;
};

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
	while (!es_galletita && i < fila+ALTO_GALLETITA) {
		j = columna-(LARGO_GALLETITA)/2;
		p = imagen.ptr<uchar>(i);
		pmapa = mapa.ptr<uchar>(i);
		while (!es_galletita && j < columna+(LARGO_GALLETITA/2)) {
			if (!en_umbral(p[j], FONDO)) {
				pixeles_correctos++;
				pmapa[j] = BLANCO;
			}
			pixeles_contados++;
			j++;
			es_galletita = (pixeles_correctos/pixeles_contados) > UMBRAL_GALLETITA;
		}
	}
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

	while (!encontre_primera && i < cH.rows) {
		p = cH.ptr<uchar>(i);
		j = 0;
		while (!encontre_primera && j < cH.cols) {
			std::cout << i << "," << j << std::endl;
			encontre_primera = !en_umbral(p[j], FONDO);
			j++;
		}
		i++;
	}

	// por ahora asumo que son 9 en una grilla de 3x3
	std::cout << "encontre en: " << i << "," << j << std::endl;

	int fila = 0, columna;
	while (fila < 3) {
		columna = 3;
		while (columna < 3) {
			es_galletita(cH, mapa, i+fila*ESPACIO_VERTICAL, j+columna*ESPACIO_HORIZONTAL);
			columna++;
		}
		fila++;
	}


	namedWindow("Mapa", cv::WINDOW_NORMAL);
	imshow("Mapa", cH);

	cv::waitKey();

	return EXIT_SUCCESS;
}
