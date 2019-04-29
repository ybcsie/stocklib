from . import msgopt, tools
from .c_api import stock
import datetime
import urllib.request
import os


logger = msgopt.Logger("updater")


def download_file(url, path):
    max_try = 3
    while True:
        logger.logp("Trying connection...")
        try:
            urllib.request.urlretrieve(url, path)
            logger.logp("OK")
            return 0

        except:
            logger.logp("Error: urllib")
            tools.wait_retry(logger, 5)
            if max_try == 0:
                return -1

            max_try -= 1
            continue


def update_listed_list(listed_path):
    return download_file("http://140.116.39.233/stockserver/data/listed.sid", listed_path)


def require_update(update_log_path):
    if not os.path.exists(update_log_path):
        return True

    log_file = open(update_log_path, 'r', encoding="UTF-8")
    last_datetime = datetime.datetime.strptime(log_file.read(), "%Y/%m/%d/%H")
    log_file.close()
    now = datetime.datetime.now()

    update_datetime = datetime.datetime.strptime(
        "{}/{}/{}/17".format(now.year, now.month, now.day), "%Y/%m/%d/%H")
    if now.hour < 17:
        update_datetime -= datetime.timedelta(days=1)

    if last_datetime >= update_datetime:
        return False

    return True


def update_smd_in_list(stock_data_cptr_list, smd_dir, force_update=False):
    if not os.path.exists(smd_dir):
        os.makedirs(smd_dir)

    update_log_path = smd_dir + "/update.log"

    error = 0
    if require_update(update_log_path) or force_update:
        for stock_data_cptr in stock_data_cptr_list:
            stock_id = stock.get_stock_id(stock_data_cptr)
            smd_path = "{}/{}.smd".format(smd_dir, stock_id)

            logger.logp("update smd: {}".format(stock_id))
            url = "http://140.116.39.233/stockserver/data/smd/{}.smd".format(stock_id)
            if download_file(url, smd_path) != 0:
                error += 1

    if error == 0:
        update_log_file = open(update_log_path, 'w')
        update_log_file.write(datetime.datetime.now().strftime("%Y/%m/%d/%H"))
        update_log_file.close()


def update_dtd(dtd_dir, months=1):
    if not os.path.exists(dtd_dir):
        os.makedirs(dtd_dir)

    now = datetime.datetime.now()
    cur_month = now.month
    cur_year = now.year
    for i in range(months):
        if cur_month == 0:
            cur_month = 12
            cur_year -= 1

        yyyymm = cur_year * 100 + cur_month

        if yyyymm < 201401:
            break

        dtd_path = "{}/{}.dtd".format(dtd_dir, yyyymm)

        if not (yyyymm < now.year * 100 + now.month and os.path.exists(dtd_path)):
            download_file("http://140.116.39.233/stockserver/data/dtd/{}.dtd".format(yyyymm), dtd_path)

        cur_month -= 1


def update_sfd_in_list(stock_data_cptr_list, sfd_dir, force_update=False):
    update_log_path = sfd_dir + "/update.log"

    if not os.path.exists(sfd_dir):
        os.makedirs(sfd_dir)

    error = 0
    if require_update(update_log_path) or force_update:
        for stock_data_cptr in stock_data_cptr_list:
            stock_id = stock.get_stock_id(stock_data_cptr)
            sfd_path = "{}/{}.sfd".format(sfd_dir, stock_id)

            logger.logp("update sfd: {}".format(stock_id))
            url = "http://140.116.39.233/stockserver/data/sfd/{}.sfd".format(stock_id)
            if download_file(url, sfd_path) != 0:
                error += 1

    if error == 0:
        update_log_file = open(update_log_path, 'w')
        update_log_file.write(datetime.datetime.now().strftime("%Y/%m/%d/%H"))
        update_log_file.close()

