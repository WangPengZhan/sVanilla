#pragma once
#include "BaseVideoView.h"
#include "BiliApi/BiliApi.h"

bool checkSeason(const BiliApi::VideoView& data);
bool checkPages(const BiliApi::VideoView& data);

Adapter::VideoView ConvertVideoView(const BiliApi::VideoView& data);
Adapter::BaseVideoView ConvertEpisodes(const BiliApi::UgcEpisode& data);
Adapter::BaseVideoView ConvertPages(const BiliApi::VideoPage& data);
Adapter::BaseVideoView ConvertSingleVideo(const BiliApi::VideoView& data);