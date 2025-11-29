// DateTime implementation using ESP32 system time (RTC / time())
#include "DateTime.h"
#include <time.h>
#include <sys/time.h>

void DateTime::setFromTm(const struct tm &t)
{
	second = t.tm_sec;
	minute = t.tm_min;
	hour = t.tm_hour;
	day = t.tm_mday;
	month = t.tm_mon + 1;
	year = t.tm_year + 1900;
	dayOfWeek = t.tm_wday; // 0 = Sunday
}

DateTime::DateTime()
{
	isValid = false;
	update();
}

DateTime::DateTime(uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t s)
{
	// Construct from components; normalize using mktime
	struct tm t = {0};
	t.tm_year = y - 1900;
	t.tm_mon = m - 1;
	t.tm_mday = d;
	t.tm_hour = h;
	t.tm_min = min;
	t.tm_sec = s;
	time_t tt = mktime(&t); // interprets as localtime
	if (tt != (time_t)(-1))
	{
		struct tm lt;
		localtime_r(&tt, &lt);
		setFromTm(lt);
		isValid = true;
	}
	else
	{
		// fallback to given values
		year = y;
		month = m;
		day = d;
		hour = h;
		minute = min;
		second = s;
		// compute dayOfWeek using a simple algorithm
	struct tm tt2 = {0};
	tt2.tm_year = y - 1900;
	tt2.tm_mon = m - 1;
	tt2.tm_mday = d;
	mktime(&tt2);
	dayOfWeek = tt2.tm_wday;
		isValid = false;
	}
}

DateTime::DateTime(String dateStr, String timeStr)
{
	// Accept either "YYYY-MM-DD HH:MM:SS" in dateStr, or dateStr and timeStr separately
	String datePart = dateStr;
	String timePart = timeStr;
	if (dateStr.indexOf(' ') > 0 && timeStr.length() == 0)
	{
		int sp = dateStr.indexOf(' ');
		datePart = dateStr.substring(0, sp);
		timePart = dateStr.substring(sp + 1);
	}

	int y = 0, mo = 0, d = 0, h = 0, mi = 0, s = 0;
	if (sscanf(datePart.c_str(), "%d-%d-%d", &y, &mo, &d) >= 3)
	{
		if (sscanf(timePart.c_str(), "%d:%d:%d", &h, &mi, &s) < 1)
		{
			h = mi = s = 0;
		}
		// reuse component constructor
		*this = DateTime((uint16_t)y, (uint8_t)mo, (uint8_t)d, (uint8_t)h, (uint8_t)mi, (uint8_t)s);
	}
	else
	{
		// invalid parse
		isValid = false;
		year = 1970;
		month = 1;
		day = 1;
		hour = minute = second = 0;
		dayOfWeek = 4; // Thursday
	}
}

DateTime::~DateTime()
{
}

DateTime::DateTime(uint32_t epoch)
{
	time_t tt = (time_t)epoch;
	struct tm tinfo;
	if (localtime_r(&tt, &tinfo) != nullptr)
	{
		setFromTm(tinfo);
		isValid = true;
	}
	else
	{
		isValid = false;
	}
}

bool DateTime::begin(const char *tz, const char *ntpServer, uint32_t timeoutMs)
{
	// tz is a POSIX TZ string, for example "GMT0" or "CST-8"
	if (tz && strlen(tz) > 0)
	{
		setenv("TZ", tz, 1);
		tzset();
	}
	// configure NTP
	configTzTime(tz, ntpServer);

	unsigned long start = millis();
	while (millis() - start < timeoutMs)
	{
		time_t now = time(nullptr);
		if (now > 1600000000UL) // sometime in 2020 — indicates time is set
		{
			update();
			return true;
		}
		delay(200);
	}
	// final attempt
	update();
	return isValid;
}

void DateTime::setTimezone(const char *tz)
{
	if (tz && strlen(tz) > 0)
	{
		setenv("TZ", tz, 1);
		tzset();
		update();
	}
}

