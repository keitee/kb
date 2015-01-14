MHVMedia.h:
{
protected:
TOUSED: <used> by derived.
   _mhvStartSbt();
   _mhvStopSbt();
   _mhvMuteAudio();
   _mhvUnmuteAudio();
   bool _mhvIsSbtEnabled(void);
   _setWindow(&p->video);
   _mhvSetVolume(aud->audio.vol);
   _mhvGetLockStatus()
   _mhvPid2Tag();
   _mhvGetDestWinMaxResW()
   _mhvGetDestWinMaxResH()

public:
bool t_OnEvent( const PTEvent *pEvent );
bool mhvAFDOnOff(bool on_off);
TOUSED: static void mhvSetIframeStatus( bool fIframeUsed, HMhvIFrame frame );
void mhvMediaInitInternalState(uint16_t def_onid, uint16_t def_tsid, uint16_t def_sid);
void mhvMediaUpdateCurrentService(uint16_t def_onid, uint16_t def_tsid, uint16_t def_sid);
void mhvMediaSetNDT(bool ndt);
void mhvMediaResetInternalState(bool on_reboot);
void mhvMediaPrefChange(void);
void mhvMediaOnAfdChange(long afd);
void mhvMediaOnMuteKeyPress(void);
TOUSED: uint16_t mhvMediaCurService(void);
TOUSED: uint16_t mhvMediaDefService(void);
void mhvMediaPictureLock(void);
void mhvMediaPictureUnlock(void);
void mhvMediaOnVidPmtChange(uint32_t ui_pmt_data);
void mhvMediaOnAudPmtChange(uint32_t ui_pmt_data);
void mhvMediaOnAudLangChange(void);
void mhvMediaUnmuteFlagSet( bool bflag );
void mhvMediaUnmuteVideo( void );
void mhvMediaUnmuteAudio( void );
TOUSED: bool mhvMuteVideo( void );
TOUSED: void mhvMediaMuteVideo( void );
void mhvMediaMuteAudio( void );
void mhvSetVideoMute( bool fVideoMute );
void mhvSetAudioMute( bool fAudioMute );
bool mhvGetVideoMute( void );
bool mhvGetAudioMute( void );
TOUSED: void mhvSetVideoPause( bool fVideoPause );
bool mhvGetVideoPause( void );
TOUSED: void mhvSwitcherMuteVideo(bool mute);
void mhvMediaSetDigitalTextControlValue( bool ControlValue );
bool mhvMediaGetDigitalTextControlValue( void );
void mhvResetInOutVideoWindows(void);
void mhvSetFullResetFlag( bool reset_flag );
static void _mhvMediaInit(TCWindow* window);
pfmerr_t mhvMediaContinuance(void);
void mhvMediaLock(void);
void mhvMediaUnlock(void);
pfmerr_t mhvMediaDisinheritVideo(void);
pfmerr_t mhvMediaDisinheritAudio(void);
pfmerr_t mhvMediaSetAspectRatio(EMhvAspectRatio r, bool forceSet = false);
pfmerr_t mhvMediaSetWidescreenAlignment(EMhvWidescreenAlignmentMode alignment);
pfmerr_t mhvMediaMapCoordinates(int x, int y, int *ox, int *oy);
pfmerr_t mhvMediaRTGOnOff(bool on_off);
bool mhvGetAFDState(void);
bool mhvSetInOutIFrameWindows( int origin_x,  int origin_y, int offset_x,  int offset_y, int scale_x,   int scale_y, int size_x,    int size_y, int if_w,      int if_h, int plane);
void mhvMediaProcessResolutionChange(int currRes);
bool mhvMediaIPStreamGetStartup( void ); 
void ICSEvent(ICSPlaybackEvent eventid, int streamId  ); 
void suppressSubtitles(bool suppress);
static bool mhvPIPStateCheckerCb(PTEvent* param);
};

