#ifndef SRC_TURNSTILE_H_
#define SRC_TURNSTILE_H_

#include <condition_variable>

class Mutex {
  class Turnstile {
    // Indication if turnstile is in usage.
    bool owned;
    // Waiting threads counter.
    size_t thread_count;
    // Threads waiting for exclusive access.
    std::condition_variable waiting_threads;

   public:
    Turnstile() noexcept;

    void subscribe(std::unique_lock<std::mutex> &holding);

    void leave();

    bool empty() const;

    void notify();
  };

  // Marker object, @state set to @mark only if mutex is owned, but no one is
  // waiting.
  static Turnstile mark;
  // Current turnstile object, holding waiting threads.
  std::unique_ptr<Turnstile> state;
  // Number of mutexes to increase concurrency.
  static constexpr uintptr_t protection_distribution_size = 256;
  // Mutexes array to increase concurrency.
  static std::array<std::mutex, protection_distribution_size> protections;
  // Hashing function to access array @protections.
  uintptr_t hash() const;

 public:
  Mutex();

  Mutex(const Mutex &) = delete;

  void lock();    // NOLINT
  void unlock();  // NOLINT
};

#endif  // SRC_TURNSTILE_H_
