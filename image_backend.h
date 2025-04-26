#ifndef IMAGE_BACKEND_H
#define IMAGE_BACKEND_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

#include <iostream>
#include <thread>
#include <vector>
#include <filesystem>
#include <mutex>
#include <functional>

#include "utils.h"




class AsyncDecoder
{
public:
    AsyncDecoder(AVFormatContext* ctx);
    ~AsyncDecoder();

    void start(int stream, std::function<void(AVFrame*)> callback);
    size_t frame_count(int stream);
    float fps(int stream) const;
    AVFormatContext* context();

private:
    void reader(int stream_idx, std::function<void(AVFrame*)> callback);
    AVFrame* _convert24(AVFrame* frame);

private:
    AVFormatContext* _ctx;
    std::vector<std::thread> _threads;
    int _fps;

};

class AsyncSimpleImage
{
public:
    constexpr static size_t max_frame_count = 1024;

public:
    AsyncSimpleImage(std::unique_ptr<AsyncDecoder>&& decoder, int stream_idx)
        : _decoder(std::move(decoder)), _stream_idx(stream_idx) {}
    ~AsyncSimpleImage() {
        for (auto& f : _frames)
            av_frame_free(&f);
    }

    int start();
    Vector2u size() const;
    size_t frame_count() const;
    size_t frame_count_total() const;
    const AVFrame* get_image(size_t frame) const;
    ColorRGBA get_pixel(Vector2u pos, size_t frame = 0) const;
    float fps() const;

private:
    std::unique_ptr<AsyncDecoder> _decoder;
    std::vector<AVFrame*> _frames;
    mutable std::mutex _frames_mutex;
    size_t _frame_count;
    int _stream_idx;
    size_t _frame_offset;

};


class ImageBackend
{
public:
    ImageBackend() {}

    std::unique_ptr<AsyncSimpleImage> load_async(std::filesystem::path const& path);


private:
    void _convert_to_rgba(std::vector<AVFrame*>& frames);


};

#endif // IMAGE_BACKEND_H
