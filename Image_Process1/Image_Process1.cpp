#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <math.h>
//User defined includes
#include "imge_bmp.h"
using namespace std;
void drawline(int x1, int y1, int x2, int y2, int w, int h, BYTE * raw) {
	double m = (y2 - y1) / (x2 - x1);
	int y;
	for (double x = x1; x < x2; x += 0.01) {
		y = m * x + y1;
		raw[y*w + (int)x] = 255;
	}
}
void drawcircle(int x1, int y1, int r, int w, int h, BYTE * raw) {
	double ytemp; // (y-b)'2
	int Y;
	for (double x = x1 - r; x < r + x1; x = x + 0.01) {
		ytemp = r * r - (x - x1)*(x - x1);
		raw[(int)(sqrt(ytemp) + y1) *w + (int)x] = 255;
		raw[(int)(-sqrt(ytemp) + y1)*w + (int)x] = 255;
	}
}
void drawelips(int x1, int y1, int ra, int rb, int w, int h, BYTE * raw) {
	double ytemp;
	int Y;
	for (double x = x1 - ra; x < x1 + ra; x = x + 0.01) {
		ytemp = rb * rb * (1 - (x - x1)*(x - x1) / (ra* ra));
		raw[(int)(sqrt(ytemp) + y1) *w + (int)x] = 255;
		raw[(int)(-sqrt(ytemp) + y1)*w + (int)x] = 255;
	}
}
void toplama(int w, int h, BYTE *raw1, BYTE * raw2) {
	for (int i = 0; i < w*h * 3; i++) {
		raw1[i] = (raw1[i] + raw2[i]) / 2;
	}
}
BYTE* zoom(int x1, int y1, int x2, int y2, int w, int h, BYTE* buffer2)
{
	int w2 = x2 - x1, h2 = y2 - y1;
	BYTE* buffer3 = new BYTE[w2*h2];
	int deger = 0;
	for (int j = y1; j < y2; j++) {
		for (int i = x1; i < x2; i++) {
			if (i <= x2 && i >= x1)
				buffer3[deger++] = buffer2[j*w + i];

		}
	}
	return buffer3;

}
BYTE* zeroadd(BYTE* image, int h, int w) {
	int h1 = 2 * h + 1;
	int w1 = 2 * w + 1;
	int hold = 0;
	BYTE * buffer2 = new BYTE[w1*h1];
	for (int j = 0; j < h1; j++) {
		for (int i = 0; i < w1; i++) {
			if (i % 2 == 0 || j % 2 == 0)
				buffer2[j*w1 + i] = 0;
			else {
				buffer2[j*w1 + i] = image[hold];
				hold++;
			}
		}
	}

	return buffer2;
}
BYTE *Zoomout(BYTE* image, int h, int w, int x1, int y1, int x2, int y2, float *mask, int mh, int mw) {
	BYTE* buffer0 = zoom(x1, y1, x2, y2, w, h, image);
	int h0 = y2 - y1;
	int w0 = x2 - x1;
	BYTE* buffer1 = zeroadd(buffer0, h0, w0);
	int h1 = 2 * h0 + 1;
	int w1 = 2 * w0 + 1;
	int hold = 0;


	BYTE * omax = new BYTE[w1*h1];
	for (int i = 0; i < h1; i++) {
		for (int j = 0; j < w1; j++) {
			omax[i * w1 + j] = 0;
		}
	}

	int total = 0;
	for (int raw = 1; raw < h1 - mh / 2; raw++) {
		for (int col = 1; col < w1 - mw / 2; col++) {

			for (int i = -1; i < mh - 1; i++) {
				for (int j = -1; j < mw - 1; j++) {
					total += buffer1[(raw + i)*w1 + (col + j)] * mask[(i + 1) * mw + j + 1];
				}
			}
			omax[raw*w1 + col] = total;
			total = 0;
		}
	}
	return omax;
}
BYTE *	Convolition_330074(BYTE *image, int h, int w, float *mask, int mh, int mw) {
	int  h1 = h + mw - 1;
	int  w1 = w + mw - 1;
	int hold = 0;
	BYTE * buffer2 = new BYTE[w1*h1];
	for (int j = mw / 2; j < h1 - mw / 2; j++) {
		for (int i = mw / 2; i < w1 - mw / 2; i++) {


			buffer2[j*w1 + i] = image[hold];
			hold++;

		}
	}
	int k = 0;
	for (int i = 0; i < mh*mh; i++) {
		k += mask[i];
	}
	if (k == 0)k = 1;


	BYTE * omax = new BYTE[w1*h1];
	for (int i = 0; i < h1; i++) {
		for (int j = 0; j < w1; j++) {
			omax[i * w1 + j] = 0;
		}
	}

	double total = 0;
	for (int raw = mh / 2; raw < h1 - mh / 2; raw++) {
		for (int col = mh / 2; col < w1 - mh / 2; col++) {

			for (int i = -(mh / 2); i < mh - mh / 2; i++) {
				for (int j = -(mh / 2); j < mw - mh / 2; j++) {
					total += buffer2[(raw + i)*w1 + (col + j)] * mask[(i + mh / 2) * mw + j + mh / 2];
				}
			}
			omax[raw*w1 + col] = (int)abs(total / k);
			total = 0;
		}
	}
	return omax;
}
BYTE* histogramCikart(BYTE* Buffer, int width, int height) {
	int* hist = new int[256];				//Renk tablosu olusturuldu
	int *level = new int[8], *eleman_sayisi = new int[8];

	//Dizileri sýfýrlar
	for (int i = 0; i < 256; i++) {
		hist[i] = 0;						//Bize buffer ile gelen degerler renk bilgisi
	}
	for (int i = 0; i < 8; i++) {
		level[i] = 0;
		eleman_sayisi[i] = 0;
	}

	for (int i = 0; i < width*height; i++) {
		hist[Buffer[i]]++;					//Gelen renklerin hangi renk oldugunu bolumledik
	}

	int onceki_sayi = 0, anlik_sayi = 0;
	/* GRAY LEVEL PIKSEL SAYILARI */
	for (int i = 0; i < 256; i++) {			//Renklerin olmasý gereken renkleri bulduk
		onceki_sayi += hist[i];				//Bunda 255 ile carpmaz ve önceki sayiyi tutmazsan PMF bulunur
		hist[i] = (onceki_sayi * 255) / (width*height);
	}

	long newpos = 0;						//Bufferden rengi al, hangisine yakýn olduguna bak, oraya yuvarla
	int renk;
	for (int i = 0; i < width*height; i++) {
		renk = Buffer[i];
		*(Buffer + i) = hist[renk];
	}



	return Buffer;
}
BYTE *KMeans_330074(BYTE *image, int w, int h, int k1, int k2, int ss) {
	int* hist = new int[256];

	for (int i = 0; i < 256; i++) {
		hist[i] = 0;
	}

	for (int i = 0; i < w*h; i++) {
		hist[image[i]]++;
	}

	int onceki_sayi = 0, anlik_sayi = 0;

	for (int i = 0; i < 256; i++) {
		onceki_sayi += hist[i];
		hist[i] = (onceki_sayi * 255) / (w*h);
	}
	int renk;
	for (int i = 0; i < w*h; i++) {
		renk = image[i];
		*(image + i) = hist[renk];
	}
	BYTE * buffer = new BYTE[h*w];
	long int ortalama = 0;
	long int standart = 0;

	for (int f = 0; f < w*h; f++)
	{
		ortalama += image[f];
	}
	ortalama = ortalama / (w*h);
	for (int s = 0; s < w * h; s++)
		standart += pow((image[s] - ortalama), 2);

	standart = sqrt(standart / (w*h));
	int count1 = 0, count2 = 0;
	for (int i = 0; i < w*h; i++) {
		if (sqrt(pow(abs(image[i] - k1), 2) / pow(ss, 2)) < sqrt(pow(abs(image[i] - k2), 2) / pow(ss, 2))) {
			buffer[i] = 0;
			count1++;
		}
		else {
			buffer[i] = 255;
			count2++;
		}
	}
	int sum = 0;
	int sumk1 = 0;
	int sumk2 = 0;

	for (int j = 0; j < w*h; j++) {
		if (buffer[j] == 0)
			sumk1 += image[j];
		else
			sumk2 += image[j];
	}
	sumk1 = sumk1 / count1;
	sumk2 = sumk2 / count2;
	if (sumk1 == k1 && sumk2 == k2)
		return buffer;
	else
		return KMeans_330074(image, w, h, sumk1, sumk2, standart);


}
BYTE* xturev(BYTE *image, int w, int h) {
	int  h1 = h + 2;
	int  w1 = w + 2;
	double total = 0;
	BYTE * ximage = new BYTE[w1*h1];
	double mask1[9] = { -1,-2,-1,
						0,0,0,
						1,2,1, };
	for (int raw = 1; raw < h1 - 1; raw++) {
		for (int col = 1; col < w1 - 1; col++) {

			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					total += image[(raw + i)*w1 + (col + j)] * mask1[(i + 1) * 3 + j + 1];
				}
			}
			ximage[raw*w1 + col] = (int)abs(total);
			total = 0;
		}
	}
	return ximage;
	
}
BYTE* yturev(BYTE *image, int w, int h) {
	int  h1 = h + 2;
	int  w1 = w + 2;
	double total = 0;
	BYTE * yimage = new BYTE[w1*h1];
	double mask1[9] = { -1, 0, 1,
		-2, 0, 2,
		-1, 0, 1, };
	for (int raw = 1; raw < h1 - 1; raw++) {
		for (int col = 1; col < w1 - 1; col++) {

			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					total += image[(raw + i)*w1 + (col + j)] * mask1[(i + 1) * 3 + j + 1];
				}
			}
			yimage[raw*w1 + col] = (int)abs(total);
			total = 0;
		}
	}
	return yimage;
}
BYTE* imagetoplam(BYTE *imagex, BYTE *imagey, int w, int h) {
	int  h1 = h + 2;
	int  w1 = w + 2;
	double total = 0;
	BYTE * imagesonuc = new BYTE[w1*h1];
	for (int i = 0; i < w1*h1; i++) {
		/*sqrt(ximage[i] * ximage[i] + yimage[i] * yimage[i]);*/
		imagesonuc[i] = imagex[i] + imagey[i];
	}
	return imagesonuc;
}
BYTE* nanmaxima( BYTE *total,BYTE* imagex, BYTE* imagey, int w, int h) {
	int  h1 = h + 2;
	int  w1 = w + 2;
	int * holdangle = new int[w1*h1];
	int aci = 0;
	int a;

	for (int i = 0; i < w1*h1; i++) {
		if (imagex[i] == 0)
			holdangle[i] = 90;
		else {
			aci = atan(imagey[i] / imagex[i]) * 57;
		
			if (((aci < 22.5) && (aci > -22.5)) || (aci > 157.5) || (aci < -157.5))
				holdangle[i] = 0;
			else if (((aci > 22.5) && (aci < 67.5)) || ((aci < -112.5) && (aci > -157.5)))
				holdangle[i] = 45;
			else if (((aci > 67.5) && (aci < 112.5)) || ((aci < -67.5) && (aci > -112.5)))
				holdangle[i] = 90;
			else if (((aci > 112.5) && (aci < 157.5)) || ((aci < -22.5) && (aci > -67.5)))
				holdangle[i] = 135;


		}
	}
	for (int raw = 1; raw < h1 - 1; raw++) {
		for (int col = 1; col < w1 - 1; col++) {
			switch (holdangle[aci])
			{
			case 0:
				if (total[raw*w1 + col] < total[raw*w1 + col - 1] || total[raw*w1 + col] < total[raw*w1 + col + 1])
					total[raw*w1 + col] = 0;
				break;
			case 45:
				if (total[raw*w1 + col] < total[(raw - 1)*w1 + col + 1] || total[raw*w1 + col] < total[(raw + 1)*w1 + col - 1])
					total[raw*w1 + col] = 0;
				break;
			case 90:
				if (total[raw*w1 + col] < total[(raw - 1)*w1 + col] || total[raw*w1 + col] < total[(raw + 1)*w1 + col])
					total[raw*w1 + col] = 0;
				break;
			case 135:
				if (total[raw*w1 + col] < total[(raw - 1)*w1 + col - 1] || total[raw*w1 + col] < total[(raw + 1)*w1 + col - 1])
					total[raw*w1 + col] = 0;
				break;

			}
		}
	}
	return total;
}
//histerir
BYTE* trashhold(BYTE *total, int w, int h) {
	int  h1 = h + 2;
	int  w1 = w + 2;
	int high = 100;
	int low = 30;
	for (int raw = 1; raw < h1 - 1; raw++) {
		for (int col = 1; col < w1 - 1; col++) {
			if (total[raw*w1 + col] <= low)
				total[raw*w1 + col] = 0;
			else if (total[raw*w1 + col] > low && total[raw*w1 + col] <= high) {
				if (total[(raw - 1)*w1 + col] == 1 || total[(raw - 1)*w1 + col - 1] == 1 || total[(raw - 1)*w1 + col + 1] == 1 ||
					total[(raw)*w1 + col] == 1 || total[raw*w1 + col - 1] == 1 || total[raw *w1 + col + 1] == 1 ||
					total[(raw + 1)*w1 + col] == 1 || total[(raw + 1)*w1 + col - 1] == 1 || total[(raw + 1)*w1 + col + 1] == 1)
					total[raw*w1 + col] = 255;
				else
					total[raw*w1 + col] = 0;

			}
			else
				total[raw*w1 + col] = 255;
		}
	}
	return total;
}
BYTE * ikinciproje(BYTE *image, int w, int h) {
	int  h1 = h + 2;
	int  w1 = w + 2;
	int hold = 0;
	BYTE * buffer2 = new BYTE[w1*h1];
	for (int i = 0; i < w1*h1; i++)
		buffer2[i] = 0;
	for (int j = 1; j < h1 - 1; j++) {
		for (int i = 1; i < w1 - 1; i++) {
			buffer2[j*w1 + i] = image[hold];
			hold++;
		}
	}
	hold = 0;
	BYTE * buffer3 = new BYTE[w1*h1];
	for (int i = 0; i < w1*h1; i++)
		buffer3[i] = 0;
	for (int j = 1; j < h1 - 1; j++) {
		for (int i = 1; i < w1 - 1; i++) {
			buffer3[j*w1 + i] = image[hold];
			hold++;
		}
	}
	BYTE * ximage = xturev(buffer2, w, h);

	BYTE * yimage = yturev(buffer2, w, h);

	BYTE * totalimage = imagetoplam(ximage, yimage, w, h);

	totalimage = nanmaxima(totalimage, ximage, yimage, w, h);
	totalimage = trashhold(totalimage, w, h);
	
	

	return totalimage;
}

