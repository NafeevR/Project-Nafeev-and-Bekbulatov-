#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <zbar.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <time.h>
using namespace cv;
using namespace zbar;
int wit=0;int hi=0; //Ширина и высота ячейки;Считаем что таблица имеет примерно равные ячейки
int maxx2 =0;
int maxy2 = 0;

//Расчет проверка кода
void colour(IplImage* dst,int Rmin,int Rmax,int Gmin,int Gmax,int Bmin,int Bmax){
  for( int y=0; y<dst->height; y++ ) {
    uchar* ptr = (uchar*) (dst->imageData + y * dst->widthStep); // изображение
    for( int x=0; x<dst->width; x++ ) {
      if(!(ptr[3*x] >=Bmin && ptr[3*x] <=Bmax && ptr[3*x+1] >=Gmin && ptr[3*x+1] <=Gmax && ptr[3*x+2] >=Rmin && ptr[3*x+2] <=Rmax )){
      // 3 канала
      ptr[3*x] =0 ; // B
      ptr[3*x+1] = 0; // G
      ptr[3*x+2] =0; // R
      }
    }
  }
}

void raschet(IplImage* dst,IplImage* dst1){
  IplImage* bin = cvCreateImage( cvGetSize(dst), IPL_DEPTH_8U, 1);
  // конвертируем в градации серого
  cvConvertImage(dst, bin, CV_BGR2GRAY);
  // находим границы
  cvCanny(bin, bin, 50, 200);
  cvNamedWindow( "bin11", 1 );
  cvShowImage("bin11", bin);
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* contours=0;
  //находим контуры
  cvFindContours( bin, storage,&contours,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
  assert(contours!=0);
  for( CvSeq* current = contours; current != NULL; current = current->h_next){
    if(current->total>2){
      for(int i = 0;i<current->total;++i){
        CvPoint* p1 = (CvPoint*)cvGetSeqElem ( current,i);
        if (p1->x>maxx2){
          maxx2 = p1->x;
        }
        if (p1->y>maxy2){
          maxy2 = p1->y;
        }
// printf("%d %d //",p1->x,p1->y);
      }
    }
  }
  IplImage* bin1 = cvCreateImage( cvGetSize(dst1), IPL_DEPTH_8U, 1);
  // конвертируем в градации серого
  cvConvertImage(dst1, bin1, CV_BGR2GRAY);
  // находим границы
  cvCanny(bin1, bin1, 50, 200);
  cvNamedWindow( "bin111", 1 );
  cvShowImage("bin111", bin1);
  CvMemStorage* storage1 = cvCreateMemStorage(0);
  CvSeq* contours1=0;
  cvFindContours( bin1, storage1,&contours1,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
  assert(contours!=0);
  int maxx1=10000,maxy1=10000;
  for( CvSeq* current = contours1; current != NULL; current = current->h_next){
    if(current->total>5){
      for(int i = 0;i<current->total;++i){
        CvPoint* p1 = (CvPoint*)cvGetSeqElem ( current,i);
        if (p1->x<maxx1){
          maxx1 = p1->x;
        }
        if (p1->y<maxy1){
          maxy1 = p1->y;
        }
      }
    }
  }
  printf("%d %d %d %d",maxy2,maxx2,maxy1,maxx1);
  wit= maxx1-maxx2;
  hi = maxy2-maxy1;
  maxy2 = maxy1;
}

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
  char ss[50]= "Stickers/screen";
  char s[50];
  int d[2][2] ={0}; // !!!!!!!!Твоя переменная первая сточка зеленный чел , вторая красный,1 столбец тип in progress второй столбец done на основе их проверки делаешь тип если там ноль значит там ее нет
//дальше думаю те ничего не понадобится больше по крайне мере чо надо будет пишщи а вроде ка ты с другими данными работать то и не будешь
  int j=1;
  char b;
  FILE *log; // = fopen("Stickers/log.csv", "r+");
  ImageScanner scanner;
  scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
  Mat imgs;
  Image image;
  uchar *raw;
  int width, heith;
  int maxx = 0;
  int miny=0;
  int c=0; int key=-1; time_t seconds; tm* timeinfo; char buffer[80]; char str[80]; char buf[80], *ptr1, *ptr2; int count=0; long pos,ex,in; char mas[10][80];
  printf("%d ",hi);
//printf("%d ",maxy2);
  printf("%d ",wit);
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
      }
      CvPoint* p = (CvPoint*)cvGetSeqElem ( current,0);
      width = (maxx - p->x);
      heith = (miny-p->y);
      if ((miny-(maxy2)>0) && ((miny)-(maxy2+hi)<0)){
        if((maxx2-maxx<=0) && ((maxx2+wit)-maxx>=0)){
          d[0][0]=1;
        } else {
          d[0][1]=1;
        }
      }
      printf("%d %d %d %d//// \n",miny,maxy2,hi,heith);
      if ((miny-(maxy2+hi)>0) && ((miny)-(maxy2+2*hi)<0)){
        if((maxx2-maxx<=0) && ((maxx2+wit)-maxx>=0)){
          d[1][0]=1;
        } else {
          d[1][1]=1;
        }
      }
      cvSetImageROI(img, cvRect(p->x, p->y, width, heith));
      IplImage *sub_img = cvCreateImage(cvGetSize(img), img->depth,img->nChannels);
      cvCopy(img, sub_img, NULL);
      strcpy(s,ss);
      b = j +'0';
      s[15]= b;
      s[16]= '\0';
      strncat (s, ".jpg",4);
//cvSaveImage(s, sub_img);
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
         c=0; count=0; ptr1=ptr2=NULL; ex=0; in=0;
         while (c<2){
         if (d[c][0]==1 || d[c][1]==1) key = c;
	   c++; 
         }

        //if (key) {
          switch (key) {
	    case 0: //нужна проверка на назначение было или нет?
              log = fopen("Stickers/log.csv", "a+t");
              
              seconds = time(NULL);
              timeinfo = localtime(&seconds);
              strftime (buffer,80,"%F_%R",timeinfo);
	      if (d[0][0]==1){
                fseek(log,0,SEEK_END); //проверка на пустоту файла
                pos=ftell(log);
                if(pos==0){
                  rewind(log);
                  fprintf(log, "%s;%s;InProgress;Green;%s\n",s, symbol->get_data().c_str(),buffer);
                } else { 
                  rewind(log); //восстановление указателя
	          while(!feof(log) && !count){
                    fscanf(log, "%s\n", str);
 	            strcpy(buf,str);
                    ptr1 = strstr(buf,";");
                    ptr2 = strstr(ptr1+1,";");
                    *ptr2 = '\0';
	            strcpy(buf,ptr1+1);
		    printf("%s %s\n",symbol->get_data().c_str(), buf);
                    ex++;
		    if (strcmp(symbol->get_data().c_str(), buf))
		      in++; 
                  }  
                  if (ex==in) 
                    fprintf(log, "%s;%s;InProgress;Green;%s\n",s, symbol->get_data().c_str(),buffer);
                }
              } 
              if (d[0][1]==1) {
                rewind(log); //восстановление указателя
	        while(!feof(log) && !count){
                  fscanf(log, "%s\n", str);
                  strcpy(mas[in],str);
 	          strcpy(buf,str);
                  ptr1 = strstr(buf,";");
                  ptr2 = strstr(ptr1+1,";");
                  *ptr2 = '\0';
	          strcpy(buf,ptr1+1);
		  printf("%s %s\n",symbol->get_data().c_str(), buf);
		  if (!strcmp(symbol->get_data().c_str(), buf)){
		    ptr2 = strstr(strstr(str,";")+1,";");
                    if (*(ptr2+1)=='I') { //от повторного прогона фото с Done
                      ptr1 = strstr(strstr(strstr(strstr(str,";")+1,";")+1,";")+1,";");
		      strcpy(buf,ptr1+1);
                      sprintf(str, "%s;%s;Done;Green;%s",s, symbol->get_data().c_str(),buf);
                      strcpy(mas[in],str);
                      strcat(mas[in],";");
		      strcat(mas[in], buffer);
                      count++;
                    }
                  }
                  in++;  
                }
                if (ex==0 && in){
		  fclose(log);
	          log = fopen("Stickers/log.csv", "w+");
                  for (int i=0; i<in; i++){                 
                    fprintf(log,"%s\n",mas[i]);
                  }    
                }
              }
              fclose(log);
	      //else
                //найти и изменить fprintf(log, "%s;%s;Green;%s \n",s, symbol->get_data().c_str(),asktime(timeinfo)); 
              break;
	    case 1:
              log = fopen("Stickers/log.csv", "a+t");
              seconds = time(NULL);
              timeinfo = localtime(&seconds);
              strftime (buffer,80,"%F_%R",timeinfo);
	      if (d[1][0]==1){
                fseek(log,0,SEEK_END);
                pos=ftell(log);
                if(pos==0){
                  rewind(log);
                  fprintf(log, "%s;%s;InProgress;Red;%s\n",s, symbol->get_data().c_str(),buffer);
                } else {
                  rewind(log);
	          while(!feof(log) && !count){
                    fscanf(log, "%s\n", str);
 	            strcpy(buf,str);
                    ptr1 = strstr(buf,";");
                    ptr2 = strstr(ptr1+1,";");
                    *ptr2 = '\0';
	            strcpy(buf,ptr1+1);
		    printf("%s %s\n",symbol->get_data().c_str(), buf);
		    ex++;
		    if (strcmp(symbol->get_data().c_str(), buf)){
		      in++;
                    } 
                  }
                  if (ex==in) 
                    fprintf(log, "%s;%s;InProgress;Red;%s\n",s, symbol->get_data().c_str(),buffer); 
                }
	      }
              if (d[1][1]==1) {
                rewind(log); //восстановление указателя
	        while(!feof(log) && !count){
                  fscanf(log, "%s\n", str);
                  strcpy(mas[in],str);
 	          strcpy(buf,str);
                  ptr1 = strstr(buf,";");
                  ptr2 = strstr(ptr1+1,";");
                  *ptr2 = '\0';
	          strcpy(buf,ptr1+1);
		  printf("%s %s\n",symbol->get_data().c_str(), buf);
		  if (!strcmp(symbol->get_data().c_str(), buf)){
                    ptr2 = strstr(strstr(str,";")+1,";");
                    if (*(ptr2+1)=='I') { //от повторного прогона фото с Done
                      ptr1 = strstr(strstr(strstr(strstr(str,";")+1,";")+1,";")+1,";");
		      strcpy(buf,ptr1+1);
                      sprintf(str, "%s;%s;Done;Red;%s",s, symbol->get_data().c_str(),buf);
                      strcpy(mas[in],str);
                      strcat(mas[in],";");
		      strcat(mas[in], buffer);
                      count++;
                    }
                  }
                  in++;  
                }
                if (ex==0 && in){
		  fclose(log);
	          log = fopen("Stickers/log.csv", "w+");
                  for (int i=0; i<in; i++){                 
                    fprintf(log,"%s\n",mas[i]);
                  }    
                }
              }
              fclose(log);
	      //else
                //найти и изменить fprintf(log, "%s;%s;Green;%s \n",s, symbol->get_data().c_str(),asktime(timeinfo)); 
              break;
	   case -1:
              log = fopen("Stickers/log.csv", "a+t"); //от повоторного прогона свободных стикеров
	      while(!feof(log) && !count){
                  fscanf(log, "%s\n", str);
                  strcpy(mas[in],str);
 	          strcpy(buf,str);
                  ptr1 = strstr(buf,";");
                  ptr2 = strstr(ptr1+1,";");
                  *ptr2 = '\0';
	          strcpy(buf,ptr1+1);
                if (!strcmp(symbol->get_data().c_str(), buf))
                  count++;
	      }
              fclose(log);
              if (!count) {
                log = fopen("Stickers/log.csv", "a");
                fprintf(log, "%s;%s;Free;\n",s, symbol->get_data().c_str()); 
                fclose(log);
              }
              break;
	  }
	//}
        //fprintf(log, "%s |out-decode| %s \n",s, symbol->get_data().c_str());
        cvSaveImage(s, sub_img);
        j++;
	key=-1;
      }
      image.set_data(NULL, 0);
      cvReleaseImage(&sub_img);
      maxx=0;
      miny=0;
    }
    *s='\0';
    strcpy(s,ss);
    printf("%d %d %d %d \n",d[0][0],d[0][1],d[1][0],d[1][1]);
    for(int i =0;i<2;i++){
      d[i][0]=0;
      d[i][1]=0;
    }
  }
  // освобождаем ресурсы
  cvReleaseMemStorage(&storage);
  cvReleaseImage(&bin);
}

