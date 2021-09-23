#include "ScreenCapture.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <vector>
#include "RdpServer.h"
#include "SoftEncoder.h"
#include <QCoreApplication>
#include <QDebug>

static RdpServer* rdp_server_ = new RdpServer();

void ExtractAndConvertToRGBA(const SL::Screen_Capture::Image &img, unsigned char *dst, size_t dst_size)
{
    assert(dst_size >= static_cast<size_t>(SL::Screen_Capture::Width(img) * SL::Screen_Capture::Height(img) * sizeof(SL::Screen_Capture::ImageBGRA)));
    auto imgsrc = StartSrc(img);
    auto imgdist = dst;
    for (auto h = 0; h < Height(img); h++) {
        auto startimgsrc = imgsrc;
        for (auto w = 0; w < Width(img); w++) {
            *imgdist++ = imgsrc->R;
            *imgdist++ = imgsrc->G;
            *imgdist++ = imgsrc->B;
            *imgdist++ = 0; // alpha should be zero
            imgsrc++;
        }
        imgsrc = SL::Screen_Capture::GotoNextRow(img, startimgsrc);
    }
}

using namespace std::chrono_literals;
std::shared_ptr<SL::Screen_Capture::IScreenCaptureManager> framgrabber;
std::atomic<int> realcounter;
std::atomic<int> onNewFramecounter;
std::atomic<int> onFrameChangedcounter;

//inline std::ostream &operator<<(std::ostream &os, const SL::Screen_Capture::ImageRect &p)
//{
//    return os << "left=" << p.left << " top=" << p.top << " right=" << p.right << " bottom=" << p.bottom;
//}
inline std::ostream &operator<<(std::ostream &os, const SL::Screen_Capture::Monitor &p)
{
    return os << "Id=" << p.Id << " Index=" << p.Index << " Height=" << p.Height << " Width=" << p.Width << " OffsetX=" << p.OffsetX
              << " OffsetY=" << p.OffsetY << " Name=" << p.Name;
}

auto onNewFramestart = std::chrono::high_resolution_clock::now();

void createframegrabber()
{
    realcounter = 0;
    onNewFramecounter = 0;
    framgrabber = nullptr;
    framgrabber =
        SL::Screen_Capture::CreateCaptureConfiguration([]() {
            auto mons = SL::Screen_Capture::GetMonitors();
            std::cout << "Library is requesting the list of monitors to capture!" << std::endl;
            for (auto &m : mons) {
                std::cout << m << std::endl;
            }
            return mons;
        })
            ->onFrameChanged([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                //std::cout << "Difference detected!  " << img.Bounds << std::endl;
                // Uncomment the below code to write the image to disk for debugging
                /*
                        auto r = realcounter.fetch_add(1);
                        auto s = std::to_string(r) + std::string("MONITORDIF_") + std::string(".jpg");
                        auto size = Width(img) * Height(img) * sizeof(SL::Screen_Capture::ImageBGRA);
                         auto imgbuffer(std::make_unique<unsigned char[]>(size));
                         ExtractAndConvertToRGBA(img, imgbuffer.get(), size);
                         tje_encode_to_file(s.c_str(), Width(img), Height(img), 4, (const unsigned char*)imgbuffer.get());
                */
//        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - onNewFramestart).count() >=
//            1000) {
//            std::cout << "onFrameChanged fps" << onFrameChangedcounter << std::endl;
//            onFrameChangedcounter = 0;
//            onNewFramestart = std::chrono::high_resolution_clock::now();
//        }
//        onFrameChangedcounter += 1;
            })
            ->onNewFrame([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                // Uncomment the below code to write the image to disk for debugging

//                        auto r = realcounter.fetch_add(1);
//                        auto s = std::to_string(r) + std::string("MONITORNEW_") + std::string(".jpg");
//                        auto size = Width(img) * Height(img) * sizeof(SL::Screen_Capture::ImageBGRA);
//                        auto imgbuffer(std::make_unique<unsigned char[]>(size));
//                        ExtractAndConvertToRGBA(img, imgbuffer.get(), size);
//                        tje_encode_to_file(s.c_str(), Width(img), Height(img), 4, (const unsigned char *)imgbuffer.get());
//                        tje_encode_to_file(s.c_str(), Width(img), Height(img), 4, (const unsigned char *)imgbuffer.get());

                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - onNewFramestart).count() >=
                    1000) {
                    std::cout << "onNewFrame fps" << onNewFramecounter << std::endl;
                    onNewFramecounter = 0;
                    onNewFramestart = std::chrono::high_resolution_clock::now();
                }
                onNewFramecounter += 1;
                qDebug() << "Monitor ID:" << monitor.Id << " " << monitor.OffsetX << " " << monitor.OffsetY
                         << " " << monitor.Width << " " << monitor.Height;
        AVFrame* frame = av_frame_alloc();
        frame->width = Width(img);
        frame->height = Height(img);
        frame->format = (AVPixelFormat)AV_PIX_FMT_RGBA;
        av_frame_get_buffer(frame, 32);

        auto dst_size = Width(img) * Height(img) * sizeof(SL::Screen_Capture::ImageBGRA);
        Extract(img, frame->data[0], dst_size);
        auto soft_encoder = (SoftEncoder*)rdp_server_->soft_video_encoder_;
        soft_encoder->PushEncodeFrame(frame);
        SetEvent(soft_encoder->GetEncodeCommitEvent());
            })
            ->onMouseChanged([&](const SL::Screen_Capture::Image *img, const SL::Screen_Capture::MousePoint &mousepoint) {
                // Uncomment the below code to write the image to disk for debugging
                /*
                        auto r = realcounter.fetch_add(1);
                        auto s = std::to_string(r) + std::string(" M") + std::string(".png");
                        if (img) {
                            std::cout << "New mouse coordinates  AND NEW Image received."
                                      << " x= " << mousepoint.Position.x << " y= " << mousepoint.Position.y << std::endl;
                            lodepng::encode(s, (unsigned char *)StartSrc(*img), Width(*img), Height(*img));
                        }
                        else {
                            std::cout << "New mouse coordinates received."
                                      << " x= " << mousepoint.Position.x << " y= " << mousepoint.Position.y
                                      << " The mouse image is still the same as the last " << std::endl;
                        }
                */
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - onNewFramestart).count() >=
            1000) {
            std::cout << "onNewFrame fps" << onNewFramecounter << std::endl;
            onNewFramecounter = 0;
            onNewFramestart = std::chrono::high_resolution_clock::now();
        }
        onNewFramecounter += 1;
            })
            ->start_capturing();

    framgrabber->setFrameChangeInterval(std::chrono::milliseconds(16));
    framgrabber->setMouseChangeInterval(std::chrono::milliseconds(16));
}

