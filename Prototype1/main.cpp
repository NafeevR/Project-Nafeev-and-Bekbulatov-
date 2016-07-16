#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
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
 char s[50]= "../Pictures/screen";
 int j=0;
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
  int width = (maxx - p->x);
  int heith = (miny-p->y);
  cvSetImageROI(img, cvRect(p->x, p->y, width, heith));
  IplImage *sub_img = cvCreateImage(cvGetSize(img), img->depth,img->nChannels);
  cvCopy(img, sub_img, NULL);
  strncat (s, "abcdefgh",j);
  strncat (s, ".jpg",4);
  cvSaveImage(s, sub_img);
  j++;
  cvReleaseImage(&sub_img);
  maxx=0;
  miny=0;
  }
 }
 // освобождаем ресурсы
 cvReleaseMemStorage(&storage);
 cvReleaseImage(&bin);
}

int main(){
 IplImage* image=0, *dst=0;
 char* filename = "source.jpg" ;
 // получаем картинку
 image = cvLoadImage(filename, 1);
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
 //запускаем bash-скрипт, на выходе которого лог с названием изображения-содержанием кода
 system("make --directory ../Pictures/");
 system("cd ../Pictures/ && ./photo.sh");
 return 0;
}
