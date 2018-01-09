#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>
#include <direct.h>
#include <fstream>
#include <string>
using namespace std;
using namespace cv;

//�Ѵ������ת��Ϊ���ǳ��õ�С������
uint32_t swap_endian(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

void readAndSave(const string& mnist_img_path, const string& mnist_label_path)
{
	//�Զ����Ƹ�ʽ��ȡmnist���ݿ��е�ͼ���ļ��ͱ�ǩ�ļ�
	ifstream mnist_image(mnist_img_path, ios::in | ios::binary);
	ifstream mnist_label(mnist_label_path, ios::in | ios::binary);
	if (mnist_image.is_open() == false)
	{
		cout << "open mnist image file error!" << endl;
		return;
	}
	if (mnist_label.is_open() == false)
	{
		cout << "open mnist label file error!" << endl;
		return;
	}

	uint32_t magic;//�ļ��е�ħ����(magic number)
	uint32_t num_items;//mnistͼ���ļ��е�ͼ����Ŀ
	uint32_t num_label;//mnist��ǩ���ļ��еı�ǩ��Ŀ
	uint32_t rows;//ͼ�������
	uint32_t cols;//ͼ�������
				  //��ħ����
	mnist_image.read(reinterpret_cast<char*>(&magic), 4);
	magic = swap_endian(magic);
	if (magic != 2051)
	{
		cout << "this is not the mnist image file" << endl;
		return;
	}
	mnist_label.read(reinterpret_cast<char*>(&magic), 4);
	magic = swap_endian(magic);
	if (magic != 2049)
	{
		cout << "this is not the mnist label file" << endl;
		return;
	}
	//��ͼ��/��ǩ��
	mnist_image.read(reinterpret_cast<char*>(&num_items), 4);
	num_items = swap_endian(num_items);
	mnist_label.read(reinterpret_cast<char*>(&num_label), 4);
	num_label = swap_endian(num_label);
	//�ж����ֱ�ǩ���Ƿ����
	if (num_items != num_label)
	{
		cout << "the image file and label file are not a pair" << endl;
	}
	//��ͼ������������
	mnist_image.read(reinterpret_cast<char*>(&rows), 4);
	rows = swap_endian(rows);
	mnist_image.read(reinterpret_cast<char*>(&cols), 4);
	cols = swap_endian(cols);
	//��ȡͼ��
	for (int i = 0; i != num_items; i++)
	{
		char* pixels = new char[rows * cols];
		mnist_image.read(pixels, rows * cols);
		char label;
		mnist_label.read(&label, 1);
		Mat image(rows, cols, CV_8UC1);
		for (int m = 0; m != rows; m++)
		{
			uchar* ptr = image.ptr<uchar>(m);
			for (int n = 0; n != cols; n++)
			{
				if (pixels[m * cols + n] == 0)
					ptr[n] = 0;
				else
					ptr[n] = 255;
			}
		}
		string saveFile = "./mnist_test/" + to_string((unsigned int)label)+ "/"+to_string((unsigned int)label) + "_" + to_string(i) + ".jpg";
		bool tt=imwrite(saveFile, image);
	}
}


void creatDir(string dir_name)
{
	fstream _file;
	string path;
	_file.open(dir_name, ios::in);
	if (!_file)
	{
		_mkdir(dir_name.c_str());
	}
	for (int i = 0; i < 10; i++)
	{
		path = dir_name + "/" + to_string(i);
		_mkdir(path.c_str());
	}
}


int main()
{
	creatDir("mnist_test");
	//readAndSave("train-images.idx3-ubyte", "train-labels.idx1-ubyte");
	readAndSave("./t10k-images.idx3-ubyte", "./t10k-labels.idx1-ubyte");
	//Mat img = Mat(500, 500, CV_8UC1);
	//cout << (bool)imwrite("./test/test.jpg", img) << false << endl;
	return 0;
}