#include "stock_data.h"
#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int days_range = 150;
int delta_percentage_min = 5;
int new_high_percentage_filter = 0;
float price_limit = 150;

trade_day_info_arr *new_trade_day_info_arr_ptr(const int size)
{
	trade_day_info_arr *new_ptr = malloc(sizeof(trade_day_info_arr));
	new_ptr->ptr_arr = malloc(sizeof(trade_day_info *) * size);
	new_ptr->cur_len_ptr = malloc(sizeof(int));
	new_ptr->size = size;

	*(new_ptr->cur_len_ptr) = 0;

	return new_ptr;
}

void del_trade_day_info_arr(trade_day_info_arr *trade_day_info_arr_ptr)
{
	for (int i = 0; i < *(trade_day_info_arr_ptr->cur_len_ptr) - 1; i++)
	{
		if (trade_day_info_arr_ptr->ptr_arr[i]->time_price_arr_ptr != NULL)
		{
			free(trade_day_info_arr_ptr->ptr_arr[i]->time_price_arr_ptr->cur_len_ptr);
			free(trade_day_info_arr_ptr->ptr_arr[i]->time_price_arr_ptr->highest_arr);
			free(trade_day_info_arr_ptr->ptr_arr[i]->time_price_arr_ptr->lowest_arr);
			free(trade_day_info_arr_ptr->ptr_arr[i]->time_price_arr_ptr);
		}
		free(trade_day_info_arr_ptr->ptr_arr[i]);
	}

	free(trade_day_info_arr_ptr->ptr_arr);
	free(trade_day_info_arr_ptr->cur_len_ptr);
	free(trade_day_info_arr_ptr);
}

void add_trade_day_info_new_item(trade_day_info_arr *trade_day_info_arr_ptr, int date, float vol, float first, float highest, float lowest, float last, float delta)
{
	assert(*(trade_day_info_arr_ptr->cur_len_ptr) < trade_day_info_arr_ptr->size);
	*(trade_day_info_arr_ptr->cur_len_ptr) += 1;

	trade_day_info *new_trade_day_info_ptr = malloc(sizeof(trade_day_info));

	new_trade_day_info_ptr->date = date;
	new_trade_day_info_ptr->vol = vol;
	new_trade_day_info_ptr->first = first;
	new_trade_day_info_ptr->highest = highest;
	new_trade_day_info_ptr->lowest = lowest;
	new_trade_day_info_ptr->last = last;
	new_trade_day_info_ptr->delta = delta;
	new_trade_day_info_ptr->day_trading = 0;
	new_trade_day_info_ptr->time_price_arr_ptr = NULL;

	trade_day_info_arr_ptr->ptr_arr[*(trade_day_info_arr_ptr->cur_len_ptr) - 1] = new_trade_day_info_ptr;
}

void update_trade_day_info_last_item(trade_day_info_arr *trade_day_info_arr_ptr, int date, float vol, float first, float highest, float lowest, float last, float delta)
{
	trade_day_info *last_item_ptr = trade_day_info_arr_ptr->ptr_arr[*(trade_day_info_arr_ptr->cur_len_ptr) - 1];

	// check empty
	assert(*(trade_day_info_arr_ptr->cur_len_ptr) > 0);

	// check date
	assert(date == last_item_ptr->date);

	// replace last
	last_item_ptr->vol = vol;
	last_item_ptr->first = first;
	last_item_ptr->highest = highest;
	last_item_ptr->lowest = lowest;
	last_item_ptr->last = last;
	last_item_ptr->delta = delta;
}

int find_idx_by_date(trade_day_info_arr *trade_day_info_arr_ptr, int date)
{
	for (int i = *(trade_day_info_arr_ptr->cur_len_ptr) - 1; i >= 0; i--)
	{
		if (trade_day_info_arr_ptr->ptr_arr[i]->date == date)
			return i;

		if (trade_day_info_arr_ptr->ptr_arr[i]->date < date)
			return -1;
	}
	return -1;
}

