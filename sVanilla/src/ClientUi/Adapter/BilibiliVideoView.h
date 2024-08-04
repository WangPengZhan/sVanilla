#pragma once
#include "BaseVideoView.h"
#include "BiliApi/BiliApi.h"

bool checkSeason(const biliapi::VideoView& data);
bool checkPages(const biliapi::VideoView& data);

Adapter::Views convertVideoView(const biliapi::VideoView& data);
Adapter::Views convertVideoView(const biliapi::History& data);

std::shared_ptr<Adapter::BaseVideoView> convertEpisodes(const biliapi::UgcEpisode& data);
std::shared_ptr<Adapter::BaseVideoView> convertPages(const biliapi::VideoPage& data);
std::shared_ptr<Adapter::BaseVideoView> convertSingleVideo(const biliapi::VideoView& data);
std::shared_ptr<Adapter::BaseVideoView> convertHistory(const biliapi::HistoryInfo& data);
