/*
 * author: 657023321@qq.com
 * date: 2021-07-19
 * function: the defination of timer
 */

#include <timer.h>
#include <string.h>
#include <assert.h>

TimeSpace::TimeSpace(struct timeval *time) {
  time_.tv_sec = time->tv_sec;
  time_.tv_usec = time->tv_usec;
}

TimeSpace::TimeSpace(const TimeSpace &another) {
  time_ = another.time_;
}

TimeSpace& TimeSpace::operator = (const TimeSpace &another) {
  time_ = another.time_;

  return *this;
}

bool TimeSpace::operator > (const TimeSpace &another) const {
  return time_.tv_sec > another.time_.tv_sec || 
    (time_.tv_sec == another.time_.tv_sec && 
     time_.tv_usec > another.time_.tv_usec);
}

bool TimeSpace::operator < (const TimeSpace &another) const {
  return time_.tv_sec < another.time_.tv_sec ||
    (time_.tv_sec == another.time_.tv_sec && 
     time_.tv_usec < another.time_.tv_usec);
}

void TimeSpace::SetTime(struct timeval * time) {
  time_.tv_sec = time->tv_sec;
  time_.tv_usec = time->tv_usec;
}

struct timeval *TimeSpace::GetTimePtr(void) {
  return &time_;
}

void TimeSpace::Reset(void) {
  memset(&time_, 0, sizeof(time_));
  time_str_[0] = '\0';
}

void TimeSpace::Time2Str(void) {
  memset(time_str_, 0, sizeof(time_str_));
  strftime(time_str_, sizeof(time_str_) - 1, "%Y-%m-%d %H:%M:%S ", 
    localtime(&time_.tv_sec));
  strncat(time_str_, std::to_string(time_.tv_usec / 1000).c_str(), 
    sizeof(time_str_) - 1);
}

const char *TimeSpace::GetTimeStr(void) {
  Time2Str();

  return const_cast<const char *>(time_str_);
}

Timer::Timer(TimerCb callback, unsigned int sec, unsigned int usec, 
    bool is_repeat)
      : callback_(std::move(callback)),
        sec_(sec),
        usec_(usec),
        is_repeat_(is_repeat) {
  struct timeval tval;

  gettimeofday(&tval, NULL);
  tval.tv_sec += sec;
  tval.tv_usec += usec;
  time_ = TimeSpace(&tval);
}

bool Timer::IsTimeOut(void) {
  struct timeval cur_time;
  int ret;

  ret = gettimeofday(&cur_time, NULL);
  assert(ret == 0);

  if (TimeSpace(&cur_time) > time_) {
    return true;
  }

  return false;
}

TimeSpace& Timer::GetTime(void) {
  return time_;
}

int Timer::RunCallBack(void) {
  return callback_();
}

bool Timer::operator < (const Timer &another) const {
  return time_ < another.time_;
}

bool Timer::operator > (const Timer &another) const {
  return time_ > another.time_;
}

void Timer::ComputeTimeAgain(void) {
  struct timeval t;

  gettimeofday(&t, nullptr);
  t.tv_sec += sec_;
  t.tv_usec += usec_;

  time_.SetTime(&t);
}

TimerQueue::TimerQueue() 
  : timer_queue_(custom_priority_queue<Timer *, cmp>()) {

}

Timer *TimerQueue::AddTimer(TimerCb cb, int sec, int usec, bool is_repeat) {
  Timer *timer = new Timer(std::move(cb), sec, usec, is_repeat);

  if (!timer) {
    return nullptr;
  }

  timer_queue_.push(timer);

  return timer;
}

bool TimerQueue::DelTimer(Timer * timer) {
  return timer_queue_.remove(timer);
}

void TimerQueue::TimerProc(void) {
  Timer *tmp;
  int ret;

  LOG_DEBUG("process timer timeout event, current timer size %d", 
    timer_queue_.size());

  if (timer_queue_.empty()) {
    return;
  }

  while (timer_queue_.top()->IsTimeOut()) {
    LOG_DEBUG("This timer timeout, the time = %s", 
      timer_queue_.top()->GetTime().GetTimeStr());
 
    ret = timer_queue_.top()->RunCallBack();
    
    LOG_DEBUG("timer callbck ret %d", ret);
 
    if (timer_queue_.top()->IsRepeat()) {
      tmp = timer_queue_.top();
      timer_queue_.pop();
      tmp->ComputeTimeAgain();
      timer_queue_.push(tmp);
    } else {
      timer_queue_.pop();
    }
  }

  return;
}

int TimerQueue::GetTimeOfLateTimeOut() {
  struct timeval t1, *t2;
  
  if (timer_queue_.empty()) {
    return -1;
  }

  Timer *tmp = timer_queue_.top();
  if (tmp->IsTimeOut()) {
    return 0;
  }

  gettimeofday(&t1, nullptr);
  t2 = tmp->GetTime().GetTimePtr();
  if (TimeSpace(&t1) > tmp->GetTime()) {
    return 0;
  } else {
    return (t2->tv_sec - t1.tv_sec) * 1000 + (t2->tv_usec - t1.tv_usec) / 1000;
  }
}

