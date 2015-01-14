class CMHVIPStream : public CMHVBroadcastStream
{
public:
   
   
   
   
   pfmerr_t getPosition(int *pos);
   pfmerr_t getMaxPosition(int *pos);
   pfmerr_t setPosition(int pos);
   pfmerr_t setEndPosition(int pos);
   
   
   
   pfmerr_t play(SMhvMedia *p);
   pfmerr_t stop(SMhvMedia *p);
   pfmerr_t pause(SMhvMedia *p);
   pfmerr_t resume(SMhvMedia *p);
TONOTE: NO overrides. HOW POSSIBLE
   // bool getVideoShape(TCRect &src_win, TCRect &dest_win);
   // bool _mhvGetVideoRes(int& w, int& h);
   // EAspectRatio _mhvGetVideoAr(void);
   // bool _mhvSetSize(int size);
   // bool _mhvGetSize(int& size);
   // bool _mhvIsVideoPresented(void);
   // bool _mhvMuteVideo(void);
   // bool _mhvSetClipVideoHD(const TCRect& src_win, const TCRect& dest_win, int plane);
   bool _mhvStartVideo(uint16_t sid, int tag, bool is_shutdown);
   // bool _mhvIsVideoMuted(bool& is_muted);
   bool _mhvStartAudio(uint16_t sid, int tag);
   // bool _mhvUnmuteVideo( bool is_shutdown, bool force = false);
   bool _mhvIsVideoPaused(bool& is_paused);
   bool _mhvPauseVideo(void);
   bool _mhvResumeVideo(void);

   void mhvMediaOnVidPmtChange(uint32_t ui_pmt_data) { };
   void mhvMediaOnAudPmtChange(uint32_t ui_pmt_data) { };
   void mhvMediaOnAudLangChange(void) { };




   // bool mhvAFDOnOff(bool on_off);
   // bool mhegControlScaling(void);
   
TODO: added from here
   bool isValid( void );
   pfmerr_t mhvMediaPlay(SMhvMedia *p);
   pfmerr_t mhvMediaStop(SMhvMedia *p);
   pfmerr_t mhvMediaPause(SMhvMedia *p);
   pfmerr_t mhvMediaResume(SMhvMedia *p);
   void updateBitrate(void);
   void updateAV(void);
   bool GetPlaybackParams( unsigned long &aPos, unsigned long &aBitrate, unsigned long long &aContentLength );
TOCLIENT:   pfmerr_t mhvMediaSetURL( const char *url );
   std::string url() { return m_url; }
TOCLIENT:   bool SetPrebuffer( void );
   static bool StopPrebuffer( void );
TOCLIENT:   static bool StartPrebuffer( void );
TOCLIENT:   static bool isBuffering(void);
   static void PlaybackStart();
   static void AvUpdate();	
TOCLIENT:   static bool IsPlayingObject( void ) { return (bool)s_PlayingObject; }
   HMhvStream getStreamHandle() { return m_StreamHandle; }
   void mhvMediaSetIndexData(const char *indexFile, size_t bitRate =0, size_t contentLenght =0 );
TOCLIENT:   void mhvSetHTTPHandle(HMhvDriverStream handle);
   void streamEnd();
TOCLIENT:   bool mhvMediaProcessEncyptionKey(KeyProcessingStatus &ProcessResult, FMhvFileOpenedCb cbfn, void *cbargs);
TOCLIENT:   bool mhvMediaSetEncryptionKey( const void *keyData, uint32_t dataLen);
   static bool SetDVBEngine(PCTask *pEngine);
   bool setupURLRedirection( void );
   void setStartedState( bool startState);
   void bufferingComplete();
   bool mhvMediaIPStreamGetStartup( void );
   bool IsUnderflow();
   void SetUnderflow(bool isUnderflowing);
   void playbackError( );
   int eventId() { return m_eventId; }
   ICSState state() { return m_ICSstate; }
   static CMHVIPStream* get(int streamId);
   void skipComplete();
   bool isSkipping() { return m_skipping; }
   void loaded();
   void windowReady();
};

