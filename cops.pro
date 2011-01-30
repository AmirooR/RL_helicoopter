HEADERS += \
	cops.h \
	copsScene.h \
    barrier.h \
    episodeelement.h \
    state.h \
    cluster.h \
    montecarlo.h
SOURCES +=	\
	main.cpp \
	cops.cpp \
	copsScene.cpp \ 
    barrier.cpp \
    episodeelement.cpp \
    state.cpp \
    cluster.cpp \
    montecarlo.cpp

RESOURCES += cops.qrc

LIBS += -L/usr/lib64/octave-3.0.1/ -loctave 

INCPATH += /usr/include/octave-3.0.1/

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/cops
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS cops.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/cops
INSTALLS += target sources