void createpartialframegrabber()
{
    realcounter = 0;
    onNewFramecounter = 0;
    framgrabber = nullptr;
    framgrabber =
        SL::Screen_Capture::CreateCaptureConfiguration([]() {
            auto mons = SL::Screen_Capture::GetMonitors();
            std::cout << "Library is requesting the list of monitors to capture!" << std::endl;
            for (auto &m : mons) {
                // capture just a 512x512 square...  USERS SHOULD MAKE SURE bounds are
                // valid!!!!
                SL::Screen_Capture::OffsetX(m, SL::Screen_Capture::OffsetX(m) + 512);
                SL::Screen_Capture::OffsetY(m, SL::Screen_Capture::OffsetY(m) + 512);
                SL::Screen_Capture::Height(m, 512);
                SL::Screen_Capture::Width(m, 512);

                std::cout << m << std::endl;
            }
            return mons;
        })
            ->onFrameChanged([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                // Uncomment the below code to write the image to disk for debugging
                // std::cout << "Difference detected!  " << img.Bounds << std::endl;
                /*
                  auto r = realcounter.fetch_add(1);
                  auto s = std::to_string(r) + std::string("MONITORDIF_") + std::string(".jpg");
                  auto size = Width(img) * Height(img) * sizeof(SL::Screen_Capture::ImageBGRA);
                   auto imgbuffer(std::make_unique<unsigned char[]>(size));
                   ExtractAndConvertToRGBA(img, imgbuffer.get(), size);
                   tje_encode_to_file(s.c_str(), Width(img), Height(img), 4, (const unsigned char*)imgbuffer.get());
              */
            })
            ->onNewFrame([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Monitor &monitor) {
                // Uncomment the below code to write the image to disk for debugging

                /*
                auto r = realcounter.fetch_add(1);
                 auto s = std::to_string(r) + std::string("MONITORNEW_") + std::string(".jpg");
                 auto size = Width(img) * Height(img) * sizeof(SL::Screen_Capture::ImageBGRA);
                 assert(Height(img) == 512);
                 assert(Width(img) == 512);
                  auto imgbuffer(std::make_unique<unsigned char[]>(size));
                  ExtractAndConvertToRGBA(img, imgbuffer.get(), size);
                  tje_encode_to_file(s.c_str(), Width(img), Height(img), 4, (const unsigned char*)imgbuffer.get());
                  */

                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - onNewFramestart).count() >=
                    1000) {
                    std::cout << "onNewFrame fps" << onNewFramecounter << std::endl;
                    onNewFramecounter = 0;
                    onNewFramestart = std::chrono::high_resolution_clock::now();
                }
                onNewFramecounter += 1;
            })
            ->onMouseChanged([&](const SL::Screen_Capture::Image *img, const SL::Screen_Capture::MousePoint &mousepoint) {
                // Uncomment the below code to write the image to disk for debugging
                /*
                auto r = realcounter.fetch_add(1);
                auto s = std::to_string(r) + std::string(" M") + std::string(".png");
                if (img) {
                    std::cout << "New mouse coordinates AND NEW Image received."
                              << " x= " << mousepoint.Position.x << " y= " << mousepoint.Position.y << std::endl;
                    lodepng::encode(s, (unsigned char *)StartSrc(*img), Width(*img), Height(*img));
                }
                else {
                    std::cout << "New mouse coordinates received."
                              << " x= " << mousepoint.Position.x << " y= " << mousepoint.Position.y
                              << " The mouse image is still the same as the last" << std::endl;
                }
                */
            })
            ->start_capturing();

    framgrabber->setFrameChangeInterval(std::chrono::milliseconds(20));
    framgrabber->setMouseChangeInterval(std::chrono::milliseconds(20));
}

