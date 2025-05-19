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
	std::string str;
	int key = 0;

	capture.open(videofile);

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
	//cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("VC - VIDEO1", cv::WINDOW_AUTOSIZE);
	int h = round(video.height);
	IVC* image = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageA = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageB = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageC = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageD = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageI = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageE = vc_image_new(video.width, video.height, 3, 255);
	IVC* imageF = vc_image_new(video.width, video.height, 1, 255);
	IVC* imageH = vc_image_new(video.width, video.height, 1, 255);

	OVC* blobsBuffer = NULL;
	int numberBlobsBuffer = 0;
	int cont = 0;
	int m200 = 0, m100 = 0, m50 = 0, m20 = 0, m10 = 0, m5 = 0, m2 = 0, m1 = 0, soma = 0;

	cv::Mat frame;
	cv::Mat frameA;
	while (key != 'q') {
		/* Leitura de uma frame do v�deo */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* N�mero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);
		//str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
		//cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		//cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		//str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		//cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		//cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		//str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		//cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		//cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		//str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		//cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		//cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);


		//cvPutText(frameorig, str, cvPoint(moedas[i].xc + 90, moedas[i].yc - 40), &font, cvScalar(0, 8, 255));
		//sprintf(str, "AREA: %d", moedas[i].area);
		//cvPutText(frameorig, str, cvPoint(moedas[i].xc + 90, moedas[i].yc - 20), &fontbkg, cvScalar(0, 0, 0));
		//cvPutText(frameorig, str, cvPoint(moedas[i].xc + 90, moedas[i].yc - 20), &font, cvScalar(0, 8, 255));
		//sprintf(str, "PERIMETRO: %d", moedas[i].perimeter);
		//cvPutText(frameorig, str, cvPoint(moedas[i].xc + 90, moedas[i].yc), &fontbkg, cvScalar(0, 0, 0));
		//cvPutText(frameorig, str, cvPoint(moedas[i].xc + 90, moedas[i].yc), &font, cvScalar(0, 8, 255));

		// Fa�a o seu c�digo aqui...

		// Cria uma nova imagem IVC
		//cv::imshow("VC - VIDEO", frame);
		// Copia dados de imagem da estrutura cv::Mat para uma estrutura IVC
		cv::medianBlur(frame, frameA, 5);
		memcpy(image->data, frameA.data, video.width* video.height * 3);
		memcpy(imageI->data, frameA.data, video.width* video.height * 3);
		int nlabels = 0;
		vc_gbr_rgb(image);
		vc_rgb_to_hsv(image, imageB);
		vc_hsv_segmentation2(imageB, imageC, 40, 60, 20, 80, 15, 55);//amarelo
		vc_hsv_segmentation2(imageB, imageA, 19, 38, 37, 82, 13, 47);//castanho
		vc_add_image(imageC, imageA);
		vc_hsv_segmentation(imageB, imageC, 40, 200, 4, 24, 15, 50);//cinzento
		vc_add_image(imageC, imageA);
		vc_binary_dilate(imageA, imageC, 3);
		vc_binary_erode(imageC, imageA, 3);
		vc_three_to_one_channel(imageA, imageF);
		OVC* blobs = vc_binary_blob_labelling(imageF, imageH, &nlabels);
		if (blobs != NULL) {
			vc_binary_blob_info(imageH, blobs, nlabels);
			blobs = vc_check_if_circle(blobs, &nlabels, imageF);
			if (blobs != NULL) {
				vc_draw_bounding_box2(imageI, blobs, nlabels);
				vc_gray_edge_prewitt(imageF, imageH);
				vc_draw_edge(imageH, imageI);
				vc_center(blobs, imageI, nlabels);
				memcpy(frame.data, imageI->data, video.width* video.height * 3);
				for (int i = 0; i < nlabels; i++) {
					str = std::string("CENTRO DE MASSA : ").append(std::to_string(blobs[i].xc)).append(", y: ").append(std::to_string(blobs[i].yc));
					cv::putText(frame, str, cv::Point(blobs[i].xc + 90, blobs[i].yc - 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
					str = std::string("Area : ").append(std::to_string(blobs[i].area));
					cv::putText(frame, str, cv::Point(blobs[i].xc + 90, blobs[i].yc - 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);
					str = std::string("Perimetro : ").append(std::to_string(blobs[i].perimeter));
					cv::putText(frame, str, cv::Point(blobs[i].xc + 90, blobs[i].yc - 0), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 2);

					if (image->height / 4 >= blobs[i].yc - 50 && image->height <= blobs[i].yc + 50) {
						switch (idCoin(blobs[i].area, blobs[i].perimeter)) {
						case 200:
							m200++;
							soma += 2;
							break;
						case 100:
							m100++;
							soma += 1;
							break;
						case 50:
							m50++;
							soma += 0.5;
							break;
						case 20:
							m20++;
							soma += 0.2;
							break;
						case 10:
							m10++;
							soma += 0.1;
							break;
						case 5:
							m5++;
							soma += 0.05;
							break;
						case 2:
							m2++;
							soma += 0.02;
							break;
						case 1:
							m1++;
							soma += 0.01;
							break;
						default:
							break;
						}
					}
				}
			}
		}
	
		// +++++++++++++++++++++++++
		str = std::string("Moedas de 200 : ").append(std::to_string(m200));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Moedas de 100 : ").append(std::to_string(m100));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Moedas de 50 : ").append(std::to_string(m50));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Moedas de 20 : ").append(std::to_string(m20));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Moedas de 10 : ").append(std::to_string(m10));
		cv::putText(frame, str, cv::Point(20, 125), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Moedas de 5 : ").append(std::to_string(m5));
		cv::putText(frame, str, cv::Point(20, 150), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Moedas de 2 : ").append(std::to_string(m2));
		cv::putText(frame, str, cv::Point(20, 175), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Moedas de 1 : ").append(std::to_string(m1));
		cv::putText(frame, str, cv::Point(20, 200), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		str = std::string("Total : ").append(std::to_string(soma));
		cv::putText(frame, str, cv::Point(20, 225), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 0, 0), 2);
		/* Exibe a frame */
		cv::imshow("VC - VIDEO1", frame);
		key = cv::waitKey(1);
	}
	vc_image_free(image);
	vc_image_free(imageA);
	vc_image_free(imageB);
	vc_image_free(imageC);
	vc_image_free(imageD);
	vc_image_free(imageE);

	/* Fecha a janela */
	//cv::destroyWindow("VC - VIDEO");
	cv::destroyWindow("VC - VIDEO1");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}