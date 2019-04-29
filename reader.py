from . import tools, msgopt, utils
from .c_api import stock
import os
import json
import datetime


logger = msgopt.Logger("reader")


def read_stock_data_cptr_list(sid_path, trade_day_size):
    if not os.path.exists(sid_path):
        raise RuntimeError("{} not exist".format(sid_path))

    sid_reader = open(sid_path, 'r', encoding="UTF-8")
    sid_list = sid_reader.read().split(';')
    sid_reader.close()

    stock_data_cptr_list = []

    for sid in sid_list:
        stock_info = sid.split(',')
        if len(stock_info) == 2:
            ipo_datetime = datetime.datetime.strptime(stock_info[1], "%Y/%m/%d")
            limit_datetime = datetime.datetime.now() - datetime.timedelta(days=30)

            if limit_datetime > ipo_datetime:
                stock_data_cptr_list.append(stock.new_stock_data_ptr(int(stock_info[0]),
                                                                     tools.date2int(stock_info[1]),
                                                                     trade_day_size))
            else:
                print("limit_datetime < ipo_datetime: {}".format(stock_info[0]))
        else:
            print("Error: stock list -- {}".format(sid))

    return stock_data_cptr_list


def read_trade_day_list(smd_path, stock_data_cptr, months):
    if not os.path.exists(smd_path):
        print("{} not exist".format(smd_path))
        return

    # read
    smd_file = open(smd_path, 'r', encoding="UTF-8")
    content_dict = json.loads(smd_file.read())
    smd_file.close()

    now = datetime.datetime.now()
    cur_month = now.month
    cur_year = now.year

    key_list = []

    for i in range(months):
        if cur_month == 0:
            cur_month = 12
            cur_year -= 1

        key = "{}{:02d}".format(cur_year, cur_month)
        trade_day_list = content_dict.get(key)
        if trade_day_list is not None:
            if len(trade_day_list) > 0:
                key_list.append(key)

        cur_month -= 1

    for key in reversed(key_list):
        for trade_day in content_dict[key]:
            if not (trade_day[3] == trade_day[4] == trade_day[5] == trade_day[6] == "--"):
                stock.add_trade_day_info(stock_data_cptr, tools.tw_date2int(trade_day[0]),
                                         tools.float_parser(trade_day[1]), tools.float_parser(trade_day[3]),
                                         tools.float_parser(trade_day[4]), tools.float_parser(trade_day[5]),
                                         tools.float_parser(trade_day[6]), tools.float_parser(trade_day[7]))


def read_trade_data_in_list(trade_data_dir, stock_data_cptr_list, months):
    for stock_data_cptr in stock_data_cptr_list:
        stock_id = stock.get_stock_id(stock_data_cptr)
        # print("read trade data {}".format(stock_id))
        read_trade_day_list("{}/{}.smd".format(trade_data_dir, stock_id), stock_data_cptr, months)


def read_dtd(dtd_path, stock_data_cptr_list):
    if not os.path.exists(dtd_path):
        raise RuntimeError("{} not exist".format(dtd_path))

    dtd_file = open(dtd_path, 'r')
    content_dict = json.loads(dtd_file.read())
    dtd_file.close()

    for key, value in content_dict.items():
        if len(value) == 0:
            continue

        for stock_dtd in value:
            try:
                stock_id = int(stock_dtd[0])
            except:
                continue

            idx = utils.get_idx_by_stock_id(stock_data_cptr_list, stock_id)
            if idx == -1:
                continue

            stock.enable_day_trading(stock_data_cptr_list[idx], int(key))


def read_all_dtd(dtd_dir, stock_data_cptr_list):
    if not os.path.exists(dtd_dir):
        raise RuntimeError("{} not exist".format(dtd_dir))

    for dtd_filename in os.listdir(dtd_dir):
        if not dtd_filename.endswith(".dtd"):
            continue

        # print("read dtd {}".format(dtd_filename))
        read_dtd("{}/{}".format(dtd_dir, dtd_filename), stock_data_cptr_list)


def read_sfd(sfd_path, stock_data_cptr):
    if not os.path.exists(sfd_path):
        raise RuntimeError("{} not exist".format(sfd_path))

    sfd_file = open(sfd_path, 'r')
    content_dict = json.loads(sfd_file.read())
    sfd_file.close()

    for key, value in content_dict.items():
        if len(value) == 0:
            continue

        data_price = value.get("DataPrice")
        time_price = []
        for data in data_price:
            time_price.append([data[3], data[4]])

        stock.set_time_price(stock_data_cptr, int(key), time_price)


def read_sfd_in_list(sfd_dir, stock_data_cptr_list):
    for stock_data_cptr in stock_data_cptr_list:
        stock_id = stock.get_stock_id(stock_data_cptr)
        # print("read sfd {}".format(stock_id))
        read_sfd("{}/{}.sfd".format(sfd_dir, stock_id), stock_data_cptr)