int find_idx_range_by_yyyymm(trade_day_info_arr *trade_day_info_arr_ptr, int yyyymm, int *start_idx_ptr, int *end_idx_ptr)
{
	if (trade_day_info_arr_ptr == NULL)
	{
		printf("trade_day_info_arr_ptr == NULL\n");
		return -1;
	}

	*end_idx_ptr = -1;
	*start_idx_ptr = -1;

	for (int i = *(trade_day_info_arr_ptr->cur_len_ptr) - 1; i >= 0; i--)
	{
		if (*end_idx_ptr == -1 && trade_day_info_arr_ptr->ptr_arr[i]->date / 100 == yyyymm)
			*end_idx_ptr = i;

		if (*end_idx_ptr != -1 && trade_day_info_arr_ptr->ptr_arr[i]->date / 100 < yyyymm)
		{
			*start_idx_ptr = i + 1;
			return 0;
		}
	}

	if (*end_idx_ptr != -1 && trade_day_info_arr_ptr->ptr_arr[0]->date / 100 == yyyymm)
	{
		*start_idx_ptr = 0;
		return 0;
	}

	return -1;
}

int find_highest_idx(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx, int earliest_date)
{
	int highest_idx = trade_day_info_idx;

	for (int i = trade_day_info_idx - 1; i >= 0; i--)
	{
		// is date in bound?
		if (trade_day_info_ptr_arr[i]->date >= earliest_date)
		{
			if (trade_day_info_ptr_arr[i]->highest > trade_day_info_ptr_arr[highest_idx]->highest)
				highest_idx = i;
		}
		else
			break;
	}

	return highest_idx;
}

float get_delta_percentage(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx)
{
	if (trade_day_info_idx == 0)
		return 0;

	return trade_day_info_ptr_arr[trade_day_info_idx]->delta / trade_day_info_ptr_arr[trade_day_info_idx - 1]->last * 100;
}

float get_open_delta_percentage(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx)
{
	if (trade_day_info_idx == 0)
		return 0;

	return (trade_day_info_ptr_arr[trade_day_info_idx]->first - trade_day_info_ptr_arr[trade_day_info_idx - 1]->last) / trade_day_info_ptr_arr[trade_day_info_idx - 1]->last * 100;
}

int is_limup(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx)
{
	if (trade_day_info_ptr_arr[trade_day_info_idx]->last != trade_day_info_ptr_arr[trade_day_info_idx]->highest)
		return 0; //false

	float percentage = get_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx);

	if (percentage > 9)
		return 1; //true

	if (trade_day_info_ptr_arr[trade_day_info_idx]->date < 20150601 && percentage > 6)
		return 1; //true

	return 0; //false
}

int is_red_k(trade_day_info *trade_day_info_ptr)
{
	if (trade_day_info_ptr->last > trade_day_info_ptr->first)
		return 1; //true

	return 0; //false
}

int is_new_high(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx)
{
	if (trade_day_info_idx <= 0)
		return 0; //false

	if (!is_red_k(trade_day_info_ptr_arr[trade_day_info_idx]))
		return 0; //false

	if (new_high_percentage_filter)
		if (get_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx) < delta_percentage_min)
			return 0; //false

	int earliest_date = get_date_by_delta(trade_day_info_ptr_arr[trade_day_info_idx]->date, days_range);

	int highest_idx = find_highest_idx(trade_day_info_ptr_arr, trade_day_info_idx, earliest_date);

	assert(highest_idx >= 0);

	if (highest_idx == trade_day_info_idx)
		return 1; //true

	return 0; //false
}

int is_jump(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx)
{
	// check is first?
	if (trade_day_info_idx == 0)
		return 0; //false

	if (trade_day_info_ptr_arr[trade_day_info_idx]->first > trade_day_info_ptr_arr[trade_day_info_idx - 1]->highest)
		return 1; //true

	return 0; //false
}

int has_gap(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx)
{
	// check is first?
	if (trade_day_info_idx == 0)
		return 0; //false

	if (trade_day_info_ptr_arr[trade_day_info_idx]->lowest > trade_day_info_ptr_arr[trade_day_info_idx - 1]->highest)
		return 1; //true

	return 0; //false
}

