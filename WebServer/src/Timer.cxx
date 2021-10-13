#include <Timer.h>
#include <Channal.h>

#define NOW ((std::chrono::duration_cast<std::chrono::milliseconds>( \
std::chrono::steady_clock::now().time_since_epoch())).count())

Timer::Timer(std::shared_ptr<Channal> channal, int msec)
  : channal_(channal),
    expiredTime_(NOW + msec),
    isDeleted_(false) {
#ifdef DEBUG
  std::cout << "new timer, expired " << expiredTime_ 
    << "ms fd " << channal->getFd() << std::endl;
#endif 
}

Timer::~Timer() {
  if (channal_ != nullptr) {
#ifdef DEBUG
    std::cout << "~timer close fd " << channal_->getFd() << std::endl;
#endif 
    channal_->handleClose();
  } else {
#ifdef DEBUG
    std::cout << "this timer resource has been freed" << std::endl;
#endif 
  }
}

void Timer::setDeleted() {
  isDeleted_ = true;
#ifdef DEBUG
  std::cout << "timer's channal reset, fd " << channal_->getFd() 
    << std::endl;
#endif
  channal_.reset();
}

size_t Timer::getExpiredTime() {
  return expiredTime_;
}

bool Timer::isDeleted() {
  return isDeleted_;
}

bool Timer::isValid() {
  long long tmp = NOW;
  if (tmp > expiredTime_) {
#ifdef DEBUG
    std::cout << "Not valid, tmp " << tmp << "ms expired " 
      << expiredTime_ << "ms" <<std::endl;
#endif
    return false;
  } else {
    return true;
  }
}

std::shared_ptr<Timer> TimerManager::addTimer(std::shared_ptr<Channal> channal, int msec) {
  std::shared_ptr<Timer> timer(std::make_shared<Timer>(channal, msec));
  if (timer != nullptr) {
    timerQueue_.push(timer);
    return timer;
  }
  return nullptr;
}

void TimerManager::handleExpired() {
  //´Ó¶Ñ¶¥É¾³ý
  while (!timerQueue_.empty()) {
    std::shared_ptr<Timer> t = timerQueue_.top();
    if (t->isDeleted())
      timerQueue_.pop();
    else if (t->isValid() == false)
      timerQueue_.pop();
    else
      break;
  }
}


