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
	IVC* imageA = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageB = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageC = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageD = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageE = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageF = vc_image_new(video.width, video.height, 1, 255);
	IVC* imageH = vc_image_new(video.width, video.height, 1, 255);



	cv::Mat frame;
	cv::Mat frameA;
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
		cv::imshow("VC - VIDEO", frame);
		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		cv::medianBlur(frame, frameA, 5);
		memcpy(image->data, frameA.data, video.width* video.height * 3);
		
		// Executa uma fun��o da nossa biblioteca vc
		vc_gbr_rgb(image);
		vc_rgb_to_hsv(image, imageA);
		vc_hsv_segmentation2(imageA, imageB, 40, 60, 20, 80, 15, 55);//amarelo
		vc_hsv_segmentation2(imageA, imageC, 19, 34, 37, 75, 15, 47);//castanho
		//vc_hsv_segmentation2(imageA, imageB, 20, 100, 35, 100, 20, 100);//amarelas erode 3/3
		//vc_hsv_segmentation2(imageA, imageC, 0, 360, 0, 25, 20,50);//cinzentos erode 7/3
		vc_add_image(imageC, imageB);
		vc_hsv_segmentation2(imageA, imageC, 40, 200, 4, 24, 15, 50);//cinzento
		vc_add_image(imageC, imageB);
		vc_three_to_one_channel(imageB, imageF);
		vc_gray_edge_prewitt(imageF, imageH, 5);
		vc_draw_edge(imageH, image);
		memcpy(frame.data, image->data, video.width* video.height * 3);
		
		//vc_hsv_segmentation2(imageA, imageB, 20, 50, 25,60, 0, 30);
		//vc_add_image(imageB, imageC);
		//vc_three_to_one_channel(imageB, imageF);
		//vc_binary_erode(imageF, imageH, 3);
		//vc_binary_dilate(imageH, imageF, 3);
		int nlabels = 0;
		//vc_binary_erode(imageF, imageH, 5);
		/*OVC* blobs = vc_binary_blob_labelling(imageH, imageF, &nlabels);
		vc_binary_blob_info(imageF, blobs, nlabels);
		vc_draw_bounding_box2(image, blobs, nlabels);
		vc_gray_edge_prewitt(imageH, imageF, 5);
		vc_draw_edge(imageF, image);*/
		
		//memcpy(frame.data, imageB->data, video.width* video.height * 3);

		// +++++++++++++++++++++++++

		/* Exibe a frame */
		cv::imshow("VC - VIDEO1", frame);
		

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
	}
	vc_image_free(image);
	vc_image_free(imageA);
	vc_image_free(imageB);
	vc_image_free(imageC);
	vc_image_free(imageD);
	vc_image_free(imageE);

	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");
	cv::destroyWindow("VC - VIDEO1");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}