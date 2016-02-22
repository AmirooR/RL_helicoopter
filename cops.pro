QT += widgets
HEADERS += \
    ColumnVector.h \
	cops.h \
	copsScene.h \
    barrier.h \
    episodeelement.h \
    state.h \
    cluster.h \
    montecarlo.h \
    util.h
SOURCES +=	\
	main.cpp \
    ColumnVector.cpp \
	cops.cpp \
	copsScene.cpp \ 
    barrier.cpp \
    episodeelement.cpp \
    state.cpp \
    cluster.cpp \
    montecarlo.cpp

RESOURCES += cops.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/cops
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS cops.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/cops
INSTALLS += target sources
