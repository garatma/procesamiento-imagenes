#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <math.h>
#include <iostream>

#define MAX_DP 50
#define MAX_MINDIST 10
#define MAX_PARAM1 200
#define MAX_PARAM2 200
#define MAX_MINRADIUS 10
#define MAX_MAXRADIUS 10

int dp = 20, minDist = 2, param1 = 100, param2 = 100, minRadius = 2, maxRadius = 7;

struct argumento_callback {
	cv::Mat imagen;
};

void encontrar_galletitas(int, void * argumento)
{
	struct argumento_callback * p_argumento = (struct argumento_callback *) argumento;
	cv::Mat imagen, grises;

	imagen = p_argumento->imagen.clone();
	cv::cvtColor(imagen, grises, cv::COLOR_BGR2GRAY);

	// smooth it, otherwise a lot of false circles may be detected
	cv::GaussianBlur(grises, grises, cv::Size(9, 9), 2, 2);
	std::vector <cv::Vec3f> circles;
	// cv::HoughCircles(grises, circles, cv::HOUGH_GRADIENT, 2, grises.cols/9, 100, 100, grises.cols/9, grises.cols/4);
	cv::HoughCircles(grises, circles, cv::HOUGH_GRADIENT, (double) (dp+1)/10, grises.cols/(10-minDist+1), param1, param2, grises.cols/(10-minRadius+1), grises.cols/(10-maxRadius+1));

	for(size_t i = 0; i < circles.size(); i++)
	{
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// draw the circle center
		circle(imagen, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
		// draw the circle outline
		circle(imagen, center, radius, cv::Scalar(0,0,255), 3, 8, 0);
	}

	namedWindow("Resultados", cv::WINDOW_FREERATIO);
	cv::resizeWindow("Resultados", 500, 700);
	imshow("Resultados", imagen);

	// namedWindow("grises", cv::WINDOW_FREERATIO);
	// cv::resizeWindow("grises", 500, 500);
	// imshow("grises", grises);
}

int main(int argc, char** argv)
{
	cv::Mat imagen, grises;
	if(argc != 2 || !(imagen = cv::imread(argv[1], 1)).data)
	{
		std::cout << "Debe especificar la ruta a una imagen." << std::endl;
		return EXIT_FAILURE;
	}

	struct argumento_callback argumento_callback;
	argumento_callback.imagen = imagen;

	encontrar_galletitas(0, &argumento_callback);

	const char * barra_dp = "dP",
		* barra_minDist = "Distancia entre círculos",
		// * barra_param1 = "param1",
		// * barra_param2 = "param2",
		* barra_minRadius = "Radio mínimo",
		* barra_maxRadius= "Radio máximo";

	cv::createTrackbar(barra_dp,
		"Resultados",
		&dp,
		MAX_DP,
		encontrar_galletitas,
		&argumento_callback
	);

	cv::createTrackbar(barra_minDist,
		"Resultados",
		&minDist,
		MAX_MINDIST,
		encontrar_galletitas,
		&argumento_callback
	);

	// cv::createTrackbar(barra_param1,
	// 	"Resultados",
	// 	&param1,
	// 	MAX_PARAM1,
	// 	encontrar_galletitas,
	// 	&argumento_callback
	// );

	// cv::createTrackbar(barra_param2,
	// 	"Resultados",
	// 	&param2,
	// 	MAX_PARAM2,
	// 	encontrar_galletitas,
	// 	&argumento_callback
	// );

	cv::createTrackbar(barra_minRadius,
		"Resultados",
		&minRadius,
		MAX_MINRADIUS,
		encontrar_galletitas,
		&argumento_callback
	);

	cv::createTrackbar(barra_maxRadius,
		"Resultados",
		&maxRadius,
		MAX_MAXRADIUS,
		encontrar_galletitas,
		&argumento_callback
	);

	cv::waitKey(0);

	return 0;
}
