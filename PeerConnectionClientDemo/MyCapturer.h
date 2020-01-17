#pragma once
/**
 *¡¡WindowsÆÁÄ»Â¼ÏñÄ£¿é
 */

#include <api/scoped_refptr.h>
#include <api/video/i420_buffer.h>
#include <modules/desktop_capture/desktop_capturer.h>
#include <modules/desktop_capture/desktop_frame.h>
#include "rtc_base/thread.h"
#include "media/base/adapted_video_track_source.h"
#include "rtc_base/message_handler.h"


class MyCapturer : public rtc::AdaptedVideoTrackSource,
                   public rtc::MessageHandler,
                   public webrtc::DesktopCapturer::Callback {
 public:
  MyCapturer();

  void startCapturer();

  void CaptureFrame();

  bool is_screencast() const override;

  absl::optional<bool> needs_denoising() const override;

  webrtc::MediaSourceInterface::SourceState state() const override;

  bool remote() const override;

  void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                               std::unique_ptr<webrtc::DesktopFrame> frame) override;
  void OnMessage(rtc::Message* msg) override;

 private:
  std::unique_ptr<webrtc::DesktopCapturer> capturer_;
  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  //mutable volatile int ref_count_;
};
