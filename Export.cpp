#include "Export.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/calib3d.hpp>
#include <fstream>
#include <string>
#include<typeinfo>
#include <json/json.h>
#include <iostream>
using namespace std;
using namespace cv;

EXPORT_API char nExport = 0;

//const char* pImgPath		原图像存储路径
//const char* pLayoutInfo	版面信息
//const char* pImgSavePath	处理后图像存储路径
EXPORT_API void fnExport(const char *pImgPath, const char *pLayoutInfo, const char *pImgSavePath) {
	cout << "pImgPath:\t" << pImgPath << endl;
	cout << "pLayoutInfo:\t" << pLayoutInfo << endl;
	cout << "pImgSavePath:\t" << pImgSavePath << endl;
	cout << "OpenCV_Version: " << CV_VERSION << endl;
	String dataPath = pImgPath;
	dataPath = dataPath;
	vector<String> imageNames;
	cv::glob(dataPath + "*.tif", imageNames, false);
	Json::Reader reader;
	Json::Value root;
	std::list<int> L;
	for (vector<String>::iterator iter = imageNames.begin(); iter != imageNames.end(); iter++)
	{
		L.clear();
		ifstream jsonfile(String(*iter).substr(0, String(*iter).size() - 3) + "json", ios::binary);
		if (!jsonfile.is_open()) {
			cout << "open json failed" << endl;
		}
		//读版面信息图像区域
		if (reader.parse(jsonfile, root)) {
			int a = root["regions"][(unsigned int)0]["blocks"].size();
			for (int i = 0; i < a; i++) {
				if (root["regions"][(unsigned int)0]["blocks"][(unsigned int)i]["type"] == "image") {
					for (int j = 0; j < 4; j++) {
						L.push_back(root["regions"][(unsigned int)0]["blocks"][(unsigned int)i]["coordinate"][(unsigned int)j].asInt());
					}
				}
			}
		}
		jsonfile.close();

		//读取图片
		Mat o_img, img;
		o_img= imread(*iter);
		cvtColor(o_img, img, COLOR_BGR2GRAY);

		//腐蚀
		Mat element_kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
		erode(img, img, element_kernel);

		//锐化
		Mat Sharpen_kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
		filter2D(img, img, CV_8UC3, Sharpen_kernel);

		//部分二值化
		threshold(img, img, 80, 0, THRESH_TOZERO);
		threshold(img, img, 255, 210, THRESH_TOZERO_INV);

		//灰度直方图正规化
		double maxx = 0, minn = 0;
		double* max = &maxx;
		double* min = &minn;
		minMaxIdx(img, min, max);
		int a, b;
		a = 255 / (*max - *min);
		b = 0 - a * *min;
		img = a * img + b;

		//转为三通道
		cvtColor(img, img, COLOR_GRAY2BGR);

		//替换图片部分
		int roi1, roi2, roi3, f;
		f = 0;
		for (list<int>::iterator iter = L.begin(); iter != L.end(); iter++) {
			if (f == 0) {
				roi1 = *iter;
				f++;
			}
			else if (f == 1) {
				roi2 = *iter;
				f++;
			}
			else if (f == 2) {
				roi3 = *iter;
				f++;
			}
			else {
				Mat roi_rect = o_img(Rect(roi1, roi2, roi3, *iter));
				Rect roi(roi1, roi2, roi3, *iter);
				roi_rect.copyTo(img(roi));
				f = 0;
			}
		}

		//浅色部分置白
		threshold(img, img, 255, 230, THRESH_TOZERO_INV);


		//处理页眉页脚
		int flag = 0;
		for (int i = 0; i < 250; i++) {
			if (o_img.ptr<uchar>(i, 0)[0] < 150) {
				flag = 1;
				for (int j = 0; j < img.cols; j++) {
					img.ptr<uchar>(i, j)[0] = o_img.ptr<uchar>(i, j)[0];
					img.ptr<uchar>(i, j)[1] = o_img.ptr<uchar>(i, j)[1];
					img.ptr<uchar>(i, j)[2] = o_img.ptr<uchar>(i, j)[2];
				}
			}
		}

		if (flag == 0) {
			for (int i = 0; i < 250; i++) {
				if (o_img.ptr<uchar>(i, img.cols - 1)[0] < 150) {
					flag = 1;
					for (int j = 0; j < img.cols; j++) {
						img.ptr<uchar>(i, j)[0] = o_img.ptr<uchar>(i, j)[0];
						img.ptr<uchar>(i, j)[1] = o_img.ptr<uchar>(i, j)[1];
						img.ptr<uchar>(i, j)[2] = o_img.ptr<uchar>(i, j)[2];
					}
				}
			}
		}

		for (int i = img.rows - 200; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				if (img.ptr<uchar>(i, j)[0] != 255) {
					img.ptr<uchar>(i, j)[0] = o_img.ptr<uchar>(i, j)[0];
					img.ptr<uchar>(i, j)[1] = o_img.ptr<uchar>(i, j)[1];
					img.ptr<uchar>(i, j)[2] = o_img.ptr<uchar>(i, j)[2];
				}
			}
		}

		//浅色部分置白
		threshold(img, img, 255, 240, THRESH_TOZERO_INV);

		string s = *iter;
		const char* temp = s.c_str();
		int i;
		for (i = strlen(temp); i > 0; i--)
		{
			if (temp[i] == '\\') {
				break;
			}
		}

		string save_path = pImgSavePath + String(temp).substr(i + 1, String(temp).size());
		imwrite(save_path, img);
		cout << save_path << endl;

		//imshow("AAAA", img);
		//waitKey(0);
	}
}

CExport::CExport() {
	return;
}