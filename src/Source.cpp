#include <iostream>
#include <string>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>

extern "C" {
#include "vc.h"
}


int main(void) {
	// V�deo
	char videofile[20] = "videos/video1.mp4";
	cv::VideoCapture capture;
	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;
	// Outros
	std::string str;
	int key = 0;

	/* Leitura de v�deo de um ficheiro */
	/* NOTA IMPORTANTE:
	O ficheiro video.avi dever� estar localizado no mesmo direct�rio que o ficheiro de c�digo fonte.
	*/
	capture.open(videofile);

	/* Em alternativa, abrir captura de v�deo pela Webcam #0 */
	//capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);

	/* Verifica se foi poss�vel abrir o ficheiro de v�deo */
	if (!capture.isOpened())
	{
		std::cerr << "Erro ao abrir o ficheiro de v�deo!\n";
		return 1;
	}

	/* N�mero total de frames no v�deo */
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
	/* Frame rate do v�deo */
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);
	/* Resolu��o do v�deo */
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	/* Cria uma janela para exibir o v�deo */
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("VC - VIDEO1", cv::WINDOW_AUTOSIZE);
	IVC* image = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageD = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageC = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageE = vc_image_new(video.width, video.height, 1, 255);
	IVC* imageF = vc_image_new(video.width, video.height, 1, 255);
	IVC* imageH = vc_image_new(video.width, video.height, 3, 255);


	cv::Mat frame;
	while (key != 'q') {
		/* Leitura de uma frame do v�deo */
		for (int i = 0; i < 2; i++) {
			capture.read(frame);
			if (frame.empty()) break;
		}

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* N�mero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		

		// Fa�a o seu c�digo aqui...

		// Cria uma nova imagem IVC

		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		memcpy(image->data, frame.data, video.width* video.height * 3);
		cv::imshow("VC - VIDEO", frame);
		// Executa uma fun��o da nossa biblioteca vc
		vc_gbr_rgb(image);
		vc_rgb_to_hsv(image, imageD);
		vc_hsv_segmentation2(imageD, imageC, 20, 100, 35, 100, 20, 100);//amarelas erode 3/3
		vc_hsv_segmentation2(imageD, imageH, 0, 360, 0, 25, 20,50);//cinzentos erode 7/3
		vc_add_image(imageC, imageH);
		vc_hsv_segmentation2(imageD, imageC, 20, 50, 25,60, 0, 30);
		vc_add_image(imageH, imageC);
		int nlabels = 0;
		vc_three_to_one_channel(imageC, imageF);
		//vc_binary_erode(imageC, imageD, 3);
		//vc_binary_dilate(imageD, imageC, 3);
		
		memcpy(frame.data, imageH->data, video.width* video.height * 3);
		//OVC * blobs = vc_binary_blob_labelling(imageF, imageE, &nlabels);
		//vc_binary_blob_info(imageE, blobs, nlabels);
		//vc_draw_bounding_box(imageE, imageF, blobs, nlabels);
		//vc_one_to_three_channel(imageE, imageC);
		// Copia dados de imagem da estrutura IVC para uma estrutura cv::Mat
		
		// Liberta a mem�ria da imagem IVC que havia sido criada

		// +++++++++++++++++++++++++

		/* Exibe a frame */
		cv::imshow("VC - VIDEO1", frame);
		

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
	}
	vc_image_free(imageD);
	vc_image_free(image);
	vc_image_free(imageC);
	vc_image_free(imageE);
	vc_image_free(imageF);
	vc_image_free(imageH);

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");
	cv::destroyWindow("VC - VIDEO1");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}