int is_attack(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx)
{
	// check is first?
	if (trade_day_info_idx <= 0)
		return 0; //false

	if (!is_jump(trade_day_info_ptr_arr, trade_day_info_idx))
		return 0; //false

	if (is_new_high(trade_day_info_ptr_arr, trade_day_info_idx - 1))
		return 1; //true

	return 0; //false
}

int is_buy_target(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx, float mppt, int rule_no)
{
	// check is first?
	if (trade_day_info_idx == 0)
		return 0; //false

	if (trade_day_info_ptr_arr[trade_day_info_idx]->date >= 20140106)
		if (!trade_day_info_ptr_arr[trade_day_info_idx]->day_trading)
			return 0; //false

	// day-1 newh + limup -> day jump -> is (high-open)/open > percentage (high > open) ?
	if (rule_no == 1)
	{
		if (!is_jump(trade_day_info_ptr_arr, trade_day_info_idx))
			return 0; //false

		if (get_open_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx) + mppt > 10)
			return 0; //false

		if (!is_limup(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		if (!is_new_high(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		return 1; //true
	}

	if (rule_no == 2)
	{
		if (trade_day_info_ptr_arr[trade_day_info_idx]->first > price_limit)
			return 0; //false

		if (trade_day_info_ptr_arr[trade_day_info_idx]->first < 5)
			return 0; //false

		if (!is_jump(trade_day_info_ptr_arr, trade_day_info_idx))
			return 0; //false

		if (get_open_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx) + mppt > 10)
			return 0; //false

		if (!is_limup(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		if (!is_new_high(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		return 1; //true
	}

	if (rule_no == 3)
	{
		if (trade_day_info_ptr_arr[trade_day_info_idx]->first > price_limit)
			return 0; //false

		if (!is_jump(trade_day_info_ptr_arr, trade_day_info_idx))
			return 0; //false

		if (!is_limup(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		if (!is_new_high(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		float yhigh = trade_day_info_ptr_arr[trade_day_info_idx - 1]->highest;
		float lowest = trade_day_info_ptr_arr[trade_day_info_idx]->lowest;
		if (lowest > yhigh)
			return 0; //false

		return 1; //true
	}

	if (rule_no == 4)
	{
		if (trade_day_info_ptr_arr[trade_day_info_idx]->first > price_limit)
			return 0; //false

		if (trade_day_info_ptr_arr[trade_day_info_idx]->first < 5)
			return 0; //false

		if (!is_jump(trade_day_info_ptr_arr, trade_day_info_idx))
			return 0; //false

		if (get_open_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx) < 2)
			return 0; //false

		if (get_open_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx) + mppt > 10)
			return 0; //false

		if (!is_limup(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		if (!is_new_high(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		return 1; //true
	}

	if (rule_no == 5)
	{
		if (trade_day_info_ptr_arr[trade_day_info_idx]->first > price_limit)
			return 0; //false

		if (trade_day_info_ptr_arr[trade_day_info_idx]->first < 5)
			return 0; //false

		if (!is_jump(trade_day_info_ptr_arr, trade_day_info_idx))
			return 0; //false

		if (get_open_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx) < 3)
			return 0; //false

		if (get_open_delta_percentage(trade_day_info_ptr_arr, trade_day_info_idx) + mppt > 10)
			return 0; //false

		if (!is_limup(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		if (!is_new_high(trade_day_info_ptr_arr, trade_day_info_idx - 1))
			return 0; //false

		return 1; //true
	}

	assert(0);
}

float get_RoI(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx, float mppt, int rule_no)
{
	float highest = trade_day_info_ptr_arr[trade_day_info_idx]->highest;
	float lowest = trade_day_info_ptr_arr[trade_day_info_idx]->lowest;
	float open = trade_day_info_ptr_arr[trade_day_info_idx]->first;
	float close = trade_day_info_ptr_arr[trade_day_info_idx]->last;
	float yhigh = trade_day_info_ptr_arr[trade_day_info_idx - 1]->highest;
	time_price_arr *time_price_arr_ptr = trade_day_info_ptr_arr[trade_day_info_idx]->time_price_arr_ptr;

	if (time_price_arr_ptr == NULL)
	{
		printf("time_price_arr_ptr == NULL\n");
		return 0;
	}

	// buy at open -> highest RoI > mppt -> RoI = mppt   else -> sell at close
	if (rule_no == 1)
	{
		if ((highest - open) / open * 100 < mppt)
			return (close - open) / open * 100;

		return mppt;
	}

	if (rule_no == 2)
	{
		if (yhigh < lowest) //must not stop lose
		{
			if ((highest - open) / open * 100 < mppt)
				return (close - open) / open * 100;

			return mppt;
		}
		else
		{
			//check lowest first or highest first
			for (int i = 0; i < *(time_price_arr_ptr->cur_len_ptr); i++)
			{
				if (yhigh < time_price_arr_ptr->lowest_arr[i])
					if ((time_price_arr_ptr->highest_arr[i] - open) / open * 100 < mppt)
					{
						// printf("%f %f\n", time_price_arr_ptr->highest_arr[i], time_price_arr_ptr->lowest_arr[i]);
						continue; //keep waiting
					}
					else
						return mppt; //price reach, stop profit
				else
					return (yhigh - open) / open * 100; //gap is filled, stop lose
			}
		}

		assert(0); //shoud not go here
	}

	if (rule_no == 3)
	{
		if (yhigh < lowest) //must not stop lose
		{
			if ((highest - open) / open * 100 < mppt)
				return (close - open) / open * 100;

			return mppt;
		}
		else
		{
			//check lowest first or highest first
			int short_mode = 0;
			float roi = 0;

			for (int i = 0; i < *(time_price_arr_ptr->cur_len_ptr); i++)
			{
				if (short_mode)
				{
					if ((yhigh - time_price_arr_ptr->lowest_arr[i]) / time_price_arr_ptr->lowest_arr[i] * 100 < 1.5)
						continue;
					else
						return roi + 1.5;
				}

				if (yhigh < time_price_arr_ptr->lowest_arr[i])
					if ((time_price_arr_ptr->highest_arr[i] - open) / open * 100 < mppt)
						continue;
					else
						return mppt;
				else
				{
					roi = (yhigh - open) / open * 100;
					short_mode = 1;
				}
			}
			return roi + (yhigh - close) / close * 100;
		}

		assert(0); //shoud not go here
	}

	if (rule_no == 4)
	{
		if ((yhigh - lowest) / lowest * 100 < mppt)
			return (yhigh - close) / close * 100;
		else
			return mppt;
	}

	if (rule_no == 5)
	{
		if ((highest - yhigh) / yhigh * 100 > 9)
			return (yhigh - highest) / highest * 100;

		if ((yhigh - lowest) / lowest * 100 < mppt)
			return (yhigh - close) / close * 100;
		else
			return mppt;
	}
	assert(0);
}

float price_normalize(float price, int round) //round = 0:round-up 1:round-down
{
	if (price < 0.01)
		return -1;

	float out_price;
	if (price < 10)
	{
		out_price = (int)(price * 100) / 100.0;
		if (round == 0 && price > out_price)
			out_price += 0.01;

		return out_price;
	}

	if (price < 50)
	{
		int tmp = price * 100;
		while (tmp % 5)
			tmp++;

		out_price = tmp / 100.0;
		if (round == 0 && price > out_price)
			out_price += 0.05;

		if (round == 1 && price < out_price)
			out_price -= 0.05;

		return out_price;
	}

	if (price < 100)
	{
		out_price = (int)(price * 10) / 10.0;
		if (round == 0 && price > out_price)
			out_price += 0.1;

		return out_price;
	}

	if (price < 500)
	{
		int tmp = price * 10;
		while (tmp % 5)
			tmp++;

		out_price = tmp / 10.0;
		if (round == 0 && price > out_price)
			out_price += 0.5;

		if (round == 1 && price < out_price)
			out_price -= 0.5;

		return out_price;
	}

	if (price < 1000)
	{
		out_price = (int)price;
		if (round == 0 && price > out_price)
			out_price += 1;

		return out_price;
	}

	int tmp = price;
	while (tmp % 5)
		tmp++;

	out_price = tmp;
	if (round == 0 && price > out_price)
		out_price += 5;

	if (round == 1 && price < out_price)
		out_price -= 5;

	return out_price;
}
