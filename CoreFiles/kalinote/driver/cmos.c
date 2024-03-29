/* 从cmos获取数据 */
#include <bootpack.h>

unsigned char read_cmos (unsigned char p) {
	unsigned char data;
	io_out8(cmos_index, p);
	data = io_in8(cmos_data);
	io_out8(cmos_index, 0x80);
	return data;
}

unsigned int get_hour_hex() {
	return BCD_HEX(read_cmos(CMOS_CUR_HOUR));
}

unsigned int get_min_hex() {
	return BCD_HEX(read_cmos(CMOS_CUR_MIN));
}

unsigned int get_sec_hex() {
	return BCD_HEX(read_cmos(CMOS_CUR_SEC));
}

unsigned int get_day_of_month() {
	return BCD_HEX(read_cmos(CMOS_MON_DAY));
}

unsigned int get_day_of_week() {
	return BCD_HEX(read_cmos(CMOS_WEEK_DAY));
}

unsigned int get_mon_hex() {
	return BCD_HEX(read_cmos(CMOS_CUR_MON));
}

unsigned int get_year() {
	return (BCD_HEX(read_cmos(CMOS_CUR_CEN))*100)+BCD_HEX(read_cmos(CMOS_CUR_YEAR))-30 + 2010;
}

unsigned char cmos_hd_info() {
	unsigned char cmos_disks, nr_hd;
	
	if ((cmos_disks = read_cmos(CMOS_DEV_TYPE)) & 0xf0)
		if (cmos_disks & 0x0f)
			nr_hd = 2;
		else
			nr_hd = 1;
	else
		nr_hd = 0;
	return nr_hd;
}
