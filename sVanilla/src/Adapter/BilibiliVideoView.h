#pragma once
#include "BaseVideoView.h"
#include "BiliApi/BiliApi.h"

bool checkSeason(const biliapi::VideoView& data);
bool checkPages(const biliapi::VideoView& data);

Adapter::VideoView ConvertVideoView(const biliapi::VideoView& data);
Adapter::BaseVideoView ConvertEpisodes(const biliapi::UgcEpisode& data);
Adapter::BaseVideoView ConvertPages(const biliapi::VideoPage& data);
Adapter::BaseVideoView ConvertSingleVideo(const biliapi::VideoView& data);