#include <Arduino.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UXtimer
{
public:

	long timer_start_time_ok;
	long timer_start_time_run;
	long timer_period;

	UXtimer(long _timer_period){
		timer_period = _timer_period;
		timer_start_time_ok = 0;
		timer_start_time_run = 0;
	}

	void setPeriod(long _timer_period){
		timer_period = _timer_period;
	}

	bool ok(long _temp_timer_period = 0){

		if(_temp_timer_period > 0){
			if(millis() - timer_start_time_ok >= _temp_timer_period){
				timer_start_time_ok = millis();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(millis() - timer_start_time_ok >= timer_period){
				timer_start_time_ok = millis();
				return 1;
			}
		}
		return 0;
	}

	bool run(void (&func)(), long _temp_timer_period = 0){
		if(_temp_timer_period > 0){
			if(millis() - timer_start_time_run >= _temp_timer_period){
				timer_start_time_run = millis();
				func();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(millis() - timer_start_time_run >= timer_period){
				timer_start_time_run = millis();
				func();
				return 1;
			}
		}
		return 0;
	}

	void resetall(){
		timer_start_time_ok = millis();
		timer_start_time_run = millis();
	}
	void reok(){
		timer_start_time_ok = millis();
	}
	void rerun(){
		timer_start_time_run = millis();
	}

	void end(long _end_period_d = 14, long _end_period_h = 0, long _end_period_m = 0, long _end_period_s = 0){
		if(millis() >= ((_end_period_d * 8640000)+(_end_period_h * 3600000)
		+(_end_period_m * 60000)+(_end_period_s * 1000))){for(;;){}}
	}

	~UXtimer(){}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
// example
/////////////////////////////

// // init:
// UXtimer t1(1000);  // t1 init 1s
// UXtimer t2(2000);  // t2 init 2s

// // function:

// t1.run(anyFunc);     // run 1s period anyFunc();
// t1.run(anyFunc,10);  // run 10s period anyFunc();

// if(t2.ok()){  // if 2s period ok
// 	// run any action this
// }

// if(t2.ok(10)){  // if 10s period ok
// 	// run any action this
// }

// t1.end(0, 0, 1, 30);   // 1m 30s
// t1.end(0, 0, 30);      // 30m
// t1.end(0, 1, 30);      // 1h 30m
// t1.end(1, 1, 30);      // 1d 1h 30m
// t1.end(1);             // 1d

/////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////