void createwindowgrabber()
{
    realcounter = 0;
    onNewFramecounter = 0;
    framgrabber = nullptr;
    framgrabber =
        SL::Screen_Capture::CreateCaptureConfiguration([]() {
            auto windows = SL::Screen_Capture::GetWindows();
            std::string srchterm = "blizzard";
            // convert to lower case for easier comparisons
            std::transform(srchterm.begin(), srchterm.end(), srchterm.begin(), [](char c) { return std::tolower(c, std::locale()); });
            decltype(windows) filtereditems;
            for (auto &a : windows) {
                std::string name = a.Name;
                std::transform(name.begin(), name.end(), name.begin(), [](char c) { return std::tolower(c, std::locale()); });
                if (name.find(srchterm) != std::string::npos) {
                    filtereditems.push_back(a);
                    std::cout << "ADDING WINDOW  Height " << a.Size.y << "  Width  " << a.Size.x << "   " << a.Name << std::endl;
                }
            }

            return filtereditems;
        })

            ->onNewFrame([&](const SL::Screen_Capture::Image &img, const SL::Screen_Capture::Window &window) {
                // Uncomment the below code to write the image to disk for debugging

                /*
                auto r = realcounter.fetch_add(1);
                 auto s = std::to_string(r) + std::string("WINNEW_") + std::string(".jpg");
                 auto size = Width(img) * Height(img) * sizeof(SL::Screen_Capture::ImageBGRA);
                  auto imgbuffer(std::make_unique<unsigned char[]>(size));
                  ExtractAndConvertToRGBA(img, imgbuffer.get(), size);
                  tje_encode_to_file(s.c_str(), Width(img), Height(img), 4, (const unsigned char*)imgbuffer.get());
                  */

                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - onNewFramestart).count() >=
                    1000) {
                    std::cout << "onNewFrame fps" << onNewFramecounter << std::endl;
                    onNewFramecounter = 0;
                    onNewFramestart = std::chrono::high_resolution_clock::now();
                }
                onNewFramecounter += 1;
            })
            ->onMouseChanged([&](const SL::Screen_Capture::Image *img, const SL::Screen_Capture::MousePoint &mousepoint) {
                // Uncomment the below code to write the image to disk for debugging
                /*
                auto r = realcounter.fetch_add(1);
                auto s = std::to_string(r) + std::string(" M") + std::string(".png");
                if (img) {
                    std::cout << "New mouse coordinates  AND NEW Image received."
                              << " x= " << mousepoint.Position.x << " y= " << mousepoint.Position.y << std::endl;
                    lodepng::encode(s, (unsigned char *)StartSrc(*img), Width(*img), Height(*img));
                }
                else {
                    std::cout << "New mouse coordinates received."
                              << " x= " << mousepoint.Position.x << " y= " << mousepoint.Position.y << " The mouse image is still the same as the last
                "
                              << std::endl;
                }
                */
            })
            ->start_capturing();

    framgrabber->setFrameChangeInterval(std::chrono::milliseconds(2));
    framgrabber->setMouseChangeInterval(std::chrono::milliseconds(2));
}

static int login (const char *_pcUserName, const char *_pcPassWord)
{
    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    rdp_server_->CreateRtspServer(566, login);
    rdp_server_->CreateSoftVideoChannel(1, 30, 1);
    rdp_server_->AllEncoderInit();
    rdp_server_->Start();

    std::srand(std::time(nullptr));

    auto goodmonitors = SL::Screen_Capture::GetMonitors();
    for (auto &m : goodmonitors) {
        std::cout << m << std::endl;
        assert(SL::Screen_Capture::isMonitorInsideBounds(goodmonitors, m));
    }
    auto badmonitors = SL::Screen_Capture::GetMonitors();

    for (auto m : badmonitors) {
        m.Height += 1;
        std::cout << m << std::endl;
        assert(!SL::Screen_Capture::isMonitorInsideBounds(goodmonitors, m));
    }
    for (auto m : badmonitors) {
        m.Width += 1;
        std::cout << m << std::endl;
        assert(!SL::Screen_Capture::isMonitorInsideBounds(goodmonitors, m));
    }
    std::cout << "Running display capturing for 10 seconds" << std::endl;
    createframegrabber();
    std::this_thread::sleep_for(std::chrono::seconds(100000000000000));
    std::cout << "Testing destroy" << std::endl;
    framgrabber = nullptr;

    return a.exec();
}
