#pragma once
#include "BaseVideoView.h"
#include "BiliApi/BiliApi.h"

bool checkSeason(const biliapi::VideoView& data);
bool checkPages(const biliapi::VideoView& data);

Adapter::Views ConvertVideoView(const biliapi::VideoView& data);
std::shared_ptr<Adapter::BaseVideoView> ConvertEpisodes(const biliapi::UgcEpisode& data);
std::shared_ptr<Adapter::BaseVideoView> ConvertPages(const biliapi::VideoPage& data);

std::shared_ptr<Adapter::BaseVideoView> ConvertSingleVideo(const biliapi::VideoView& data);
