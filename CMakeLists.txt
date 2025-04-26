cmake_minimum_required(VERSION 3.5)

project(greenwall2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost COMPONENTS program_options REQUIRED)

find_path(LIBAVCODEC_INCLUDE_DIR REQUIRED NAMES libavcodec/avcodec.h)
find_path(LIBAVFORMAT_INCLUDE_DIR REQUIRED  NAMES libavformat/avformat.h)
find_path(LIBAVUTIL_INCLUDE_DIR REQUIRED NAMES libavutil/avutil.h)
find_path(LIBAVUTIL_INCLUDE_DIR REQUIRED NAMES libswscale/swscale.h)

option (LIBAV_STATIC "Static link libav" false)
set(AV_DEP_LIBS
    bz2 ogg vorbis theora ssh aom openjp2 opus shine speex codec2 gsm mp3lame drm dav1d va-drm xcb-xfixes z va-x11 vdpau xcb lzma va X11 m Xv Xext x264 x265 xcb-shape sndio SDL2 asound xcb-shm)

if (${LIBAV_STATIC})
	message("~~ STATIC ~~")

	find_library(LIBAVCODEC_LIBRARY  REQUIRED NAMES libavcodec.a)
	get_filename_component(LIBAVCODEC_LIBRARY ${LIBAVCODEC_LIBRARY} DIRECTORY)
	set(LIBAVCODEC_LIBRARY "${LIBAVCODEC_LIBRARY}/libavcodec.a")

	find_library(LIBAVFORMAT_LIBRARY REQUIRED NAMES libavformat.a)
	get_filename_component(LIBAVFORMAT_LIBRARY ${LIBAVFORMAT_LIBRARY} DIRECTORY)
	set(LIBAVFORMAT_LIBRARY "${LIBAVFORMAT_LIBRARY}/libavformat.a")

	find_library(LIBAVUTIL_LIBRARY   REQUIRED NAMES libavutil.a)
	get_filename_component(LIBAVUTIL_LIBRARY ${LIBAVUTIL_LIBRARY} DIRECTORY)
	set(LIBAVUTIL_LIBRARY "${LIBAVUTIL_LIBRARY}/libavutil.a")

	find_library(LIBAVDEVICE_LIBRARY  REQUIRED NAMES libavdevice.a)
	get_filename_component(LIBAVDEVICE_LIBRARY ${LIBAVDEVICE_LIBRARY} DIRECTORY)
	set(LIBAVDEVICE_LIBRARY "${LIBAVDEVICE_LIBRARY}/libavdevice.a")

	find_library(LIBAVFILTER_LIBRARY  REQUIRED NAMES libavfilter.a)
	get_filename_component(LIBAVFILTER_LIBRARY ${LIBAVFILTER_LIBRARY} DIRECTORY)
	set(LIBAVFILTER_LIBRARY "${LIBAVFILTER_LIBRARY}/libavfilter.a")

	find_library(LIBSWSCALE_LIBRARY  REQUIRED NAMES libswscale.a)
	get_filename_component(LIBSWSCALE_LIBRARY ${LIBSWSCALE_LIBRARY} DIRECTORY)
	set(LIBSWSCALE_LIBRARY "${LIBSWSCALE_LIBRARY}/libswscale.a")

	find_library(LIBSWRESAMPLE_LIBRARY  REQUIRED NAMES libswresample.a)
	get_filename_component(LIBSWRESAMPLE_LIBRARY ${LIBSWRESAMPLE_LIBRARY} DIRECTORY)
	set(LIBSWRESAMPLE_LIBRARY "${LIBSWRESAMPLE_LIBRARY}/libswresample.a")

	set(LIBAV_STATIC_EXTRA_LIBS
		${LIBAVDEVICE_LIBRARY} ${LIBAVFILTER_LIBRARY} ${LIBSWRESAMPLE_LIBRARY}
                ${AV_DEP_LIBS})
else ()
	message("~~ DYNAMIC ~~")

	find_library(LIBAVCODEC_LIBRARY  REQUIRED NAMES avcodec)
	find_library(LIBAVFORMAT_LIBRARY REQUIRED NAMES avformat)
	find_library(LIBAVUTIL_LIBRARY   REQUIRED NAMES avutil)
	find_library(LIBSWSCALE_LIBRARY  REQUIRED NAMES swscale)
endif ()

set(LIBAV_INCLUDE_DIRS
	${LIBAVCODEC_INCLUDE_DIR}
	${LIBAVFORMAT_INCLUDE_DIR}
	${LIBAVUTIL_INCLUDE_DIR}
)

set(LIBAV_LIBRARIES
	${LIBAVFORMAT_LIBRARY}
	${LIBAVCODEC_LIBRARY}
	${LIBAVUTIL_LIBRARY}
	${LIBSWSCALE_LIBRARY}
	${LIBAV_STATIC_EXTRA_LIBS}
)


set(GW_SOURCES
	map2d.h
	utils.h utils.cpp
	wall.h wall.cpp
	console.h console.cpp
	wallentity.h wallentity.cpp
	config.h
	app.h app.cpp
	image_backend.h image_backend.cpp
	profile.h profile.cpp
	)


add_executable(greenwall2 main.cpp
	           ${GW_SOURCES})
target_include_directories(greenwall2 PRIVATE ${LIBAV_INCLUDE_DIRS})
target_link_libraries(greenwall2 PRIVATE ${LIBAV_LIBRARIES} Boost::program_options)
