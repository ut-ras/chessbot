
#include "firmware.h"
#include "temperature.h"

//===========================================================================
//=============================public variables============================
//===========================================================================
int target_temperature[EXTRUDERS] = { 0 };
float hot_temperature[EXTRUDERS] = { 0 };
int target_temperature_bed = 0;
int current_temperature_raw[EXTRUDERS] = { 0 };
float current_temperature[EXTRUDERS] = { 0.0 };
int current_temperature_bed_raw = 0;
float current_temperature_bed = 0.0;


float Kp=DEFAULT_Kp;
float Ki=(DEFAULT_Ki*PID_dT);
float Kd=(DEFAULT_Kd/PID_dT);
float Kc=DEFAULT_Kc;

unsigned char soft_pwm_bed;

void temp_checkrange();
void temp_error_trigger(int err);
void temp_heaters_disable();
void temp_bed_disable();
void temp_checkrange();
void temp_checkheaters();
void temp_updatePID();



//static
static unsigned char soft_pwm[EXTRUDERS];
static volatile bool flg_temp_read = true;

static float temp_iState[EXTRUDERS] = { 0 };
static float temp_dState[EXTRUDERS] = { 0 };
static float pTerm[EXTRUDERS];
static float iTerm[EXTRUDERS];
static float dTerm[EXTRUDERS];
//int output;
static float pid_error[EXTRUDERS];
static float temp_iState_min[EXTRUDERS];
static float temp_iState_max[EXTRUDERS];
// static float pid_input[EXTRUDERS];
// static float pid_output[EXTRUDERS];
static bool pid_reset[EXTRUDERS];

static unsigned long  last_bed_heater_check;


#if EXTRUDERS > 3
# error Unsupported number of extruders
#elif EXTRUDERS > 2
# define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2, v3 }
#elif EXTRUDERS > 1
# define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2 }
#else
# define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1 }
#endif

// Init min and max temp with extreme values to prevent false errors during startup
static int minttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_RAW_LO_TEMP , HEATER_1_RAW_LO_TEMP , HEATER_2_RAW_LO_TEMP );
static int maxttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_RAW_HI_TEMP , HEATER_1_RAW_HI_TEMP , HEATER_2_RAW_HI_TEMP );
static int minttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS( 0, 0, 0 );
static int maxttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS( 16383, 16383, 16383 );


static void *heater_ttbl_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( (void *)HEATER_0_TEMPTABLE, (void *)HEATER_1_TEMPTABLE, (void *)HEATER_2_TEMPTABLE );
static uint8_t heater_ttbllen_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN, HEATER_2_TEMPTABLE_LEN );


#ifdef CFG_BED_MAXTEMP
static int bed_maxttemp_raw = HEATER_BED_RAW_HI_TEMP;
#endif


static float analog2temp(int raw, uint8_t e);
static float analog2tempBed(int raw);
static void temp_readRawValues();



void temp_updatePID(){
	
	for(int e = 0; e < EXTRUDERS; e++) {
		temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / Ki;
	}

}

void temp_error_trigger(int err){
	
	temp_heaters_disable();
	if(job.isStopped()) return;
	
	if(err == 521){
		job.triggerError(err, 'Extruder 0 Temp is beyond range.');
	}else if(err == 522){
		job.triggerError(err, 'Extruder 0 Temp is bellow range.');
	}else if(err == 522){
		job.triggerError(err, 'Extruder 1 Temp is beyond range.');
	}else if(err == 523){
		job.triggerError(err, 'Extruder 1 Temp is bellow range.');
	}else if(err == 524){
		job.triggerError(err, 'Bed Temp is out of beyond.');
	}else if(err == 526){
		job.triggerError(err, 'Extruder 0 was unable to keep temp.');
	}else if(err == 527){
		job.triggerError(err, 'Extruder 1 was unable to keep temp.');
	}else{
		job.triggerError(err, 'Unspecified tempeture error.');
	}
	job.stop();
}

