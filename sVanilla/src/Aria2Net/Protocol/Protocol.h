#pragma once
#include <vector>
#include <list>

#include <nlohmann/json.hpp>

namespace aria2net
{

// 协议基类
class Protocol
{
public:
    Protocol() = default;
    virtual ~Protocol() = default;
    virtual std::string toString() const = 0;
};

class AriaError : public Protocol
{
public:
    int code{};
    std::string message;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaError, code, message)
};

// http://aria2.github.io/manual/en/html/aria2c.html
class AriaUri : public Protocol
{
public:
    std::string status;
    std::string uri;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaUri, status, uri)
};

template <typename Result> class AriaBasicJson : public Protocol
{
public:
    std::string id;
    std::string jsonrpc;
    Result result;
    AriaError error;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump(4);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaBasicJson<Result>, id, jsonrpc, result, error)
};

using AriaAddMetalink = AriaBasicJson<std::string>;
using AriaAddTorrent = AriaBasicJson<std::string>;
using AriaAddUri = AriaBasicJson<std::string>;
using AriaChangeOption = AriaBasicJson<std::string>;
using AriaChangePosition = AriaBasicJson<std::string>;
using AriaChangeUri = AriaBasicJson<std::list<int>>;

using AriaGetFiles = AriaBasicJson<std::list<AriaUri>>;
class AriaGetFilesResult : public Protocol
{
public:
    std::string completedLength;
    std::string index;
    std::string length;
    std::string path;
    std::string selected;
    std::list<AriaUri> uris;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaGetFilesResult, completedLength, index, length, path, selected, uris)
};

class AriaGetGlobalStatResult : public Protocol
{
public:
    std::string downloadSpeed;
    std::string numActive;
    std::string numStopped;
    std::string numStoppedTotal;
    std::string numWaiting;
    std::string uploadSpeed;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaGetGlobalStatResult, downloadSpeed, numActive, numStopped, numStoppedTotal, numWaiting, uploadSpeed)
};
using AriaGetGlobalStat = AriaBasicJson<AriaGetGlobalStatResult>;

class AriaOption : public Protocol
{
public:
    std::string all_proxy;
    std::string allow_overwrite;
    std::string allow_piece_length_change;
    std::string always_resume;
    std::string async_dns;
    std::string auto_file_renaming;
    std::string bt_enable_hook_after_hash_check;
    std::string bt_enable_lpd;
    std::string bt_force_encryption;
    std::string bt_hash_check_seed;
    std::string bt_max_open_files;
    std::string bt_max_peers;
    std::string bt_metadata_only;
    std::string bt_min_crypto_level;
    std::string bt_remove_unselected_file;
    std::string bt_request_peer_speed_limit;
    std::string bt_require_crypto;
    std::string bt_save_metadata;
    std::string bt_seed_unverified;
    std::string bt_stop_timeout;
    std::string bt_tracker_connect_timeout;
    std::string bt_tracker_interval;
    std::string bt_tracker_timeout;
    std::string check_integrity;
    std::string conditional_get;
    std::string connect_timeout;
    std::string continue_;
    std::string dir;
    std::string dry_run;
    std::string enable_http_keep_alive;
    std::string enable_http_pipelining;
    std::string enable_mmap;
    std::string enable_peer_exchange;
    std::string file_allocation;
    std::string follow_metalink;
    std::string follow_torrent;
    std::string force_save;
    std::string ftp_pasv;
    std::string ftp_reuse_connection;
    std::string ftp_type;
    std::string hash_check_only;
    std::string http_accept_gzip;
    std::string http_auth_challenge;
    std::string http_no_cache;
    std::string lowest_speed_limit;
    std::string max_connection_per_server;
    std::string max_download_limit;
    std::string max_file_not_found;
    std::string max_mmap_limit;
    std::string max_tries;
    std::string max_upload_limit;
    std::string max_resume_failure_tries;
    std::string metalink_enable_unique_protocol;
    std::string metalink_preferred_protocol;
    std::string min_split_size;
    std::string no_file_allocation_limit;
    std::string no_netrc;
    std::string out;
    std::string parameterized_uri;
    std::string pause_metadata;
    std::string piece_length;
    std::string proxy_method;
    std::string real_time_chunk_checksum;
    std::string remote_time;
    std::string remove_control_file;
    std::string reuse_uri;
    std::string rpc_save_upload_metadata;
    std::string save_not_found;
    std::string seed_ratio;
    std::string seed_time;
    std::string select_file;
    std::string split;
    std::string stream_piece_selector;
    std::string timeout;
    std::string uri_selector;
    std::string use_head;
    std::string user_agent;
    std::string allow_overwrite_set;
    std::string allow_piece_length_change_set;
    std::string always_resume_set;
    std::string async_dns_set;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    friend void to_json(nlohmann::json& json, const AriaOption& ariaOption)
    {
        json = nlohmann::json{
            {"all-proxy",                 ariaOption.all_proxy                },
            {"allow-overwrite",           ariaOption.allow_overwrite          },
            {"allow-piece-length-change", ariaOption.allow_piece_length_change},
            {"always-resume",             ariaOption.always_resume            },
        };
    }

