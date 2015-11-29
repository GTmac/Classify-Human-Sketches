all: app build_vocab_pipeline
CFLAGS = -std=c++11 -O2 -I ~/dlib-18.18 -lfftw3f -lm -lz -lpng -lpthread

app: app.cpp
	g++ -o app $(CFLAGS) app.cpp `pkg-config gtkmm-3.0 --cflags --libs`

read_images: read_images.cpp
	g++ -o read_images read_images.cpp $(CFLAGS)
build_vocab_pipeline: build_vocab_pipeline.cpp extract_feature.cpp read_images.cpp kmeans.cpp
	g++ -o build_vocab_pipeline build_vocab_pipeline.cpp extract_feature.cpp read_images.cpp kmeans.cpp $(CFLAGS)

clean:
	rm app build_vocab_pipeline
