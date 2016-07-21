all: main

main: main.o
	g++ main.o -I/usr/local/include/opencv -L/usr/lib -lzbar -L/usr/local/lib -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -o main

main.o:
	g++ main.c -I/usr/local/include/opencv  -c

clean:
	rm -rf *.o main
	rm -rf Stickers/*.jpg Stickers/log.csv