    friend void from_json(const nlohmann::json& json, AriaOption& ariaOption)
    {
        if (json.contains("all-proxy"))
        {
            ariaOption.all_proxy = json["all-proxy"].get<std::string>();
        }
        if (json.contains("allow-overwrite"))
        {
            ariaOption.allow_overwrite = json["allow-overwrite"].get<std::string>();
        }
        if (json.contains("allow-piece-length-change"))
        {
            ariaOption.allow_piece_length_change = json["allow-piece-length-change"].get<std::string>();
        }
        if (json.contains("always-resume"))
        {
            ariaOption.always_resume = json["always-resume"].get<std::string>();
        }
        if (json.contains("async-dns"))
        {
            ariaOption.async_dns = json["async-dns"].get<std::string>();
        }
        if (json.contains("auto-file-renaming"))
        {
            ariaOption.auto_file_renaming = json["auto-file-renaming"].get<std::string>();
        }
        if (json.contains("bt-enable-hook-after-hash-check"))
        {
            ariaOption.bt_enable_hook_after_hash_check = json["bt-enable-hook-after-hash-check"].get<std::string>();
        }
        if (json.contains("bt-enable-lpd"))
        {
            ariaOption.bt_enable_lpd = json["bt-enable-lpd"].get<std::string>();
        }
        if (json.contains("bt-force-encryption"))
        {
            ariaOption.bt_force_encryption = json["bt-force-encryption"].get<std::string>();
        }
        if (json.contains("bt-hash-check-seed"))
        {
            ariaOption.bt_hash_check_seed = json["bt-hash-check-seed"].get<std::string>();
        }
        if (json.contains("bt-max-open-files"))
        {
            ariaOption.bt_max_open_files = json["bt-max-open-files"].get<std::string>();
        }
        if (json.contains("bt-max-peers"))
        {
            ariaOption.bt_max_peers = json["bt-max-peers"].get<std::string>();
        }
        if (json.contains("bt-metadata-only"))
        {
            ariaOption.bt_metadata_only = json["bt-metadata-only"].get<std::string>();
        }
        if (json.contains("bt-min-crypto-level"))
        {
            ariaOption.bt_min_crypto_level = json["bt-min-crypto-level"].get<std::string>();
        }
        if (json.contains("bt-remove-unselected-file"))
        {
            ariaOption.bt_remove_unselected_file = json["bt-remove-unselected-file"].get<std::string>();
        }
        if (json.contains("bt-request-peer-speed-limit"))
        {
            ariaOption.bt_request_peer_speed_limit = json["bt-request-peer-speed-limit"].get<std::string>();
        }
        if (json.contains("bt-require-crypto"))
        {
            ariaOption.bt_require_crypto = json["bt-require-crypto"].get<std::string>();
        }
        if (json.contains("bt-save-metadata"))
        {
            ariaOption.bt_save_metadata = json["bt-save-metadata"].get<std::string>();
        }
        if (json.contains("bt-seed-unverified"))
        {
            ariaOption.bt_seed_unverified = json["bt-seed-unverified"].get<std::string>();
        }
        if (json.contains("bt-stop-timeout"))
        {
            ariaOption.bt_stop_timeout = json["bt-stop-timeout"].get<std::string>();
        }
        if (json.contains("bt-tracker-connect-timeout"))
        {
            ariaOption.bt_tracker_connect_timeout = json["bt-tracker-connect-timeout"].get<std::string>();
        }
        if (json.contains("bt-tracker-interval"))
        {
            ariaOption.bt_tracker_interval = json["bt-tracker-interval"].get<std::string>();
        }
        if (json.contains("bt-tracker-timeout"))
        {
            ariaOption.bt_tracker_timeout = json["bt-tracker-timeout"].get<std::string>();
        }
        if (json.contains("check-integrity"))
        {
            ariaOption.check_integrity = json["check-integrity"].get<std::string>();
        }
        if (json.contains("conditional-get"))
        {
            ariaOption.conditional_get = json["conditional-get"].get<std::string>();
        }
        if (json.contains("connect-timeout"))
        {
            ariaOption.connect_timeout = json["connect-timeout"].get<std::string>();
        }
        if (json.contains("continue"))
        {
            ariaOption.continue_ = json["continue"].get<std::string>();
        }
        if (json.contains("dir"))
        {
            ariaOption.dir = json["dir"].get<std::string>();
        }
        if (json.contains("dry-run"))
        {
            ariaOption.dry_run = json["dry-run"].get<std::string>();
        }
        if (json.contains("enable-http-keep-alive"))
        {
            ariaOption.enable_http_keep_alive = json["enable-http-keep-alive"].get<std::string>();
        }
        if (json.contains("enable-http-pipelining"))
        {
            ariaOption.enable_http_pipelining = json["enable-http-pipelining"].get<std::string>();
        }
        if (json.contains("enable-mmap"))
        {
            ariaOption.enable_mmap = json["enable-mmap"].get<std::string>();
        }
        if (json.contains("enable-peer-exchange"))
        {
            ariaOption.enable_peer_exchange = json["enable-peer-exchange"].get<std::string>();
        }
        if (json.contains("file-allocation"))
        {
            ariaOption.file_allocation = json["file-allocation"].get<std::string>();
        }
        if (json.contains("follow-metalink"))
        {
            ariaOption.follow_metalink = json["follow-metalink"].get<std::string>();
        }
        if (json.contains("follow-torrent"))
        {
            ariaOption.follow_torrent = json["follow-torrent"].get<std::string>();
        }
        if (json.contains("force-save"))
        {
            ariaOption.force_save = json["force-save"].get<std::string>();
        }
        if (json.contains("ftp-pasv"))
        {
            ariaOption.ftp_pasv = json["ftp-pasv"].get<std::string>();
        }
        if (json.contains("ftp-reuse-connection"))
        {
            ariaOption.ftp_reuse_connection = json["ftp-reuse-connection"].get<std::string>();
        }
        if (json.contains("ftp-type"))
        {
            ariaOption.ftp_type = json["ftp-type"].get<std::string>();
        }
        if (json.contains("hash-check-only"))
        {
            ariaOption.hash_check_only = json["hash-check-only"].get<std::string>();
        }
        if (json.contains("http-accept-gzip"))
        {
            ariaOption.http_accept_gzip = json["http-accept-gzip"].get<std::string>();
        }
        if (json.contains("http-auth-challenge"))
        {
            ariaOption.http_auth_challenge = json["http-auth-challenge"].get<std::string>();
        }
        if (json.contains("http-no-cache"))
        {
            ariaOption.http_no_cache = json["http-no-cache"].get<std::string>();
        }
        if (json.contains("lowest-speed-limit"))
        {
            ariaOption.lowest_speed_limit = json["lowest-speed-limit"].get<std::string>();
        }
        if (json.contains("max-connection-per-server"))
        {
            ariaOption.max_connection_per_server = json["max-connection-per-server"].get<std::string>();
        }
        if (json.contains("max-download-limit"))
        {
            ariaOption.max_download_limit = json["max-download-limit"].get<std::string>();
        }
        if (json.contains("max-file-not-found"))
        {
            ariaOption.max_file_not_found = json["max-file-not-found"].get<std::string>();
        }
        if (json.contains("max-mmap-limit"))
        {
            ariaOption.max_mmap_limit = json["max-mmap-limit"].get<std::string>();
        }
        if (json.contains("max-tries"))
        {
            ariaOption.max_tries = json["max-tries"].get<std::string>();
        }
        if (json.contains("max-upload-limit"))
        {
            ariaOption.max_upload_limit = json["max-upload-limit"].get<std::string>();
        }
        if (json.contains("max-resume-failure-tries"))
        {
            ariaOption.max_resume_failure_tries = json["max-resume-failure-tries"].get<std::string>();
        }
        if (json.contains("metalink-enable-unique-protocol"))
        {
            ariaOption.metalink_enable_unique_protocol = json["metalink-enable-unique-protocol"].get<std::string>();
        }
        if (json.contains("metalink-preferred-protocol"))
        {
            ariaOption.metalink_preferred_protocol = json["metalink-preferred-protocol"].get<std::string>();
        }
        if (json.contains("min-split-size"))
        {
            ariaOption.min_split_size = json["min-split-size"].get<std::string>();
        }
        if (json.contains("no-file-allocation-limit"))
        {
            ariaOption.no_file_allocation_limit = json["no-file-allocation-limit"].get<std::string>();
        }
        if (json.contains("no-netrc"))
        {
            ariaOption.no_netrc = json["no-netrc"].get<std::string>();
        }
        if (json.contains("out"))
        {
            ariaOption.out = json["out"].get<std::string>();
        }
        if (json.contains("parameterized-uri"))
        {
            ariaOption.parameterized_uri = json["parameterized-uri"].get<std::string>();
        }
        if (json.contains("pause-metadata"))
        {
            ariaOption.pause_metadata = json["pause-metadata"].get<std::string>();
        }
        if (json.contains("piece-length"))
        {
            ariaOption.piece_length = json["piece-length"].get<std::string>();
        }
        if (json.contains("proxy-method"))
        {
            ariaOption.proxy_method = json["proxy-method"].get<std::string>();
        }
        if (json.contains("realtime-chunk-checksum"))
        {
            ariaOption.real_time_chunk_checksum = json["realtime-chunk-checksum"].get<std::string>();
        }
        if (json.contains("remote-time"))
        {
            ariaOption.remote_time = json["remote-time"].get<std::string>();
        }
        if (json.contains("remove-control-file"))
        {
            ariaOption.remove_control_file = json["remove-control-file"].get<std::string>();
        }
        if (json.contains("reuse-uri"))
        {
            ariaOption.reuse_uri = json["reuse-uri"].get<std::string>();
        }
        if (json.contains("rpc-save-upload-metadata"))
        {
            ariaOption.rpc_save_upload_metadata = json["rpc-save-upload-metadata"].get<std::string>();
        }
        if (json.contains("save-not-found"))
        {
            ariaOption.save_not_found = json["save-not-found"].get<std::string>();
        }
        if (json.contains("seed-ratio"))
        {
            ariaOption.seed_ratio = json["seed-ratio"].get<std::string>();
        }
        if (json.contains("seed-time"))
        {
            ariaOption.seed_time = json["seed-time"].get<std::string>();
        }
        if (json.contains("select-file"))
        {
            ariaOption.select_file = json["select-file"].get<std::string>();
        }
        if (json.contains("split"))
        {
            ariaOption.split = json["split"].get<std::string>();
        }
        if (json.contains("stream-piece-selector"))
        {
            ariaOption.stream_piece_selector = json["stream-piece-selector"].get<std::string>();
        }
        if (json.contains("timeout"))
        {
            ariaOption.timeout = json["timeout"].get<std::string>();
        }
        if (json.contains("uri-selector"))
        {
            ariaOption.uri_selector = json["uri-selector"].get<std::string>();
        }
        if (json.contains("use-head"))
        {
            ariaOption.use_head = json["use-head"].get<std::string>();
        }
        if (json.contains("user-agent"))
        {
            ariaOption.user_agent = json["user-agent"].get<std::string>();
        }
        if (json.contains("allow-overwrite-set"))
        {
            ariaOption.allow_overwrite_set = json["allow-overwrite-set"].get<std::string>();
        }
        if (json.contains("allow-piece-length-change-set"))
        {
            ariaOption.allow_piece_length_change_set = json["allow-piece-length-change-set"].get<std::string>();
        }
        if (json.contains("always-resume-set"))
        {
            ariaOption.always_resume_set = json["always-resume-set"].get<std::string>();
        }
        if (json.contains("async-dns-set"))
        {
            ariaOption.async_dns_set = json["async-dns-set"].get<std::string>();
        }
    }
};
using AriaGetOption = AriaBasicJson<AriaOption>;

