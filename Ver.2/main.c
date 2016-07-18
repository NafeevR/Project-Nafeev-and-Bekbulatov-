#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <zbar.h>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
using namespace cv;
using namespace zbar;
void cut_counter(IplImage* _image,IplImage* img){
  assert(_image!=0);
  IplImage* bin = cvCreateImage( cvGetSize(_image), IPL_DEPTH_8U, 1);
  // конвертируем в градации серого
  cvConvertImage(_image, bin, CV_BGR2GRAY);
  // находим границы
  cvCanny(bin, bin, 50, 200);
  cvNamedWindow( "bin", 1 );
  cvShowImage("bin", bin);
  // хранилище памяти для контуров
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* contours=0;
  //находим контуры
  cvFindContours( bin, storage,&contours,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
  assert(contours!=0);
  int maxx =0;
  int miny = 0;
  char ss[50]= "Pictures/screen";
  char s[50];
  int j=1;
  char b;
  FILE *log = fopen("Pictures/log.txt", "w+");
  ImageScanner scanner;
  scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
  Mat imgs;
  Image image;
  uchar *raw;
  int width, heith;
  // обходим все контуры
  for( CvSeq* current = contours; current != NULL; current = current->h_next){
    if(current->total>5){
      for(int i = 0;i<current->total;++i){
        CvPoint* p1 = (CvPoint*)cvGetSeqElem ( current,i);
        if (p1->x>maxx){
          maxx = p1->x;
        }
        if (p1->y>miny){
          miny = p1->y;
        }
        printf("!!!!!%d %d ",p1->y,p1->x);
      }
     printf("/////////////////////////////////////////////////////////////////////////////////");
     CvPoint* p = (CvPoint*)cvGetSeqElem ( current,0);
     width = (maxx - p->x);
     heith = (miny-p->y);
     cvSetImageROI(img, cvRect(p->x, p->y, width, heith));
     IplImage *sub_img = cvCreateImage(cvGetSize(img), img->depth,img->nChannels);
     cvCopy(img, sub_img, NULL);
     strcpy(s,ss);
     printf("%s\n",s );
     b = j +'0';
     s[15]= b;
     s[16]= '\0';
     strncat (s, ".jpg",4);
     //проверка на qr код
     imgs = cvarrToMat(sub_img, true);
     Mat imgout(imgs.size(), CV_8UC1);
     cvtColor(imgs,imgout, CV_BGR2GRAY);
     width = imgout.cols;
     heith = imgout.rows;
     raw = (uchar *)imgout.data;
     image.set_data(raw, width*heith);
     image.set_format("Y800");
     image.set_size(width,heith);
     scanner.scan(image);
       for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
         fprintf(log, "%s |out-decode| %s \n",s, symbol->get_data().c_str());
         cvSaveImage(s, sub_img);
       }
     image.set_data(NULL, 0);
     j++;
     cvReleaseImage(&sub_img);
     maxx=0;
     miny=0;
     }
     *s='\0';
     strcpy(s,ss);
  }
 // освобождаем ресурсы
  cvReleaseMemStorage(&storage);
  cvReleaseImage(&bin);
}

int main(){
  IplImage* image=0, *dst=0;
  char* filename = "source.jpg" ;
  // получаем картинку
  image = cvLoadImage(filename, 1); //0!
  printf("[i] image: %s\n", filename);
  assert( image != 0 );

  // покажем изображение
  cvNamedWindow( "original");
  cvShowImage( "original", image );

  // картинка для хранения изображения
  dst = cvCloneImage(image);
  // зелёную компоненту до максимума
  for( int y=0; y<dst->height; y++ ) {
    uchar* ptr = (uchar*) (dst->imageData + y * dst->widthStep); // изображение
    for( int x=0; x<dst->width; x++ ) {
      if(!(ptr[3*x] >=0 && ptr[3*x] <=193 && ptr[3*x+1] >25 && ptr[3*x+1] <256 && ptr[3*x+2] >=203 && ptr[3*x+2] <256 )){
        // 3 канала
        ptr[3*x] =0 ; // B
        ptr[3*x+1] = 0; // G
        ptr[3*x+2] =0; // R
      }
    }
  }

  cut_counter(dst,image);
  // ждём нажатия клавиши
  cvWaitKey(0);
  // освобождаем ресурсы
  cvReleaseImage(&dst);
  // удаляем окна
  cvDestroyAllWindows();
 return 0;
}