int _tmain(int argc, _TCHAR* argv[])
{
	LPCTSTR input, input2, input3, output;
	int Width, Height;
	long Size, new_size;
	int dx = 200;
	int dy = 200;
	float matris[9] = { 1,2,1,0,0,0,-1,-2,-1 };
	int masken = 3;
	float convMatrix7[49] = { 0.015625 ,    0.03125,    0.0625,    0.125,    0.0625,    0.03125,0.015625,
								0.03125,    0.0625,        0.125,    0.25,    0.125,    0.0625,0.03125,
								0.625,        0.125,        0.25,    0.5,    0.25,    0.125,0.625,
								0.125,        0.25,        0.5,1,    0.5,    0.25,    0.125,
								0.625,        0.125,        0.25,    0.5,    0.25,    0.125,0.625,
								0.03125,    0.0625,        0.125,    0.25,    0.125,    0.0625,0.03125,
								0.015625,    0.03125,    0.0625, 0.125,    0.0625,    0.03125,0.015625 };

	cout << "Haydi Bismillah" << endl;
	input = L"C://Users//sulu//Desktop//images//boru.bmp";  //kendi bilgisayarýna göre lokasyonu degistir
	/*input2 = L"C://Users//sulu//Desktop//images//a2.bmp";
	input3 = L"C://Users//sulu//Desktop//images//a1.bmp";*/
	BYTE* buffer = LoadBMP(&Width, &Height, &Size, input);
	/*BYTE* buffer2 = LoadBMP(&Width, &Height, &Size, input2);
	BYTE* buffer3 = LoadBMP(&Width, &Height, &Size, input3);*/
	BYTE* raw_intensity = ConvertBMPToIntensity(buffer, Width, Height);
	/*drawline(500, 500, 600, 600, Width, Height, raw_intensity);*/
	/*drawcircle(500, 500, 200, Width, Height, raw_intensity);
	drawelips(300, 300, 200, 100, Width, Height, raw_intensity);*/
	/*toplama(Width, Height, buffer2, buffer3);*/
	int h1 = Height + masken - 1;
	int w1 = Width + masken - 1;
	int hold = 0;
	BYTE * buffer2 = new BYTE[w1*h1];
	for (int i = 0; i < w1*h1; i++)
		buffer2[i] = 0;
	for (int j = 1; j < h1 - 1; j++) {
		for (int i = 1; i < w1 - 1; i++) {
			buffer2[j*w1 + i] = raw_intensity[hold];
			hold++;
		}
	}
	hold = 0;
	BYTE * buffer3 = new BYTE[w1*h1];
	for (int i = 0; i < w1*h1; i++)
		buffer3[i] = 0;
	for (int j = 1; j < h1 - 1; j++) {
		for (int i = 1; i < w1 - 1; i++) {
			buffer3[j*w1 + i] = raw_intensity[hold];
			hold++;
		}
	}
	/*zoom(0, 0, 300, 300, Width, Height, raw_intensity)*/
	char ch = 'e';
	if ((ch == 'E') || (ch == 'e')) {
		/*BYTE* display_imge = ConvertIntensityToBMP(Zoomout(raw_intensity, Height, Width, 50, 50, 200, 200, matris, 3, 3), w1, h1, &new_size);
		BYTE* display_imge = ConvertIntensityToBMP(histogramCikart(raw_intensity,Width,Height), Width, Height, &new_size);
		BYTE* display_imge = ConvertIntensityToBMP(KMeans_330074(raw_intensity, Width, Height, 0, 255, 1), Width, Height, &new_size);*/
		/*BYTE* display_imge = ConvertIntensityToBMP(xturev(buffer2, Width, Height), w1, h1, &new_size);
		output = L"C://Users//sulu//Desktop//images//1xturev.bmp";
		SaveBMP(display_imge, w1, h1, new_size, output);
		display_imge = ConvertIntensityToBMP(yturev(buffer3, Width, Height), w1, h1, &new_size);
		output = L"C://Users//sulu//Desktop//images//2yturev.bmp";
		SaveBMP(display_imge, w1, h1, new_size, output);
		display_imge = ConvertIntensityToBMP(imagetoplam(xturev(buffer2, Width, Height), yturev(buffer3, Width, Height), Width, Height), w1, h1, &new_size);
		output = L"C://Users//sulu//Desktop//images//3toplam.bmp";
		SaveBMP(display_imge, w1, h1, new_size, output);
		display_imge = ConvertIntensityToBMP(nanmaxima(imagetoplam(xturev(buffer2, Width, Height), yturev(buffer3, Width, Height), Width, Height), xturev(buffer2, Width, Height), yturev(buffer3, Width, Height), Width, Height), w1, h1, &new_size);
		output = L"C://Users//sulu//Desktop//images//4nan.bmp";
		SaveBMP(display_imge, w1, h1, new_size, output);*/
		 BYTE *display_imge = ConvertIntensityToBMP(ikinciproje(raw_intensity, Width, Height)/*raw_intensity*/, w1, h1, &new_size);
		output = L"C://Users//sulu//Desktop//images//5sonuc.bmp"; //kendi bilgisayarýna göre lokasyonu degistir
		SaveBMP(display_imge, w1, h1, new_size, output);

	}//
	delete[] buffer;

	return 0;
}