class AriaPeer : public Protocol
{
public:
    std::string amChoking;
    std::string bitfield;
    std::string downloadSpeed;
    std::string ip;
    std::string peerChoking;
    std::string peerId;
    std::string port;
    std::string seeder;
    std::string uploadSpeed;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaPeer, amChoking, bitfield, downloadSpeed, ip, peerChoking, peerId, port, seeder, uploadSpeed)
};
using AriaGetPeers = AriaBasicJson<std::list<AriaPeer>>;

class AriaResultServer : public Protocol
{
public:
    std::string currentUri;
    std::string downloadSpeed;
    std::string uri;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaResultServer, currentUri, downloadSpeed, uri)
};

class AriaGetServersResult : public Protocol
{
public:
    std::string index;
    std::list<AriaResultServer> servers;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaGetServersResult, index, servers)
};

using AriaGetServers = AriaBasicJson<std::list<AriaGetServersResult>>;

class AriaGetSessionInfoResult : public Protocol
{
public:
    std::string sessionId;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaGetSessionInfoResult, sessionId)
};

using AriaGetSessionInfo = AriaBasicJson<AriaGetSessionInfoResult>;
using AriaPause = AriaBasicJson<std::string>;
using AriaRemove = AriaBasicJson<std::string>;
using AriaSaveSession = AriaBasicJson<std::string>;
using AriaShutdown = AriaBasicJson<std::string>;

