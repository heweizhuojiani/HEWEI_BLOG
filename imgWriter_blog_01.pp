#include <iostream>
#include <opencv2\opencv.hpp>

using	namespace std;
using	namespace cv;

/**** TIPS: Alpha通道为第四个通道，负责调节图像的透明度，因此具有Alpha通道的图像必然是四通道数据，但这并不意味着三通道数据无法调节透明的，只是原理不同，三通道是通过调节三个颜色通道的数据来实现调节透明度的 ****/
void AlphaMat(Mat &mat)
{
	CV_Assert(mat.channels() == 4);
 	/**** TIPS: 此处为opencv中的特殊宏CV_Assert，其中的参数是一个bool类型条件，如果条件不满足会报错，只能用于调试，因为可能泄漏项目机密。 ****/
	for (int i = 0; i < mat.rows; ++i)
	{
		for (int j = 0; j < mat.cols; ++j)
		{
			Vec4b& bgra = mat.at<Vec4b>(i, j);
   			/**** TIPS: 此处Vec4b表示第四通道的uchar类型数据，其中b是uchar的缩写，4表示通道数，因此其他通道的数据同理为：Vec+通道数+数据类型。 ****/
      			/**** TIPS: 此处的 & bgra 表示引用，这意味着它不会创建像素值的副本，而是直接引用 at 函数返回的像素值，这样可以通过修改bgra来修改像素值 ****/
			bgra[0] = UCHAR_MAX;  // 蓝色通道
   			/**** TIPS: 在图像处理和计算机图形学中，UCHAR_MAX 常常用于表示颜色通道的最大值。在8位颜色通道中，0 表示完全不透明或没有颜色，255 表示完全不透明或最大强度的颜色。对于RGB颜色空间，255 表示红色、绿色和蓝色通道中的最大值，因此 (255, 0, 0) 表示纯红色，(0, 255, 0) 表示纯绿色，(0, 0, 255) 表示纯蓝色。****/
			bgra[1] = saturate_cast<uchar>((float(mat.cols - j)) / ((float)mat.cols) * UCHAR_MAX);  // 绿色通道
			bgra[2] = saturate_cast<uchar>((float(mat.rows - i)) / ((float)mat.rows) * UCHAR_MAX);  // 红色通道
   			/**** TIPS: 这两行代码是在处理图像的绿色和红色通道，根据像素的位置来计算并设置通道的值。 saturate_cast<uchar>是OPENCV 4中一个函数，用于防止数据超出指定数据类型范围，防止溢出。 ****/
			/**** TIPS: 这两行代码根据像素的位置在图像中计算并设置了绿色通道和红色通道的值。这种方式可以根据像素的位置来调整颜色通道的亮度，从而实现特定的颜色效果。这在图像处理和计算机视觉中常常用于创建视觉效果和图像处理。****/
   			bgra[3] = saturate_cast<uchar>(0.5 * (bgra[1] + bgra[2]));  // Alpha通道
		}
	}
}
int main(int agrc, char** agrv)
{
	// Create mat with alpha channel
	Mat mat(480, 640, CV_8UC4);
 	/**** TIPS：创建一个四通道矩阵（前三个是颜色通道，第四个是Alpha通道. ****/
	AlphaMat(mat);
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);  //PNG格式图像压缩标志
	compression_params.push_back(9);  //设置最高压缩质量		
	bool result = imwrite("alpha.png", mat, compression_params);
	if (!result)
	{
		cout << "保存成PNG格式图像失败" << endl;
		return -1;
	}
	cout << "保存成功" << endl;
	return 0;
}
