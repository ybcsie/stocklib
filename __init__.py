from .c_api import stock
from . import reader, tools, updater, utils
from .msgopt import Logger


def init_work_arr(stock_data_cptr_list):
    stock_arr_cptr = stock.new_stock_data_arr_ptr(len(stock_data_cptr_list))
    for stock_data_cptr in stock_data_cptr_list:
        stock.add_stock_data(stock_arr_cptr, stock_data_cptr)

    return stock_arr_cptr


def del_work_arr(stock_arr_cptr):
    stock.del_stock_data_arr(stock_arr_cptr)
