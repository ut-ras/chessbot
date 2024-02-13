#ifndef printjob_h
#define printjob_h


#define kStatusIdle 0
#define kStatusPrinting 1
#define kStatusPaused 2
#define kStatusStopped 5


#define kAXIS_COUNT 4
#define kAXIS_MOTION {'X','Y','Z'}





static gc_cmd gc_commands[CFG_BUFSIZE];


extern unsigned long lastActivityTime;
#define updateLastActivity lastActivityTime=millis();

class gcPrintJob {
	public:
	int status = 0;
	int source = 0;
	int8 cmdIdx = 0;
	int8 cmdReadIdx = -1; //last cmd in buffer read
	int8 cmdCount = 0;
	
	bool cmdLineOnSerial = false;
	bool cmdLineOnStream = false;
	char cmdLineSerial[CFG_MAX_CMD_SIZE];
	char cmdLineStream[CFG_MAX_CMD_SIZE];
	
	char sbuff[CFG_BUFSIZE][CFG_MAX_CMD_SIZE];
	
	
	
	mc_state mc;
	gcodeParser parser;
	
	int8 gc_methods_count = 0;
	gc_method* gc_methods;
	
	gcPrintJob();
	
	
	
	bool isStopped();
	void setFeedRate(float feed);
	void setFeedRateWithMultiplier(float feed, float mul);
	void restoreFeedRate();
	
	void settingsChaged();
	
	void triggerError(int code, char *msg);
	void pollInteractions();
	void pollJob();
	void pollInactivity();
	void updateInteractions();
	void kill();
	void stop();
	
	void registerGCODE(char group, int code, fn_callback fn);
	
	void run();
	void nextCommand();
	void doCommand(gc_cmd gc);
	void debugDumpCommands();
	void loadCMDReadFromSerial();
	
	void readLineFromSerial();
};


#endif
