#include "utility/cr_utility.h"

using namespace std;

void Proctimer::get_start_time()
{
  clock_gettime(CLOCK_REALTIME, &t_start);
}

void Proctimer::get_end_time(string proc_name)
{
  clock_gettime(CLOCK_REALTIME, &t_end);
  if(t_end.tv_nsec-t_start.tv_nsec < 0)
  {
    time_s = t_end.tv_sec-t_start.tv_sec-1;
    time_ms = (t_end.tv_nsec-t_start.tv_nsec+1e9)/1e6;
    time_us = ((t_end.tv_nsec-t_start.tv_nsec+1e9)-time_ms*1e6)/1e3;
    time_ns = (t_end.tv_nsec-t_start.tv_nsec+1e9)-time_us*1e3-time_ms*1e6;
    cout << proc_name << " time: " << time_s << " s, " << time_ms << " ms, "
      << time_us <<" us, " << time_ns << " ns." << endl;
  }
  else
  {
    time_s = t_end.tv_sec-t_start.tv_sec;
    time_ms = (t_end.tv_nsec-t_start.tv_nsec)/1e6;
    time_us = ((t_end.tv_nsec-t_start.tv_nsec)-time_ms*1e6)/1e3;
    time_ns = (t_end.tv_nsec-t_start.tv_nsec)-time_us*1e3-time_ms*1e6;
    cout << proc_name << " time: " << time_s << " s, " << time_ms << " ms, "
      << time_us <<" us, " << time_ns << " ns." << endl;
  }
}
