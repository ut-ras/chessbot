
#define int8 uint8_t

typedef void (*fn_callback)();
struct gc_method {
	char group;
	int code;
	fn_callback fn;
};



#define kCMD_SOURCE_SERIAL 0
#define kCMD_SOURCE_SD 1
typedef struct gc_cmd_entry {

	uint8_t source;
	uint8_t sz; //cmd size
	uint8_t csz; //comment size
	bool complete;
	char cmd[CFG_MAX_CMD_SIZE];
	char comment[CFG_MAX_CMD_SIZE];
	long n;
	uint8_t checksum;
	uint8_t readState;
	
} gc_cmd;


#define kSTATUS_NOTREADY 100
#define kSTATUS_READY 1
#define kSTATUS_MOVING 10
#define kSTATUS_PAUSSED 101
#define kSTATUS_STOPPED 102


enum mc_axis {X_AXIS=0, Y_AXIS=1, Z_AXIS=2, E_AXIS=3};
enum mc_extruder {EXT1=0, EXT2=1};
//motion control state
struct mc_state {
	float destination[4]; //position where we want to travel
	//G1/G3 The point in X space from the current X position to maintain a constant distance from
	//G1/G3 The point in Y space from the current Y position to maintain a constant distance from
	float destOffset[3];
	
	float position[4]; //current position
	float relative_modes[4]; //indicator if axis is on absolute pos or relative
	float min_pos[3];
	float max_pos[3];
	float offsets[4]; //offset applied to axis with M206
	float located[4]; //indicator to see if axis was homed and location known
	//float status[4]; //status of each stepper
	float max_feedrate[4]; //maxfeedrate for each axis
	
	float feedrate;
	float feedrate_saved;
	float feedmultiply;
	float feedmultiply_saved;
	
	float zoffset;
	
	int extrudemultiply;
	float volumetric_multiplier[2];
	
	int8 status; //job status
	bool isSDPrinting;
	
	
	int active_extruder;
	int fanSpeed;
	unsigned long lastMotionTime;
	
	unsigned long max_inactive_time;
	
	
};

