HEADERS += \
	cops.h \
	copsScene.h \
    barrier.h
SOURCES +=	\
	main.cpp \
	cops.cpp \
	copsScene.cpp \ 
    barrier.cpp

RESOURCES += cops.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/cops
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS cops.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/cops
INSTALLS += target sources