DateTime DateTime::now()
{
	DateTime d;
	d.update();
	return d;
}

void DateTime::update()
{
	time_t now = time(nullptr);
	if (now == (time_t)(-1))
	{
		isValid = false;
		return;
	}
	struct tm tinfo;
	if (localtime_r(&now, &tinfo) == nullptr)
	{
		isValid = false;
		return;
	}
	setFromTm(tinfo);
	isValid = true;
}

bool DateTime::valid()
{
	return isValid;
}

uint8_t DateTime::getSecond()
{
	return second;
}

uint8_t DateTime::getMinute()
{
	return minute;
}

uint8_t DateTime::getHour()
{
	return hour;
}

uint8_t DateTime::getDay()
{
	return day;
}

uint8_t DateTime::getMonth()
{
	return month;
}

uint16_t DateTime::getYear()
{
	return year;
}

uint8_t DateTime::getDayOfWeek()
{
	return dayOfWeek;
}

String DateTime::toString()
{
	char buf[32];
	snprintf(buf, sizeof(buf), "%04u-%02u-%02u %02u:%02u:%02u", (unsigned)year, (unsigned)month, (unsigned)day, (unsigned)hour, (unsigned)minute, (unsigned)second);
	return String(buf);
}

uint32_t DateTime::getTimestamp()
{
	// Return current epoch seconds. If this DateTime was populated from system time, return that.
	time_t now = time(nullptr);
	if (now == (time_t)(-1))
	{
		// If unable to get system time, attempt to compute from fields
		struct tm t = {0};
		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		t.tm_hour = hour;
		t.tm_min = minute;
		t.tm_sec = second;
		time_t tt = mktime(&t);
		if (tt == (time_t)(-1))
			return 0;
		return (uint32_t)tt;
	}
	return (uint32_t)now;
}

String DateTime::convertToDateTime(uint32_t timestamp)
{
	time_t tt = (time_t)timestamp;
	struct tm tinfo;
	if (localtime_r(&tt, &tinfo) == nullptr)
		return String("");
	char buf[32];
	snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", tinfo.tm_year + 1900, tinfo.tm_mon + 1, tinfo.tm_mday, tinfo.tm_hour, tinfo.tm_min, tinfo.tm_sec);
	return String(buf);
}

uint32_t DateTime::convertToTimestamp(String dateStr, String timeStr)
{
	// parse dateStr "YYYY-MM-DD" and timeStr "HH:MM:SS" or combined
	String datePart = dateStr;
	String timePart = timeStr;
	if (dateStr.indexOf(' ') > 0 && timeStr.length() == 0)
	{
		int sp = dateStr.indexOf(' ');
		datePart = dateStr.substring(0, sp);
		timePart = dateStr.substring(sp + 1);
	}
	int y = 0, mo = 0, d = 0, h = 0, mi = 0, s = 0;
	if (sscanf(datePart.c_str(), "%d-%d-%d", &y, &mo, &d) < 3)
		return 0;
	if (sscanf(timePart.c_str(), "%d:%d:%d", &h, &mi, &s) < 1)
	{
		h = mi = s = 0;
	}
	struct tm t = {0};
	t.tm_year = y - 1900;
	t.tm_mon = mo - 1;
	t.tm_mday = d;
	t.tm_hour = h;
	t.tm_min = mi;
	t.tm_sec = s;
	time_t tt = mktime(&t);
	if (tt == (time_t)(-1))
		return 0;
	return (uint32_t)tt;
}

String DateTime::getDateString()
{
	char buf[16];
	snprintf(buf, sizeof(buf), "%04u-%02u-%02u", (unsigned)year, (unsigned)month, (unsigned)day);
	return String(buf);
}

String DateTime::getTimeString()
{
	char buf[16];
	snprintf(buf, sizeof(buf), "%02u:%02u:%02u", (unsigned)hour, (unsigned)minute, (unsigned)second);
	return String(buf);
}
