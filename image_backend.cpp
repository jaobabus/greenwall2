#include "image_backend.h"





int AsyncSimpleImage::start()
{
    _decoder->start(_stream_idx, [this](AVFrame* f) {
        std::lock_guard _lg(_frames_mutex);
        if (_frames.size() > max_frame_count) {
            av_frame_free(&_frames[0]);
            _frames.erase(begin(_frames));
            _frame_offset++;
        }
        _frames.push_back(f);
    });
    return 0;
}

Vector2u AsyncSimpleImage::size() const
{
    std::lock_guard _lg(_frames_mutex);
    if (_frames.empty())
        return Vector2u(1);
    else
        return Vector2u(_frames[0]->width, _frames[0]->height);
}

size_t AsyncSimpleImage::frame_count() const
{
    std::lock_guard _lg(_frames_mutex);
    return _frames.size();
}

size_t AsyncSimpleImage::frame_count_total() const
{
    return _decoder->frame_count(_stream_idx);
}

const AVFrame* AsyncSimpleImage::get_image(size_t frame) const
{
    std::lock_guard _lg(_frames_mutex);
    if (frame < _frames.size())
        return _frames[frame];
    else
        return nullptr;
}

ColorRGBA AsyncSimpleImage::get_pixel(Vector2u pos, size_t frame) const
{
    if (frame_count() == 0)
        return {};
    frame = frame % frame_count();
    auto img = get_image(frame);
    const uint32_t* pixels = (const uint32_t*)img->data[0];
    auto pixel = pixels[pos.x + size().x * pos.y];
    return ColorRGBA{uint8_t((pixel >> 0) & 0xFF), uint8_t((pixel >> 8) & 0xFF), uint8_t((pixel >> 16) & 0xFF), uint8_t((pixel >> 24) & 0xFF)};
}

float AsyncSimpleImage::fps() const
{
    return _decoder->fps(_stream_idx);
}

std::unique_ptr<AsyncSimpleImage> ImageBackend::load_async(const std::filesystem::path& path)
{
    AVDictionary* dict = nullptr;
    av_dict_set(&dict, "protocol_whitelist", "file,udp,crypto,data,rtp", 0);
    auto fmt_ctx = avformat_alloc_context();
    int r = avformat_open_input(&fmt_ctx, path.string().c_str(), nullptr, &dict);
    if (r != 0) {
        char buf[1024];
        av_strerror(r, buf, sizeof(buf));
        std::cerr << "Error open: " << buf << std::endl;
        return nullptr;
    }
    r = avformat_find_stream_info(fmt_ctx, nullptr);
    if (r < 0) {
        char buf[1024];
        av_strerror(r, buf, sizeof(buf));
        std::cerr << "Error open: " << buf << std::endl;
        return nullptr;
    }
    int idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (fmt_ctx->nb_streams == 0 or idx == -1) {
        std::cerr << "Error open: " << "In file no streams" << std::endl;
        return nullptr;
    }
    auto decoder = std::make_unique<AsyncDecoder>(fmt_ctx);
    return std::make_unique<AsyncSimpleImage>(std::move(decoder), idx);
}

void ImageBackend::_convert_to_rgba(std::vector<AVFrame*>& frames)
{
    auto first = frames[0];
    SwsContext * ctx = sws_getContext(first->width, first->height, AVPixelFormat(first->format),
                                      first->width, first->height, AV_PIX_FMT_RGBA,
                                      SWS_FAST_BILINEAR, 0, 0, 0);
    for (auto& frame : frames) {
        auto frame2 = av_frame_alloc();
        frame2->format = AV_PIX_FMT_RGBA;
        frame2->width = frame->width;
        frame2->height = frame->height;
        av_frame_get_buffer(frame2, 4);
        av_frame_make_writable(frame2);
        int ret = sws_scale(ctx, frame->data, frame->linesize, 0, frame->height, frame2->data, frame2->linesize);
        if (ret <= 0) {
            std::cout << "Warning: no lines scaled\n";
        }
        std::swap(frame, frame2);
        av_frame_free(&frame2);
    }
    sws_freeContext(ctx);
}