class AriaVersionResult : public Protocol
{
public:
    std::list<std::string> enabledFeatures;
    std::string version;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaVersionResult, enabledFeatures, version)
};
using AriaVersion = AriaBasicJson<AriaVersionResult>;

using SystemListMethods = AriaBasicJson<std::list<std::string>>;
using SystemListNotifications = AriaBasicJson<std::list<std::string>>;
using SystemMulticall = AriaBasicJson<std::string>;
class SystemMulticallMathod : public Protocol
{
public:
    std::string method;
    std::list<std::string> params;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SystemMulticallMathod, method, params)
};

class AriaSendData : public Protocol
{
public:
    std::string id;
    std::string jsonrpc;
    std::string method;
    nlohmann::json params;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump(4);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaSendData, id, jsonrpc, method, params)
};

class AriaSendOption : public Protocol
{
public:
    std::string all_proxy;
    std::string out;
    std::string dir;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    friend void to_json(nlohmann::json& json, const AriaSendOption& ariaSendOption)
    {
        if (!ariaSendOption.all_proxy.empty())
        {
            json["all-proxy"] = ariaSendOption.all_proxy;
        }
        if (!ariaSendOption.out.empty())
        {
            json["out"] = ariaSendOption.out;
        }
        if (!ariaSendOption.dir.empty())
        {
            json["dir"] = ariaSendOption.dir;
        }
    }

