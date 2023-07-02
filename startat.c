void startat(timer_t* t, int year, int month, int day, int hour, int minute, int second) {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);

  struct tm startTm;
  startTm.tm_sec = second;
  startTm.tm_min = minute;
  startTm.tm_hour = hour;
  startTm.tm_mday = day;
  startTm.tm_mon = month - 1;
  startTm.tm_year = year - 1970;
  startTm.tm_isdst = -1;

  time_t startTime = mktime(&startTm);
  int startDelay = (int)(startTime - currentTime.tv_sec);

  while (startDelay > 0) {
    sleep(1);
  }

  start(t);
}