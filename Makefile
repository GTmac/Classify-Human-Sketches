all: app read_images
CFLAGS = -O2

app: app.cpp
	g++ -std=c++11 -o app $(CFLAGS) app.cpp `pkg-config gtkmm-3.0 --cflags --libs`

read_images: read_images.cpp
	g++ -std=c++11 -o read_images read_images.cpp -lz -lpng -lpthread -I ~/dlib-18.18/ $(CFLAGS)

clean:
	rm app read_images
