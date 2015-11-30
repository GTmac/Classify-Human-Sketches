all: app build_vocab run_svm
CFLAGS = -std=c++11 -O3 -funroll-loops -I ~/dlib-18.18 -lfftw3f -lm -lz -lpng -lpthread

app: app.cpp
	g++ -o app $(CFLAGS) app.cpp `pkg-config gtkmm-3.0 --cflags --libs`

read_images: read_images.cpp
	g++ -o read_images read_images.cpp $(CFLAGS)

build_vocab: build_vocab_pipeline.cpp extract_feature.cpp read_images.cpp kmeans.cpp
	g++ -o build_vocab build_vocab_pipeline.cpp extract_feature.cpp read_images.cpp kmeans.cpp $(CFLAGS)

run_svm: svm.cpp
	g++ -o run_svm svm.cpp $(CFLAGS)
increment_svm: increment_svm.cpp
	g++ -o increment_svm increment_svm.cpp svm.cpp $(CFLAGS)

clean:
	rm app build_vocab_pipeline
