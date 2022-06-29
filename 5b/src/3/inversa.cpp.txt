#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int main(int argc, char ** argv)
{
	cv::Mat original, modificada, resultado;

	// obtener imágenes
	if (argc > 2) {
		original = imread(argv[1], cv::IMREAD_COLOR);
		modificada = imread(argv[2], cv::IMREAD_COLOR);
	}
	else {
		std::cout << "Debe especificar la ruta a las imágenes." << std::endl;
		return EXIT_FAILURE;
	}

	if(original.empty() || modificada.empty()) {
		std::cout << "No se pudo encontrar una imagen." << std::endl;
		return EXIT_FAILURE;
	}

	// PT
	float Pa[3][3] = {{2455, 1265, 2492}, {632, 375, 1100}, {1, 1, 1}},
		  Qa[3][3] = {{335, 218, 753}, {857, 400, 480}, {1, 1, 1}};

	// std::vector<int> v;
	cv::Mat P = cv::Mat(3, 3, CV_32FC1, Pa);
	cv::Mat Q = cv::Mat(3, 3, CV_32FC1, Qa);

	cv::Mat PT, PPT, PPT_inv, PT_PPT_inv;

	// T = Q·PT·(P·PT)⁻¹
	cv::transpose(P, PT); // PT
	PPT = P * PT; // P·PT
	PPT_inv = PPT.inv(); // (P·PT)⁻¹
	cv::Mat T = Q*PT*PPT_inv; // Q·PT·(P·PT)⁻¹

	cv::warpPerspective(modificada, resultado, T, cv::Size(original.cols, original.rows));

	imshow("Imagen reconstruida", resultado);

	cv::waitKey();

	return EXIT_SUCCESS;
}
