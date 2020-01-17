#include "MyCapturer.h"
#include "rtc_base/thread.h"
#include <modules/desktop_capture/desktop_capture_options.h>
#include <third_party/libyuv/include/libyuv.h>

MyCapturer::MyCapturer() {

}

void MyCapturer::startCapturer() {
  auto options = webrtc::DesktopCaptureOptions::CreateDefault();
  options.set_allow_directx_capturer(true);
  capturer_ = webrtc::DesktopCapturer::CreateScreenCapturer(options);
  capturer_->Start(this);
  CaptureFrame();
}

webrtc::MediaSourceInterface::SourceState MyCapturer::state() const {
  return webrtc::MediaSourceInterface::kLive;
}

bool MyCapturer::remote() const {
  return false;
}

bool MyCapturer::is_screencast() const {
  return true;
}

absl::optional<bool> MyCapturer::needs_denoising() const {
  return false;
}

void MyCapturer::OnCaptureResult(webrtc::DesktopCapturer::Result result,
                                 std::unique_ptr<webrtc::DesktopFrame> frame) {
  if (result != webrtc::DesktopCapturer::Result::SUCCESS)
    return;

  int width = frame->size().width();
  int height = frame->size().height();

  if (!i420_buffer_.get() ||
      i420_buffer_->width() * i420_buffer_->height() < width * height) {
    i420_buffer_ = webrtc::I420Buffer::Create(width, height);
  }
  libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
                        i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                        i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                        i420_buffer_->StrideV(), 0, 0, width, height, width,
                        height, libyuv::kRotate0, libyuv::FOURCC_ARGB);

  OnFrame(webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0));
}

void MyCapturer::OnMessage(rtc::Message* msg) {
  if (msg->message_id == 0)
    CaptureFrame();
}

void MyCapturer::CaptureFrame() {
  capturer_->CaptureFrame();

  rtc::Location loc(__FUNCTION__, __FILE__);
  rtc::Thread::Current()->PostDelayed(loc, 33, this, 0);
}