void temp_init(){
	
	// Finish init of mult extruder arrays
	for(int e = 0; e < EXTRUDERS; e++) {
		// populate with the first value
		maxttemp[e] = maxttemp[0];
		temp_iState_min[e] = 0.0;
		temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / Ki;
	}
	
#if defined(HEATER_0_PIN) && (HEATER_0_PIN > -1)
	SET_OUTPUT(HEATER_0_PIN);
#endif
#if defined(HEATER_1_PIN) && (HEATER_1_PIN > -1)
	SET_OUTPUT(HEATER_1_PIN);
#endif

#if defined(HEATER_BED_PIN) && (HEATER_BED_PIN > -1)
	SET_OUTPUT(HEATER_BED_PIN);
#endif
	
#if defined(FAN_PIN) && (FAN_PIN > -1)
	SET_OUTPUT(FAN_PIN);
#endif
	
	
	
	ADCSRA = 1<<ADEN | 1<<ADSC | 1<<ADIF | 0x07;
	DIDR0 = 0; //Disable digital input registers on analogue inputs A0-7
#ifdef DIDR2
	DIDR2 = 0; //Disable digital input registers on analogue inputs A8-15
#endif
	
#if defined(TEMP_0_PIN) && (TEMP_0_PIN > -1)
#if TEMP_0_PIN < 8
	DIDR0 |= 1 << TEMP_0_PIN;
#else
	DIDR2 |= 1<<(TEMP_0_PIN - 8);
#endif
#endif
#if defined(TEMP_1_PIN) && (TEMP_1_PIN > -1)
#if TEMP_1_PIN < 8
	DIDR0 |= 1<<TEMP_1_PIN;
#else
	DIDR2 |= 1<<(TEMP_1_PIN - 8);
#endif
#endif
	
#if defined(TEMP_BED_PIN) && (TEMP_BED_PIN > -1)
#if TEMP_BED_PIN < 8
	DIDR0 |= 1<<TEMP_BED_PIN;
#else
	DIDR2 |= 1<<(TEMP_BED_PIN - 8);
#endif
#endif
	
	//Configure 8 bit timer in fast PWM
	// Use timer0 for temperature measurement
	OCR0B = 128; //pulse width (0-255)
	TIMSK0 |= (1<<OCIE0B); //1 millisecond timer, using same timer as millis()
	
	// Wait for temperature measurement to settle
	delay(250);
	
	//init values
#if defined(HEATER_0_MINTEMP) && defined(HEATER_0_MAXTEMP)
	minttemp[0] = HEATER_0_MINTEMP;
	maxttemp[0] = HEATER_0_MAXTEMP;
	
	while(analog2temp(minttemp_raw[0], 0) < HEATER_0_MINTEMP) {
		minttemp_raw[0] -= OVERSAMPLENR;
	}
	while(analog2temp(maxttemp_raw[0], 0) > HEATER_0_MAXTEMP) {
		maxttemp_raw[0] += OVERSAMPLENR;
	}
	
#if (EXTRUDERS > 1)
	minttemp[1] = HEATER_0_MINTEMP;
	maxttemp[1] = HEATER_0_MAXTEMP;
	
	while(analog2temp(minttemp_raw[1], 0) < HEATER_0_MINTEMP) {
		minttemp_raw[1] -= OVERSAMPLENR;
	}
	while(analog2temp(maxttemp_raw[1], 0) > HEATER_0_MAXTEMP) {
		maxttemp_raw[1] += OVERSAMPLENR;
	}
#endif //(EXTRUDERS > 1)
	
#endif //defined(HEATER_0_MINTEMP) && defined(HEATER_0_MAXTEMP)
	
#ifdef BED_MAXTEMP
	while(analog2tempBed(bed_maxttemp_raw) > BED_MAXTEMP) {
#if HEATER_BED_RAW_LO_TEMP < HEATER_BED_RAW_HI_TEMP
		bed_maxttemp_raw -= OVERSAMPLENR;
#else
		bed_maxttemp_raw += OVERSAMPLENR;
#endif
	}
#endif //BED_MAXTEMP
	
	
}
void temp_checkheaters(){ //manage_heater
	float pid_input;
	float pid_output;
	
	if(flg_temp_read) return;
	
	temp_readRawValues();
	
	//K1 defined in Configuration.h in the PID settings
	#define K2 (1.0-K1)
	
	for(int e = 0; e < EXTRUDERS; e++){
		pid_input = current_temperature[e];
		pid_error[e] = target_temperature[e] - pid_input;
		if(pid_error[e] > CFG_PID_FUNCTIONAL_RANGE) {
			pid_output = CFG_BANG_MAX;
			pid_reset[e] = true;
		}else if(pid_error[e] < -CFG_PID_FUNCTIONAL_RANGE || target_temperature[e] == 0) {
			pid_output = 0;
			pid_reset[e] = true;
		}else {
			if(pid_reset[e] == true) {
				temp_iState[e] = 0.0;
				pid_reset[e] = false;
			}
			pTerm[e] = Kp * pid_error[e];
			temp_iState[e] += pid_error[e];
			temp_iState[e] = constrain(temp_iState[e], temp_iState_min[e], temp_iState_max[e]);
			iTerm[e] = Ki * temp_iState[e];
			
			dTerm[e] = (Kd * (pid_input - temp_dState[e]))*K2 + (K1 * dTerm[e]);
			pid_output = constrain(pTerm[e] + iTerm[e] - dTerm[e], 0, CFG_PID_MAX);
		}
		
		temp_dState[e] = pid_input;
	
	
		// Check if temperature is within the correct range
		if((current_temperature[e] > minttemp[e]) && (current_temperature[e] < maxttemp[e])){
			soft_pwm[e] = (int)pid_output >> 1;
		}else {
			soft_pwm[e] = 0;
		}
	
#ifdef CFG_TEMP_SENSOR_DISCONNECT_CHECK
		if(current_temperature[e] > hot_temperature[e])
			hot_temperature[e] = current_temperature[e];
		if(hot_temperature[e] > target_temperature[e])
			hot_temperature[e] = target_temperature[e];
		if(hot_temperature[e] - current_temperature[e] > CFG_TEMP_SENSOR_DISCONNECT_LIMIT) {
			hot_temperature[e]=0;
			temp_error_trigger((e== EXT1)? 527 : 526 );
			job.stop();
		}
#endif
	} //end for extruders
	
	if(millis() - last_bed_heater_check < CFG_BED_CHECK_INTERVAL){
		return;
	}
	last_bed_heater_check = millis();
	
	if((current_temperature_bed > CFG_BED_MINTEMP) && (current_temperature_bed < CFG_BED_MAXTEMP)){
		if(current_temperature_bed >= target_temperature_bed){
			soft_pwm_bed = 0;
		}else{
			soft_pwm_bed = MAX_BED_POWER>>1;
		}
	}else{
		soft_pwm_bed = 0;
		WRITE(HEATER_BED_PIN, 0);
	}
}

