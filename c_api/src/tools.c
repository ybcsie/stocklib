#include"tools.h"
#include <time.h>

int get_date_by_delta(int date, int delta_days)
{
	struct tm tm_date =
		{
			.tm_year = date / 10000 - 1900,
			.tm_mon = date % 10000 / 100 - 1,
			.tm_mday = date % 100,
			.tm_hour = 0,
			.tm_min = 0,
			.tm_sec = 0,
			.tm_isdst = 0

		};

	time_t tt = mktime(&tm_date) - delta_days * 24 * 3600;
	struct tm *tm_ptr = localtime(&tt);

	return (tm_ptr->tm_year + 1900) * 10000 + (tm_ptr->tm_mon + 1) * 100 + tm_ptr->tm_mday;
}