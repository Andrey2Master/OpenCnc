#ifndef gcode_h
#define gcode_h

#define GCSTATUS_OK 0
#define GCSTATUS_BAD_NUMBER_FORMAT 1
#define GCSTATUS_EXPECTED_COMMAND_LETTER 2
#define GCSTATUS_UNSUPPORTED_STATEMENT 3
#define GCSTATUS_FLOATING_POINT_ERROR 4
#define GCSTATUS_UNSUPPORTED_PARAM 5
#define GCSTATUS_UNSOPORTED_FEEDRATE 6
#define GCSTATUS_TABLE_SIZE_OVER_X 7 
#define GCSTATUS_TABLE_SIZE_OVER_Y 8 
#define GCSTATUS_TABLE_SIZE_OVER_Z 9 
#define GCSTATUS_CANCELED 101

#define K_FRQ 10

#define MAX_SPINDEL_TEMPERATURE 48

typedef struct {
 int32_t smoothStartF_from0[3];
 int32_t smoothStopF_to0[3];
 int32_t smoothAF[3];
 uint32_t maxFeedRate[3]; // steps/sec
 uint16_t maxSpindleTemperature;
} SM_PARAM;

extern SM_PARAM _smParam;
// 200 - full step 360/1.8
#define SM_STEPS_PER_360 3200/2
#define SM_X_STEPS_PER_360 SM_STEPS_PER_360
#define SM_Y_STEPS_PER_360 SM_STEPS_PER_360
#define SM_Z_STEPS_PER_360 SM_STEPS_PER_360
#define SM_E_STEPS_PER_360 SM_STEPS_PER_360

// 320 step/mm
#define SM_X_STEPS_PER_MM (SM_X_STEPS_PER_360/5)
#define SM_Y_STEPS_PER_MM (SM_Y_STEPS_PER_360/5)
#define SM_Z_STEPS_PER_MM (SM_Z_STEPS_PER_360/5)
#define SM_E_STEPS_PER_MM (10)

#define SM_X_MAX_FEEDRATE  2000
#define SM_Y_MAX_FEEDRATE  1700
#define SM_Z_MAX_FEEDRATE  300

#define SM_X_MAX_STEPS_PER_SEC (SM_X_MAX_FEEDRATE*SM_X_STEPS_PER_MM/60)
#define SM_Y_MAX_STEPS_PER_SEC (SM_Y_MAX_FEEDRATE*SM_Y_STEPS_PER_MM/60)
#define SM_Z_MAX_STEPS_PER_SEC (SM_Z_MAX_FEEDRATE*SM_Z_STEPS_PER_MM/60)

#define SM_MANUAL_MODE_STEPS_PER_SEC SM_STEPS_PER_360 * 2 // 640*2 -> 10mm/sec = 600mm/min


#define SM_DEFAULT_FEED_RATE 50
#define SM_DEFAULT_SEEK_RATE 1000 // in millimeters per minute

// Скорость с которой можно стартовать от 0, не боясь потери шагов (шагов/sec)
#define SM_SMOOTH_START_X (200*SM_X_STEPS_PER_MM/60) // 170 mm/min = 2.3 mm/s
#define SM_SMOOTH_START_Y (180*SM_Y_STEPS_PER_MM/60)
#define SM_SMOOTH_START_Z (70*SM_Z_STEPS_PER_MM/60)
// Скорость с которой можно тормозить в 0, не боясь потери шагов (шагов/sec)
#define SM_SMOOTH_STOP_X (210*SM_X_STEPS_PER_MM/60) // 170 mm/min = 2.3 mm/s
#define SM_SMOOTH_STOP_Y (170*SM_Y_STEPS_PER_MM/60)
#define SM_SMOOTH_STOP_Z (80*SM_Z_STEPS_PER_MM/60)

// Ускорение в mm/sec/sec
#define SM_SMOOTH_DFEED_X 60
#define SM_SMOOTH_DFEED_Y 50
#define SM_SMOOTH_DFEED_Z 20

// время на ступеньку (msec)
#define SM_SMOOTH_TFEED (50) 

#define SM_SMOOTH_COS_A 977000

#define SM_TOO_SHORT_SEGMENT_MM 0.015

// for smoth alg.
#define SM_SHORT_SEGMENT_MM 0.5

#define DEFAULT_MM_PER_ARC_SEGMENT 0.1
// Number of arc generation iterations by small angle approximation before exact arc trajectory
// correction. This parameter maybe decreased if there are issues with the accuracy of the arc
// generations. In general, the default value is more than enough for the intended CNC applications
// of grbl, and should be on the order or greater than the size of the buffer to help with the
// computational efficiency of generating arcs.
#define N_ARC_CORRECTION 25 // Integer (1-255)


#define MAX_TABLE_SIZE_X 300
#define MAX_TABLE_SIZE_Y 200
#define MAX_TABLE_SIZE_Z 180

#define GFILE_MODE_MASK_CHK  0
#define GFILE_MODE_MASK_SHOW 2
#define GFILE_MODE_MASK_EXEC 4

void cnc_gfile(char *fileName, int mode);

void gc_init(void);

unsigned char gc_execute_line(char *line);

void cnc_go_home(double rate);
void cnc_dwell(int pause);
uint8_t cnc_line(double x, double y, double z, double extruder_length, 
                 double length, double feed_rate);
void cnc_end(void);
void cnc_extruder_stop(void);
void cnc_extruder_on(void );
void cnc_extruder_t(int temperature, int isWait);
double strtod_M(const char *str, char **endptr);

#endif
