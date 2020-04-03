#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Utimer
{
public:

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// var init
	long timer_start_time_over;
	long timer_start_time_run;
	long timer_period;
	bool wr_en;
	bool latch;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// constructor
	Utimer(long _timer_period){
		timer_period = _timer_period;
		timer_start_time_over = 0;
		timer_start_time_run = 0;
		wr_en = 0;
		latch = 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// set perid, use this period if over() or run(funcName) functions launch without time parameter
	void setPeriod(long _timer_period){
		timer_period = _timer_period;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// std over and std run functions
	bool over(long _temp_timer_period = 0){

		if(_temp_timer_period > 0){
			if(millis() - timer_start_time_over >= _temp_timer_period){
				timer_start_time_over = millis();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(millis() - timer_start_time_over >= timer_period){
				timer_start_time_over = millis();
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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// with latch function. для запуска функции если millis() < timer_period, это полезно при старте когда прошло мало
	// времени и значение millis() - 0 еще меньше установленного периода, но нужно запустить функцию при первом вызове
	bool overlatch(long _temp_timer_period = 0){

		if(latch == 0){
			timer_start_time_over = millis();
			latch == 1;
			return 1;
		}
		if(_temp_timer_period > 0){
			if(millis() - timer_start_time_over >= _temp_timer_period){
				timer_start_time_over = millis();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(millis() - timer_start_time_over >= timer_period){
				timer_start_time_over = millis();
				return 1;
			}
		}
		return 0;
	}

	bool runlatch(void (&func)(), long _temp_timer_period = 0){

		if(latch == 0){
			timer_start_time_run = millis();
			latch == 1;
			func();
			return 1;
		}
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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// with reset timer
	bool overst(long _temp_timer_period = 0){

		if(_temp_timer_period > 0){
			if(millis() - timer_start_time_over >= _temp_timer_period){
				timer_start_time_over = millis();
				wr_en = 1;
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(millis() - timer_start_time_over >= timer_period){
				timer_start_time_over = millis();
				wr_en = 1;
				return 1;
			}
		}
		return 0;
	}

	bool runst(void (&func)(), long _temp_timer_period = 0){
		if(_temp_timer_period > 0){
			if(millis() - timer_start_time_run >= _temp_timer_period){
				timer_start_time_run = millis();
				wr_en = 1;
				func();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if(millis() - timer_start_time_run >= timer_period){
				timer_start_time_run = millis();
				wr_en = 1;
				func();
				return 1;
			}
		}
		return 0;
	}

	///////////////////////////////////////////////////////
	// without reset timer
	bool overwr(long _temp_timer_period = 0){  // without restart timer

		if(_temp_timer_period > 0){
			if((millis() - timer_start_time_over >= _temp_timer_period) && (wr_en == 1)){
				wr_en = 0;
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if((millis() - timer_start_time_over >= timer_period) && (wr_en == 1)){
				wr_en = 0;
				return 1;
			}
		}
		return 0;
	}

	bool runwr(void (&func)(), long _temp_timer_period = 0){  // without restart timer
		if(_temp_timer_period > 0){
			if((millis() - timer_start_time_run >= _temp_timer_period) && (wr_en == 1)){
				wr_en = 0;
				func();
				return 1;
			}
		}
		else if(_temp_timer_period == 0){
			if((millis() - timer_start_time_run >= timer_period) && (wr_en == 1)){
				wr_en = 0;
				func();
				return 1;
			}
		}
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void resetall(){
		timer_start_time_over = millis();
		timer_start_time_run = millis();
	}
	void reover(){
		timer_start_time_over = millis();
	}
	void rerun(){
		timer_start_time_run = millis();
	}

	void end(long _end_period_d = 14, long _end_period_h = 0, long _end_period_m = 0, long _end_period_s = 0){
		if(millis() >= ((_end_period_d * 8640000)+(_end_period_h * 3600000)
		+(_end_period_m * 60000)+(_end_period_s * 1000))){for(;;){}}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	~Utimer(){}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////
// example
/////////////////////////////

// // init:
// #include "Utimer.h"
// Utimer t1(1000);  // t1 init 1s
// Utimer t2(2000);  // t2 init 2s

// // function:

// t1.run(anyFunc);     // run 1s period anyFunc();
// t2.run(anyFunc2,10000);  // run 10s period anyFunc2();

// if(t1.over()){  // if 1s period over
// 	// run any action this
// }

// if(t2.over(10000)){  // if 10s period over
// 	// run any action this
// }

/////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////