class CMHVMediaPlayer : public CMHVMedia
{
public:
   MEDIAPLAYERTYPE getPlayerType( void ) { return m_MediaType; }
TOCLIENT:   virtual pfmerr_t mhvMediaSetVideo(SMhvMedia *p) { return ERR_OK; }
TOCLIENT:   virtual pfmerr_t mhvMediaSetAudio(SMhvMedia *p) { return ERR_OK; }
TOCLIENT:   virtual pfmerr_t mhvMediaSetWindow(SMhvMedia *p) { return ERR_OK; }
TOCLIENT:   virtual pfmerr_t getPosition(int *pos) { (*pos) = -1; return ERR_OK; }
TOCLIENT:   virtual pfmerr_t getMaxPosition(int *pos) { (*pos) = -1; return ERR_OK; }
TOCLIENT:   virtual pfmerr_t setPosition(int pos) { (void)pos; return ERR_OK; }
TOCLIENT:   virtual pfmerr_t setEndPosition(int pos) { (void)pos; return ERR_OK; }
   
TOCLIENT:   pfmerr_t setVolume(SMhvMedia *p);
   
TOCLIENT:   virtual pfmerr_t play(SMhvMedia *p) = 0;
TOCLIENT:   virtual pfmerr_t stop(SMhvMedia *p) = 0;
TOCLIENT:   virtual pfmerr_t pause(SMhvMedia *p) = 0;
TOCLIENT:   virtual pfmerr_t resume(SMhvMedia *p) = 0;
   
   virtual bool getVideoShape(TCRect &src_win, TCRect &dest_win) = 0;
   virtual bool _mhvGetVideoRes(int& w, int& h) = 0;
   virtual EAspectRatio _mhvGetVideoAr(void) = 0;
   virtual bool _mhvSetSize(int size) = 0;
   virtual bool _mhvGetSize(int& size) = 0;
   virtual bool _mhvIsVideoPresented(void) = 0;
   virtual bool _mhvMuteVideo(void) = 0;
   virtual bool _mhvSetClipVideoHD(const TCRect& src_win, const TCRect& dest_win, int plane) = 0;
   virtual bool _mhvStartVideo(uint16_t sid, int tag, bool is_shutdown) = 0;
   virtual bool _mhvIsVideoMuted(bool& is_muted) = 0;
   virtual bool _mhvStartAudio(uint16_t sid, int tag) = 0;
   virtual bool _mhvUnmuteVideo( bool is_shutdown, bool force = false) = 0;
   virtual bool _mhvIsVideoPaused(bool& is_paused) = 0;
   virtual bool _mhvPauseVideo(void) = 0;
   virtual bool _mhvResumeVideo(void) =0;
   
TONOTE: there is the same in the parent class
   virtual void mhvMediaOnVidPmtChange(uint32_t ui_pmt_data) = 0;
TONOTE: there is the same in the parent class
   virtual void mhvMediaOnAudPmtChange(uint32_t ui_pmt_data) = 0;
TONOTE: there is the same in the parent class
   virtual void mhvMediaOnAudLangChange(void) = 0;
   
   virtual bool mhvAFDOnOff(bool on_off) = 0;
   virtual bool mhegControlScaling(void) = 0;
};

NO duplication between CMHVMedia and this class other than above three.
