#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>

using namespace std;
using namespace cv;

//Yolun çercevesini aldýk
Point2f noktalar[] = { Point2f(670,593),Point2f(953,593),Point2f(194,806),Point2f(1125,806) };

//Kuþ bakýþý görünüm 
Point2f donusum[] = { Point2f(0,0),Point2f(640,0),Point2f(640,480),Point2f(0,480) };

vector<int> serit;
Mat ilgili_bölge;

Mat sonuc;

int solKonum, sagKonum;

int pid;//PÝD arastýr


//Binary dönüsüm
void renk_donusumu(Mat giris)
{
	Mat cikis;

	cvtColor(giris, giris, COLOR_BGR2GRAY);

	threshold(giris, cikis, 120, 255, THRESH_BINARY);

	Canny(giris, giris, 70, 175);//Kenar

	add(giris, cikis, sonuc);//Canny+Binary

	//imshow("Sonuc", sonuc);
}



void serit_bul()
{
	serit.resize(640);
	serit.clear();

	for (int i = 0; i < 630; i++)//Seritleri bulmak için,degiþebilir
	{
		ilgili_bölge = sonuc(Rect(i, 150, 2, 100));//Degisebilir deðer görüntüye göre

		divide(255, ilgili_bölge, ilgili_bölge);

		serit.push_back((int)(sum(ilgili_bölge)[0]));

	}

}



void serit_goster(Mat son)
{
	vector<int>::iterator sol;
	
	sol = max_element(serit.begin(), serit.begin() + 150);

	solKonum = distance(serit.begin(), sol);

	vector<int>::iterator sag;

	sag = max_element(serit.begin()+350, serit.end());

	sagKonum = distance(serit.begin(), sag);

	line(son, Point(solKonum, 0), Point(solKonum, 480),Scalar(0,156,157), 3);

	line(son, Point(sagKonum, 0), Point(sagKonum, 480),Scalar(0, 156, 157), 3);

	cout << "Sag Þerit Mesafe:" << sagKonum << endl << "Sol Þerit Mesafe:" << solKonum << endl;

	//imshow("Þerit Tespiti", son);

}



void serit_merkez_bul(Mat son)
{
	int seritMerkez = (sagKonum - solKonum) / 2 + solKonum;

	int kalibre = son.cols / 2 - 16;

	pid = seritMerkez - kalibre;

	line(son, Point(seritMerkez, 0), Point(seritMerkez, 480), Scalar(0, 0, 255),3);

	line(son, Point(kalibre,0), Point(kalibre,480), Scalar(255, 0, 0), 1);//Resim ortasý

	imshow("Son Durum",son);
}



int main()
{
	//Mat resim = imread("C:/Users/savaseeratesli/Desktop/otoban.png"); 

	VideoCapture serit("C:/Users/savaseeratesli/Desktop/yol.mkv");

	if (!serit.isOpened()) cout << "Serit bulma islemi basarisiz!";
	else
	{
		while (true)//Video sonsuz döngü
		{
			Mat oku;
			serit.read(oku);


			Mat pers = getPerspectiveTransform(noktalar, donusum);

			Mat son(480, 640, CV_8UC3);
			warpPerspective(oku, son, pers, son.size());

			renk_donusumu(son);

			serit_bul();

			serit_goster(son);

			serit_merkez_bul(son);

			stringstream yazi;

			yazi.str(" ");

			yazi.clear();

			yazi << "PID Degeri:" << pid;

			putText(oku, yazi.str(), Point(150, 150), FONT_HERSHEY_TRIPLEX, 2, Scalar(0, 125, 240), 3);


			imshow("Serit", son);

			imshow("Orjinal", oku);

			if (waitKey(50) == 32) break;;

		}
	}

}


