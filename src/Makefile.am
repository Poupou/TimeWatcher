bin_PROGRAMS = clockreader

clockreader_LDADD = $(OPENCV_LIBS)
clockreader_CPPFLAGS = $(OPENCV_CFLAGS)
clockreader_CXXLAGS = $(OPENCV_CFLAGS) `pkg-config --cflags opencv`
clockreader_SOURCES = main.cc timereader.cc clockdetect.cc mask.cc sauvola.cc


#timeWatcher_LDFLAGS = `pkg-config --libs opencv`
#timeWatcher_CXXFLAGS = `pkg-config --cflags opencv`