void temp_bed_disable(){
	target_temperature_bed=0;
	soft_pwm_bed=0;
	
	WRITE(HEATER_BED_PIN, 0);
}

void temp_heaters_disable(){ //old disable_heater()

	for(int i=0;i<EXTRUDERS;i++){
		setTargetHotend(0,i);
	}
	
	setTargetBed(0);
	
#if defined(TEMP_0_PIN) && TEMP_0_PIN > -1
	target_temperature[0]=0;
	soft_pwm[0]=0;

	WRITE(HEATER_0_PIN,0);
#endif
#if defined(TEMP_BED_PIN) && TEMP_BED_PIN > -1
	target_temperature_bed=0;
	soft_pwm_bed=0;

	WRITE(HEATER_BED_PIN,0);
#endif
	
}
void temp_checkrange(){
	
	int err = 0;
	
#if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
	if(current_temperature_raw[0] <= maxttemp_raw[0]) err = 521;
	if(current_temperature_raw[0] >= minttemp_raw[0]) err = 522;
#else
	if(current_temperature_raw[0] >= maxttemp_raw[0]) err = 521;
	if(current_temperature_raw[0] <= minttemp_raw[0])  err = 522;
#endif
	
	if(err) temp_error_trigger(err);
	

#if EXTRUDERS > 1
	#if HEATER_1_RAW_LO_TEMP > HEATER_1_RAW_HI_TEMP
	if(current_temperature_raw[1] <= maxttemp_raw[1]) err = 523;
	if(current_temperature_raw[1] >= minttemp_raw[1]) err = 524;
	#else
	if(current_temperature_raw[1] >= maxttemp_raw[1]) err = 523;
	if(current_temperature_raw[1] <= minttemp_raw[1]) err = 524;
	#endif
	
	if(err) temp_error_trigger(err);
#endif
				
#if HEATER_BED_RAW_LO_TEMP > HEATER_BED_RAW_HI_TEMP
	if(current_temperature_bed_raw <= bed_maxttemp_raw) {
#else
	if(current_temperature_bed_raw >= bed_maxttemp_raw) {
#endif
		target_temperature_bed = 0;
		temp_error_trigger(524);
	}

	
}
///Interrupt to read temperatures
ISR(TIMER0_COMPB_vect){ //System timing, PWM 5 and 6)
	//these variables are only accesible from the ISR, but static, so they don't lose their value
	static unsigned char temp_count = 0;
	static unsigned char temp_state = 8;
	
	static unsigned long raw_temp_0_value = 0;
	static unsigned long raw_temp_1_value = 0;
	static unsigned long raw_temp_2_value = 0;
	static unsigned long raw_temp_bed_value = 0;
	
	static unsigned char pwm_count = (1 << CFG_TEMP_SOFT_PWM_SCALE);
	static unsigned char soft_pwm_0;
	static unsigned char soft_pwm_1;
	static unsigned char soft_pwm_b;
	
	if(pwm_count == 0){
		soft_pwm_0 = soft_pwm[0];
		
		WRITE(HEATER_0_PIN, ((soft_pwm_0 > 0) ? 1:0) );
		if(soft_pwm_0 > 0) {
			WRITE(HEATER_0_PIN,1);
		} else {
			WRITE(HEATER_0_PIN,0);
		}
		
#if EXTRUDERS > 1
		soft_pwm_1 = soft_pwm[1];
		WRITE(HEATER_1_PIN, ((soft_pwm_1 > 0) ? 1:0) );
#endif
		
		soft_pwm_b = soft_pwm_bed;
		WRITE(HEATER_BED_PIN, ((soft_pwm_b > 0) ? 1:0) );
	}
	
	
	if(soft_pwm_0 < pwm_count) WRITE(HEATER_0_PIN,0);
#if EXTRUDERS > 1
	if(soft_pwm_1 < pwm_count) WRITE(HEATER_1_PIN,0);
#endif

	if(soft_pwm_b < pwm_count) WRITE(HEATER_BED_PIN,0);

	pwm_count += (1 << CFG_TEMP_SOFT_PWM_SCALE);
	pwm_count &= 0x7f;
	
	switch(temp_state) {
		case 0: // Setup to read TEMP_0
#if TEMP_0_PIN > 7
			ADCSRB = 1<<MUX5;
#else
			ADCSRB = 0;
#endif
			ADMUX = ((1 << REFS0) | (TEMP_0_PIN & 0x07));
			ADCSRA |= 1<<ADSC; // Start conversion

			job.pollInteractions();
			temp_state = 1;
			break;
		case 1: // Measure TEMP_0
			raw_temp_0_value += ADC;
			temp_state = 2;
			break;
		case 2: // Setup to read TEMP_BED
#if TEMP_BED_PIN > 7
			ADCSRB = 1<<MUX5;
#else
			ADCSRB = 0;
#endif

			ADMUX = ((1 << REFS0) | (TEMP_BED_PIN & 0x07));
			ADCSRA |= 1<<ADSC; // Start conversion
			
			job.pollInteractions();
			temp_state = 3;
			break;
		case 3: // Measure TEMP_BED
			raw_temp_bed_value += ADC;
			
			
			temp_state = 0; //we are done
			temp_count++;
			
			break;
			
		case 8: //Startup, delay initial temp reading a tiny bit so the hardware can settle.
			temp_state = 0;
			break;
	} //switch
	
	
	
	if(temp_count >= OVERSAMPLENR){ // 8 * 16 * 1/(16000000/64/256)  = 131ms.
		
		if (flg_temp_read){
			//Only update the raw values if they have been read. Else we could be updating them during reading.

			current_temperature_raw[0] = raw_temp_0_value;
#if EXTRUDERS > 1
			current_temperature_raw[1] = raw_temp_1_value;
#endif
			
			current_temperature_bed_raw = raw_temp_bed_value;
		}
		
		flg_temp_read = false;
		temp_count = 0;
		raw_temp_0_value = 0;
		raw_temp_1_value = 0;
		raw_temp_2_value = 0;
		raw_temp_bed_value = 0;
		
		temp_checkrange();
	}
}
/* Called to get the raw values into the the actual temperatures. The raw values are created in interrupt context,
 and this function is called from normal context as it is too slow to run in interrupts and will block the stepper routine otherwise */
static void temp_readRawValues(){
	for(uint8_t e=0;e<EXTRUDERS;e++){
		current_temperature[e] = analog2temp(current_temperature_raw[e], e);
	}
	current_temperature_bed = analog2tempBed(current_temperature_bed_raw);

	//Reset the watchdog after we know we have a temperature measurement.
	//watchdog_reset();  ///TODO RESET watchdog
	
	CRITICAL_SECTION_START;
	flg_temp_read = true; //we read the values
	CRITICAL_SECTION_END;
}

#define PGM_RD_W(x)   (short)pgm_read_word(&x)
// Derived from RepRap FiveD extruder::getTemperature()
// For hot end temperature measurement.
static float analog2temp(int raw, uint8_t e) {
	if(e >= EXTRUDERS){
		serial_printf("error: Invalid extruder number [%d]\n", (int)e);
		job.kill();
	}
#ifdef HEATER_0_USES_MAX6675
	if (e == 0){
		return 0.25 * raw;
	}
#endif
	
	if(heater_ttbl_map[e] != NULL){
		float celsius = 0;
		uint8_t i;
		short (*tt)[][2] = (short (*)[][2])(heater_ttbl_map[e]);
		
		for (i=1; i<heater_ttbllen_map[e]; i++){
			if (PGM_RD_W((*tt)[i][0]) > raw){
				celsius = PGM_RD_W((*tt)[i-1][1]) +
				(raw - PGM_RD_W((*tt)[i-1][0])) *
				(float)(PGM_RD_W((*tt)[i][1]) - PGM_RD_W((*tt)[i-1][1])) /
				(float)(PGM_RD_W((*tt)[i][0]) - PGM_RD_W((*tt)[i-1][0]));
				break;
			}
		}
		
		// Overflow: Set to last value in the table
		if (i == heater_ttbllen_map[e]) celsius = PGM_RD_W((*tt)[i-1][1]);
		
		return celsius;
	}
	return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
}

// Derived from RepRap FiveD extruder::getTemperature()
// For bed temperature measurement.
static float analog2tempBed(int raw) {
	float celsius = 0;
	byte i;
	
	for (i=1; i<BEDTEMPTABLE_LEN; i++){
		if (PGM_RD_W(BEDTEMPTABLE[i][0]) > raw)
		{
			celsius  = PGM_RD_W(BEDTEMPTABLE[i-1][1]) +
			(raw - PGM_RD_W(BEDTEMPTABLE[i-1][0])) *
			(float)(PGM_RD_W(BEDTEMPTABLE[i][1]) - PGM_RD_W(BEDTEMPTABLE[i-1][1])) /
			(float)(PGM_RD_W(BEDTEMPTABLE[i][0]) - PGM_RD_W(BEDTEMPTABLE[i-1][0]));
			break;
		}
	}
	
	// Overflow: Set to last value in the table
	if (i == BEDTEMPTABLE_LEN) celsius = PGM_RD_W(BEDTEMPTABLE[i-1][1]);
	
	return celsius;
}
// Apply the scale factors to the PID values
float scalePID_i(float i){
	return i*PID_dT;
}

float unscalePID_i(float i){
	return i/PID_dT;
}

float scalePID_d(float d){
	return d/PID_dT;
}

float unscalePID_d(float d){
	return d*PID_dT;
}
