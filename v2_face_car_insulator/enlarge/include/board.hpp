#ifndef BOARD_HPP
#define BOARD_HPP
#include <vector>
#include "tcpsocket.hpp"

void board_init(void);
void board_close(void);

bool get_flag(void);

unsigned char get_signal(void);

void send_data_to_board(void *data, unsigned long &size);

void recv_data_from_board(vector<float> &data, unsigned long &size);


#endif
