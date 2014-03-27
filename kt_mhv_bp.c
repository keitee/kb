class CMHVBroadcastStream : public CMHVMediaPlayer
{
public:
   
   pfmerr_t mhvMediaSetVideo(SMhvMedia *p);
   pfmerr_t mhvMediaSetAudio(SMhvMedia *p);
   pfmerr_t mhvMediaSetWindow(SMhvMedia *p);
   
   
   
   
   
   
   
   pfmerr_t play(SMhvMedia *p);
   pfmerr_t stop(SMhvMedia *p);
   pfmerr_t pause(SMhvMedia *p);
   pfmerr_t resume(SMhvMedia *p);
   
   bool getVideoShape(TCRect &src_win, TCRect &dest_win);
TOINTERNAL:   bool _mhvGetVideoRes(int& w, int& h);
   EAspectRatio _mhvGetVideoAr(void);
   bool _mhvSetSize(int size);
   bool _mhvGetSize(int& size);
   bool _mhvIsVideoPresented(void);
TOINTERNAL:   bool _mhvMuteVideo(void);
   bool _mhvSetClipVideoHD(const TCRect& src_win, const TCRect& dest_win, int plane);
TOINTERNAL:   bool _mhvStartVideo(uint16_t sid, int tag, bool is_shutdown);
   bool _mhvIsVideoMuted(bool& is_muted);
TOINTERNAL:   bool _mhvStartAudio(uint16_t sid, int tag);
TOINTERNAL:   bool _mhvUnmuteVideo( bool is_shutdown, bool force = false);
   bool _mhvIsVideoPaused(bool& is_paused);
TOINTERNAL:   bool _mhvPauseVideo(void);
TOINTERNAL:   bool _mhvResumeVideo(void);
   
   void mhvMediaOnVidPmtChange(uint32_t ui_pmt_data);
   void mhvMediaOnAudPmtChange(uint32_t ui_pmt_data);
   void mhvMediaOnAudLangChange(void);
   
   
   
   
   bool mhvAFDOnOff(bool on_off);
   bool mhegControlScaling(void);
   
TODO: added from here
TOINTERNAL:   bool _mhvAudioTag2Audio(uint16_t sid, int tag, SAudCmpInfo& acmp_info);
TOINTERNAL:   bool _mhvGetDefaultAudio(const TCChannel& ch, SAudCmpInfo& acmp_info);
TOINTERNAL:   bool _mhvGetAudio(const TCChannel& ch, int tag, SAudCmpInfo& acmp_info);
TOINTERNAL:   bool _mhvVideoTag2Pid(uint16_t sid, int tag, SVidCmpInfo& vcmp_info);
   bool _mhvGetCurrentChannel(TCChannel* ch);
};

