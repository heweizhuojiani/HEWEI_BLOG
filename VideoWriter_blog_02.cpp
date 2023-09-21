/** TIPS: VideoWriter 函数 与 VideoCapture 函数有很大的相似之处，都可以通过 isOpened() 函数判断是否成功创建一个视频流。可以通过 get() 
        查看视频流中的各属性。 在保存视频时，我们只需要将生成视频的图像一帧一帧地通过 << 操作符（或者write()函数）赋值给视频流。最后使用 release() 关闭视频流。。**/

#include <opencv2\opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	Mat img;
	/** TIPS: 法一，通过摄像头生成新的视频文件。**/
	VideoCapture video(0);  //使用某个摄像头
  	/** TIPS: 这句代码创建了一个名为 video 的 VideoCapture 对象。VideoCapture 是OpenCV库中用于捕获视频流的类。**/
  	/** TIPS: 在这里，VideoCapture 的构造函数接受一个参数，该参数指定要捕获的视频源。在这个例子中，参数是0，表示使用默认的摄像头作为视频源. **/

	/** TIPS: 在这里，读取已有的视频文件。**/
	//VideoCapture video;
	//video.open("cup.mp4");  

	if (!video.isOpened())  // 判断是否调用成功
	{
		cout << "打开摄像头失败，请确实摄像头是否安装成功";
		return -1;
	}

	video >> img;  //获取图像
	/** TIPS：获取一帧图像并存放到Mat对象img中，这是VideoCature类进行视频捕捉的一种常用方式。 **/
	//检测是否成功获取图像
	if (img.empty())   //判断有没有读取图像成功
	{
		cout << "没有获取到图像" << endl;
		return -1;
	}
	bool isColor = (img.type() == CV_8UC3);  //判断相机（视频）类型是否为彩色
	

	VideoWriter writer;
	int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');  // 选择编码格式
	//OpenCV 4.0版本设置编码格式
	//int codec = CV_FOURCC('M', 'J', 'P', 'G'); 

	double fps = 25.0;  //设置视频帧率 
	string filename = "live.avi";  //保存的视频文件名称
	writer.open(filename, codec, fps, img.size(), isColor);  //创建保存视频文件的视频流

	if (!writer.isOpened())   //判断视频流是否创建成功
	{
		cout << "打开视频文件失败，请确实是否为合法输入" << endl;
		return -1;
	}

	while (1)
	{
		//检测是否执行完毕
		if (!video.read(img))   //判断能都继续从摄像头或者视频文件中读出一帧图像
		{
			cout << "摄像头断开连接或者视频读取完成" << endl;
			break;
		}
		writer.write(img);  //把图像写入视频流
		//writer << img;
		imshow("Live", img);  //显示图像
		char c = waitKey(50);
		if (c == 27)  //按ESC案件退出视频保存
		{
			break;
		}
	}
	// 退出程序时刻自动关闭视频流
	//video.release();
	//writer.release();

	
	return 0;
}