int main(){
  printf("Демонстрация работы:\n1-ый запуск введите source1.jpg и посмотрите в Stickers/log.csv\n2-ой запуск введите source2.jpg и посмотрите изменения в Stickers/log.csv\n");
  char base[80];
  scanf("%s",base);
  IplImage* image=0, *dst=0,*dst1=0,*dst2=0,*dst3=0,*dst4=0;
  char* filename = base;
  // получаем картинку
  image = cvLoadImage(filename, 1); //0!
  printf("[i] image: %s\n", filename);
  assert( image != 0 );
  // покажем изображение
  cvNamedWindow( "original");
  cvShowImage( "original", image );
  // картинка для хранения изображения
  dst = cvCloneImage(image);
  // желтую компоненту до максимума
  colour(dst,203,256,25,256,0,193);
  // cvNamedWindow( "dst", 1 );
  //cvShowImage("dst", dst);
  dst1 = cvCloneImage(image);
  colour(dst1,0,80,232,256,0,38);
  dst2 = cvCloneImage(image);
  colour(dst2,0,51,102,143,0,38);
  raschet(dst1,dst2);
  //dst3 = cvCloneImage(image);
  //colour(dst3,240,255,0,19,0,46);
  dst4 = cvCloneImage(image);
  colour(dst4,65,178,0,19,0,46);
  cut_counter(dst,image); 
  // ждём нажатия клавиши
  cvWaitKey(0);
  // освобождаем ресурсы
  cvReleaseImage(&dst);
  // удаляем окна
  cvDestroyAllWindows();
  return 0;
}


/*
	        while(!feof(log) && !count){
                  fscanf(log, "%s\n", str);

                  //if(!strcmp(str,"")) fprintf(log, "%s;%s;InProgress;Green;%s",s, symbol->get_data().c_str(),buffer);
 	          strcpy(buf,str);
                  ptr1 = strstr(buf,";");
                  ptr2 = strstr(ptr1+1,";");
                  *ptr2 = '\0';
	          strcpy(buf,ptr1+1);
		  printf("%s %s\n",symbol->get_data().c_str(), buf);
		  if (strcmp(symbol->get_data().c_str(), buf)){
		    fprintf(log, "%s;%s;InProgress;Green;%s",s, symbol->get_data().c_str(),buffer);
                    count++;
                  } else {
                    ptr1 = strstr(strstr(strstr(strstr(str,";")+1,";")+1,";")+1,";");
		    strcpy(buf,ptr1+1);
		    if (atoi(buf)>atoi(buffer)){
                      fprintf(log, "%s;%s;InProgress;Green;%s",s, symbol->get_data().c_str(),buffer);
		    }
                    count++;
                  }
                }
       */ 
