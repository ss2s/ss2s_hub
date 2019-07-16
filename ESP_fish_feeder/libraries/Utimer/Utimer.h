////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Utimer
{
public:

	long timer_start_time_ok;
	long timer_start_time_run;
	long timer_period;

	Utimer(long _timer_period){
		timer_period = _timer_period;
		timer_start_time_ok = 0;
		timer_start_time_run = 0;
	}

	void setPeriod(long _timer_period){
		timer_period = _timer_period;
	}

	bool ok(long _temp_timer_period = 0){

		if(_temp_timer_period > 0){
			if(timer_start_time_ok + _temp_timer_period >= millis()){
				timer_start_time_ok = millis();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(timer_start_time_ok + timer_period >= millis()){
				timer_start_time_ok = millis();
				return 1;
			}
		}
	}

	bool run(void (&func)(), long _temp_timer_period = 0){
		if(_temp_timer_period > 0){
			if(timer_start_time_run + _temp_timer_period >= millis()){
				timer_start_time_run = millis();
				func();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(timer_start_time_run + timer_period >= millis()){
				timer_start_time_run = millis();
				func();
				return 1;
			}
		}
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

	void triam(long _trial_period_m = 1){
		if(millis() >= _trial_period_m * 60000){for(;;){}}
	}
	void triah(long _trial_period_h = 1, long _trial_period_m = 0){
		if(millis() >= ((_trial_period_h * 3600000)+(_trial_period_m * 60000))){for(;;){}}
	}
	void triad(long _trial_period_d = 1, long _trial_period_h = 0, long _trial_period_m = 0){
		if(millis() >= ((_trial_period_d * 8640000)+(_trial_period_h * 3600000)+(_trial_period_m * 60000))){for(;;){}}
	}

	~Utimer(){}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
// example
/////////////////////////////

// // init:
// Utimer t1(1000);  // t1 init 1s
// Utimer t2(2000);  // t2 init 2s

// // function:

// t1.run(anyFunc);     // run 1s period anyFunc();
// t1.run(anyFunc,10);  // run 10s period anyFunc();

// if(t2.ok()){  // if 2s period ok
// 	// run any action
// }

// if(t2.ok(10)){  // if 10s period ok
// 	// run any action
// }

// t1.triam(30);
// t1.triah(1, 30);
// t1.triad(1, 1, 30);
// t1.triad(1);
/////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////