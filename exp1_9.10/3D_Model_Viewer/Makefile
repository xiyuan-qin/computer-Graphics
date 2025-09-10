modelviewer: mainModel.o ModelSet.o CameraSet.o
	g++ $(CFLAGS) -Wall -o modelviewer mainModel.o ModelSet.o CameraSet.o -I/opt/homebrew/include -L/opt/homebrew/lib -lglut -lGL -lGLU

mainModel.o: main.cpp Model.h Camera.h
	g++ $(CFLAGS) -Wall -DGL_SILENCE_DEPRECATION -o mainModel.o -c main.cpp 

ModelSet.o: Model.cpp Model.h
	g++ $(CFLAGS) -Wall -c Model.cpp -o ModelSet.o

CameraSet.o: Camera.cpp Camera.h
	g++ $(CFLAGS) -Wall -c Camera.cpp -o CameraSet.o

