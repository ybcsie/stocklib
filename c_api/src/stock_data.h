#ifndef STOCK_DATA_H
#define STOCK_DATA_H

#define STOCK_ID_LEN 4
#define DATE_LEN 8
#define FORMATTED_DATE_LEN DATE_LEN + 2

typedef struct
{
	float *highest_arr;
	float *lowest_arr;
	int *cur_len_ptr;
	int size;

} time_price_arr;

typedef struct
{
	int date; //20180101
	float vol;
	float first;
	float highest;
	float lowest;
	float last;
	float delta;
	int day_trading;
	time_price_arr *time_price_arr_ptr;

} trade_day_info;

typedef struct
{
	trade_day_info **ptr_arr;
	int *cur_len_ptr;
	int size;

} trade_day_info_arr;

typedef struct
{
	int stock_id;
	int ipo_date; //20180101
	trade_day_info_arr *trade_day_info_arr_ptr;

} stock_data;

typedef struct
{
	stock_data **ptr_arr;
	int *cur_len_ptr;
	int size;

} stock_data_arr;

trade_day_info_arr *new_trade_day_info_arr_ptr(const int size);
void del_trade_day_info_arr(trade_day_info_arr *trade_day_info_arr_ptr);

void add_trade_day_info_new_item(trade_day_info_arr *trade_day_info_arr_ptr, int date, float vol, float first, float highest, float lowest, float last, float delta);
void update_trade_day_info_last_item(trade_day_info_arr *trade_day_info_arr_ptr, int date, float vol, float first, float highest, float lowest, float last, float delta);
int find_idx_by_date(trade_day_info_arr *trade_day_info_arr_ptr, int date);
int find_idx_range_by_yyyymm(trade_day_info_arr *trade_day_info_arr_ptr, int yyyymm, int *start_idx_ptr, int *end_idx_ptr);

int days_range;
int delta_percentage_min;
int new_high_percentage_filter;
float price_limit;

int is_new_high(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx);
int is_jump(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx);
int has_gap(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx);
int is_attack(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx);

int is_buy_target(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx, float mppt, int rule_no);
float get_RoI(trade_day_info **trade_day_info_ptr_arr, int trade_day_info_idx, float mppt, int rule_no);

#define ROUND_UP 0
#define ROUND_DOWN 0
float price_normalize(float price, int round); //round = 0:round-up 1:round-down

#endif