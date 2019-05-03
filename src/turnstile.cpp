#define NDEBUG

#include "turnstile.h"
#include <cassert>
#include <mutex>

decltype(Mutex::protections) Mutex::protections{};

decltype(Mutex::mark) Mutex::mark{};

Mutex::Mutex() : state{nullptr} {}

Mutex::Turnstile::Turnstile() noexcept
    : owned(true), thread_count(0), waiting_threads{} {}

void Mutex::Turnstile::subscribe(std::unique_lock<std::mutex> &holding) {
  if (owned) {
    // Wait for exclusive access, counting waiting threads.
    ++thread_count;
    waiting_threads.wait(holding, [this]() { return !owned; });
    --thread_count;
  }
  owned = true;
}

void Mutex::Turnstile::leave() { owned = false; }

bool Mutex::Turnstile::empty() const { return !owned && !thread_count; }

void Mutex::Turnstile::notify() { waiting_threads.notify_one(); }

uintptr_t Mutex::hash() const {
  return ((uintptr_t)(this) >> 3) & (protection_distribution_size - 1);
}

void Mutex::lock() {
  std::unique_lock<std::mutex> exclusion_lock{protections[hash()]};

  if (state == nullptr) {
    // Enter critical section, setting @state to @mark.
    state.reset(&mark);
  } else {
    // If no turnstile attached, create one.
    if (state.get() == &mark) {
      (void)state.release();
      state.reset(new Turnstile);
    }

    // Wait for exclusive access.
    state->subscribe(exclusion_lock);
    if (state->empty()) {
      state.reset(&mark);
    }
  }
  assert(state != nullptr);
}

void Mutex::unlock() {
  std::unique_lock<std::mutex> exclusion_lock{protections[hash()]};
  assert(state != nullptr);

  if (state.get() == &mark) {
    // Reset if no one waits.
    (void)state.release();
    state.reset();
  } else {
    // Wake one thread otherwise.
    state->leave();
    if (state->empty()) {
      state.reset();
    } else {
      state->notify();
    }
  }
}