    friend void from_json(const nlohmann::json& json, AriaSendOption& ariaSendOption)
    {
        if (json.contains("all-proxy"))
        {
            ariaSendOption.all_proxy = json["all-proxy"].get<std::string>();
        }
        if (json.contains("out"))
        {
            ariaSendOption.out = json["out"].get<std::string>();
        }
        if (json.contains("dir"))
        {
            ariaSendOption.dir = json["dir"].get<std::string>();
        }
    }
};

class AriaTellStatusResultFile : public Protocol
{
public:
    std::string completedLength;
    std::string index;
    std::string length;
    std::string path;
    std::string selected;
    std::list<AriaUri> uris;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump();
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaTellStatusResultFile, completedLength, index, length, path, selected, uris)
};

class AriaTellStatusResult : public Protocol
{
public:
    std::string bitfield;
    std::string completedLength;
    std::string connections;
    std::string dir;
    std::string downloadSpeed;
    std::string errorCode;
    std::string errorMessage;
    std::list<AriaTellStatusResultFile> files;
    std::string gid;
    std::string numPieces;
    std::string pieceLength;
    std::string status;
    std::string totalLength;
    std::string uploadLength;
    std::string uploadSpeed;

    std::string toString() const override
    {
        nlohmann::json json;
        to_json(json, *this);
        return json.dump(4);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AriaTellStatusResult, bitfield, completedLength, connections, dir, downloadSpeed, errorCode, errorMessage,
                                                files, gid, numPieces, pieceLength, status, totalLength, uploadLength, uploadSpeed)
};

using AriaTellStatusList = AriaBasicJson<std::list<AriaTellStatusResult>>;
using AriaTellStatus = AriaBasicJson<AriaTellStatusResult>;

}  // namespace aria2net
