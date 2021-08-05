#pragma once

enum class TimeType {
	milliseconds = 0, seconds
};


class Duration {
public:
	Duration(unsigned int num, TimeType t);
	void set_duration(unsigned int dur, TimeType t);

	/** 
	 * @param type Duration in milliseconds or seconds(will cut off the milliseconds after the comma)
	 * @return returns the duration
	 */
	unsigned int get_duration(TimeType type = TimeType::milliseconds);

private:
	unsigned int duration; // Milliseconds
	TimeType type;
};

unsigned int Duration::get_duration(TimeType type) {
	switch (type) {
	case (TimeType::seconds): {
		return duration / 60;
	}
	case (TimeType::milliseconds): {
		return duration;
	}
	default:
		break;
	}
}







class Touch {
public:
	void set_touchBegin(unsigned long long num)	{ touchBegin = num;	}
	void set_touchEnd(unsigned long long num)	{ touchEnd = num;	}

	unsigned long long get_touchBegin()	{ return touchBegin;	}
	unsigned long long get_touchEnd()	{ return touchEnd;		}

	/**
	 * @return touchDuration in milliseconds
	 */
	unsigned int get_touchDuration() { return touchEnd - touchBegin; }
private:
	unsigned long long touchBegin;
	unsigned long long touchEnd;
};
