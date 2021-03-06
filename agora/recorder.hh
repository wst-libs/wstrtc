#ifndef RECORDER_HH
#define RECORDER_HH

#include "IAgoraLinuxSdkCommon.h"
#include "IAgoraRecordingEngine.h"
#include "utils.h"

#include <string>
#include <vector>

struct MixModeSettings {
    int m_height;
    int m_width;
    bool m_videoMix;
    MixModeSettings():
        m_height(0),
        m_width(0),
        m_videoMix(false)
    {};
};

class Recorder : virtual public agora::recording::IRecordingEngineEventHandler
{
public:
    Recorder();

    virtual ~Recorder();

    virtual bool createChannel(const std::string &appid, const std::string &channelKey, const std::string &name,  agora::linuxsdk::uid_t uid,
                agora::recording::RecordingConfig &config, callback b);

    virtual bool leaveChannel();

    const char * Out();

protected:
    /**
     *  Callback when an error occurred during the runtime of recording engine
     *
     *
     *  @param error        Error code
     *  @param stat_code    state code
     *
     */
    virtual void onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code);

    /**
     *  Callback when an warning occurred during the runtime of recording engine
     *
     *
     *  @param warn         warning code
     *
     */
    virtual void onWarning(int warn);
   
    /**
     *  Callback when the user hase successfully joined the specified channel
     *
     *
     *  @param channelID    channel ID 
     *  @param uid          User ID
     *
     */
    virtual void onJoinChannelSuccess(const char * channelId, uid_t uid);
   
    /**
     *  Callback when recording application successfully left the channel
     *
     *
     *  @param code        leave path code
     *
     */
    virtual void onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code);

    /**
     *  Callback when another user successfully joined the channel
     *
     *
     *  @param uid          user ID
     *  @param infos        user join information    
     *
     */
    virtual void onUserJoined(uid_t uid, agora::linuxsdk::UserJoinInfos &infos);
   
    /**
     *  Callback when a user left the channel or gone offline
     *
     *
     *  @param uid          user ID
     *  @param reason       offline reason    
     *
     */
    virtual void onUserOffline(uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason);

    /**
     *  Callback when received a audio frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to received audio frame    
     *
     */
    virtual void audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const;

    /**
     *  Callback when received a video frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to received video frame    
     *
     */
    virtual void videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const;

private:
    bool _stopped;
    std::string _channelId;
    std::vector<agora::linuxsdk::uid_t> _peers;
    std::string _logdir;
    std::string _storageDir;
    MixModeSettings _mixRes;
    agora::recording::RecordingConfig _config;
    agora::recording::IRecordingEngine *_engine;
    callback _callback;
};

static callback _callback;

class RecorderContext
{
public:
    static RecorderContext* Instance()
    {
        static RecorderContext instance;
        return &instance;
    }

    void Init(callback b)
    {
        _callback = b;
    }

    bool StartRecorder(const char *appId, const char *channelKey, const char *channelId)
    {
        Recorder * produce = new Recorder;
        agora::recording::RecordingConfig config;
        bool isOk = produce->createChannel(appId, channelKey, channelId, 1000, config, Callback);
        if (isOk != true)
        {
            delete produce;
            produce = nullptr;
            return false;
        }

        _recorder.push_back(produce);
        return true;
    }

    bool StopRecorder(const char *channelId)
    {
        return true;
    }

    static int Callback(const char *channelId, int status)
    {
        if (_callback != nullptr) {
            _callback(channelId, status);
        }
        
    }
private:
    RecorderContext(){
        _callback = nullptr;
    }
    std::vector<Recorder*> _recorder;
    // callback  _callback;
};

#endif // RECORDER_HH