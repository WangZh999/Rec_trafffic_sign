#include"predict.h"
#include"getFeature.h"
using namespace cv;

ANN_Wz::ANN_Wz()
{
	/*����ѵ���õ����������*/
	_Ann = ml::ANN_MLP::load("../param/nn_param.xml");
	_feature_mat.convertTo(_feature_mat, CV_32F);	//���ڴ��ͼƬ��������
	_result = (Mat_<float>(1, CLASS_NUM));	//���ڴ��Ԥ����
}


int ANN_Wz::predict(cv::Mat img)
{
	float maxVal = -2;
	int result = 0;
	_feature_mat = getFeature(img);
	_Ann->predict(_feature_mat, _result);	//����������ȡ���
	for (int j = 0; j < CLASS_NUM; j++) {	//���������ԭ��
		float val = _result.at<float>(j);
		if (val > maxVal) {
			maxVal = val;
			result = j;
		}
	}

	return result;

	/*if (maxVal < 0.5)
	{
		return -1;
	}
	else
	{
		return result;
	}*/
}


ANN_Wz::~ANN_Wz()
{
}


void test()
{
	cout << "test   test    test" << endl;
}