from .c_api import stock


def get_new_high(work_arr_cptr, days_range, delta_percentage_min):
    stock.set_days_range(days_range)
    stock.set_delta_percentage_min(delta_percentage_min)
    opt_list = stock.work(work_arr_cptr, stock.WORK_TYPE_NEWHIGH)

    return opt_list


def get_attack(work_arr_cptr, days_range, delta_percentage_min):
    stock.set_days_range(days_range)
    stock.set_delta_percentage_min(delta_percentage_min)
    opt_list = stock.work(work_arr_cptr, stock.WORK_TYPE_ATTACK)

    return opt_list


def cal_p(work_arr_cptr, days_range, delta_percentage_min, days, mppt, buy_rule_no, roi_rule_no):
    stock.set_days_range(days_range)
    stock.set_delta_percentage_min(delta_percentage_min)
    return stock.calc_days_e(work_arr_cptr, days, mppt, buy_rule_no, roi_rule_no)


def cal_day_e(work_arr_cptr, days_range, delta_percentage_min, date, mppt, buy_rule_no, roi_rule_no):
    stock.set_days_range(days_range)
    stock.set_delta_percentage_min(delta_percentage_min)
    return stock.calc_day_e(work_arr_cptr, date, mppt, buy_rule_no, roi_rule_no)


def cal_month_e(work_arr_cptr, days_range, delta_percentage_min, yyyymm, mppt, buy_rule_no, roi_rule_no):
    stock.set_days_range(days_range)
    stock.set_delta_percentage_min(delta_percentage_min)
    return stock.calc_month_e(work_arr_cptr, yyyymm, mppt, buy_rule_no, roi_rule_no)


def get_idx_by_stock_id(stock_data_cptr_list, stock_id):
    for i, stock_data_cptr in enumerate(stock_data_cptr_list):
        if stock.get_stock_id(stock_data_cptr) == stock_id:
            return i

    return -1
