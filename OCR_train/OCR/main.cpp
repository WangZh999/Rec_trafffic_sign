#include"train.h"
#include<opencv.hpp>

using namespace std;
using namespace cv;

int main()
{

	//getFeature("E:\\OCR\\Train\\B\\10-5.jpg");	//��ȡͼ����������

		
	/*Mat train_data_mat;		//ͼ������������
	Mat labels_mat;			//��Ӧ��ͼ���������
	train_data_mat.convertTo(train_data_mat, CV_32F);
	GetData("E:\\OCR\\Train", train_data_mat, labels_mat);*/


	train("E:\\OCR\\mnist_train"); //ѵ��



	//����ͼƬ����
	//char filename[100];
	//ANN_Wz ann = ANN_Wz();
	//int result;
	//while (1) {
	//	gets_s(filename);
	//	Mat img = imread(filename, 0);
	//	if (NULL==img.data)
	//	{
	//		cout << "NULL" << endl;
	//		continue;
	//	}
	//	result = ann.predict(img);
	//	cout << result << endl;
	//}


	//float rate;
	//rate=test("E:\\OCR\\TestDataSet");
	//cout << rate << endl;
	return 0;
   }

