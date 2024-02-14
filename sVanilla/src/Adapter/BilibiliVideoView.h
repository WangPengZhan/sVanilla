#pragma once
#include "BaseVideoView.h"
#include "BiliApi/BiliApi.h"

Adapter::BaseVideoView ConvertToVideoView(const BiliApi::VideoView& data);

std::vector<Adapter::BaseVideoView> ConvertToVideoListView(const BiliApi::VideoView& data);

bool checkSeason(const BiliApi::VideoView& data);


Adapter::VideoView ConvertVideoView(const BiliApi::VideoView& data);