AsyncDecoder::AsyncDecoder(AVFormatContext* ctx)
    : _ctx(ctx) {}

AsyncDecoder::~AsyncDecoder() {
    avformat_close_input(&_ctx);
}

void AsyncDecoder::start(int stream, std::function<void (AVFrame*)> callback) {
    _threads.emplace_back(std::thread([=] { reader(stream, callback); }));
}

size_t AsyncDecoder::frame_count(int stream) {
    return _ctx->streams[stream]->duration
            * _ctx->streams[stream]->r_frame_rate.num / _ctx->streams[stream]->r_frame_rate.den
            * _ctx->streams[stream]->time_base.num / _ctx->streams[stream]->time_base.den;
}

float AsyncDecoder::fps(int stream) const {
    return 1.f * _ctx->streams[stream]->r_frame_rate.num / _ctx->streams[stream]->r_frame_rate.den;
}

AVFormatContext* AsyncDecoder::context() {
    return _ctx;
}

void AsyncDecoder::reader(int stream_idx, std::function<void (AVFrame*)> callback)
{
    auto stream = _ctx->streams[stream_idx];
    const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
    auto codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, stream->codecpar);
    int r = avcodec_open2(codec_ctx, codec, nullptr);
    if (r < 0) {
        char buf[1024];
        av_strerror(r, buf, sizeof(buf));
        std::cerr << "Error open: " << "Fail decode: " << buf << std::endl;
        exit(1);
    }
    if (not _ctx)
        return;
    int prev_ret = AVERROR_EOF;
    AVFrame* frame = nullptr;
    AVPacket* packet = av_packet_alloc();
    while (true) {
        if (prev_ret >= 0) {
            if (not frame)
                frame = av_frame_alloc();
            prev_ret = avcodec_receive_frame(codec_ctx, frame);
        }
        if (prev_ret >= 0) {
            callback(_convert24(frame));
            frame = nullptr;
        }
        if (av_read_frame(_ctx, packet) < 0)
            break;

        if ((unsigned)packet->stream_index != stream_idx)
            continue;

        if ((r = avcodec_send_packet(codec_ctx, packet)) < 0) {
            char buf[1024];
            av_strerror(r, buf, sizeof(buf));
            std::cerr << "Warning: " << "Fail decode: " << buf << std::endl;
        }
        if (not frame)
            frame = av_frame_alloc();
        prev_ret = avcodec_receive_frame(codec_ctx, frame);
        if (prev_ret >= 0) {
            callback(_convert24(frame));
            frame = nullptr;
        }
    }
    prev_ret = avcodec_send_packet(codec_ctx, nullptr);
    while (prev_ret >= 0) {
        if (not frame)
            frame = av_frame_alloc();
        prev_ret = avcodec_receive_frame(codec_ctx, frame);
        if (prev_ret >= 0) {
            callback(_convert24(frame));
            frame = nullptr;
        }
    }
    av_frame_free(&frame);
}

AVFrame* AsyncDecoder::_convert24(AVFrame* frame)
{
    SwsContext * ctx = sws_getContext(frame->width, frame->height, AVPixelFormat(frame->format),
                                      frame->width, frame->height, AV_PIX_FMT_RGBA,
                                      SWS_FAST_BILINEAR, 0, 0, 0);
    auto frame2 = av_frame_alloc();
    frame2->format = AV_PIX_FMT_RGBA;
    frame2->width = frame->width;
    frame2->height = frame->height;
    av_frame_get_buffer(frame2, 4);
    av_frame_make_writable(frame2);
    int ret = sws_scale(ctx, frame->data, frame->linesize, 0, frame->height, frame2->data, frame2->linesize);
    if (ret <= 0) {
        std::cout << "Warning: no lines scaled\n";
    }
    std::swap(frame, frame2);
    av_frame_free(&frame2);
    sws_freeContext(ctx);
    return frame;
}
