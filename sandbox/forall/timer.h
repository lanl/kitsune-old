//
//
//
//
#ifndef _TIMER_H_
#define _TIMER_H_
#include <cassert>
#include <string>
#include <chrono>
#include <ratio>

// A simple wrapper around C++'s std::chrono capabilities for measuring
// time.  The usage of the timer is primarily driven by a sequence of
// start() and stop() calls that are wrapped around the code whose
// execution should be timed.  For example,
//
//    Timer t;
//
//    t.start();
//    ...
//    --- code segment to time execution of...
//    ...
//    t.stop();
//
//    cout << "code took " << t.elapsedSeconds() << " to execute.\n";
//
//
// In addition to the elapsed time it is possible to capture the
// accumulated times for multiple start()+stop() sequences.  The
// timer will count the number of sequences and the accumulated
// time allowing you to capture an average time (for smoothing
// noise out, etc.).
//
// The accumulated time is available via the accumulatedSeconds()
// member function.  The average time is available via
// averageSeconds().
//
class Timer {

  public:

   Timer(const std::string &name)
   : _name(name)
   { _trips = 0; }

   ~Timer() { };

  // --- start the timer running
  //
  //   note: the code will assert that the timer is not already running.
  //
  void start()
  {
    assert(_running == false);
    _running = true;
    _start   = clock::now();
  }

  // --- stop the timer
  //
  //   note: the code will assert that the timer is running prior to this call.
  //
  void stop()
  {
    assert(_running == true);
    _stop = clock::now();
    _lap_time = std::chrono::duration_cast<duration>(_stop - _start);
    _total_time += _lap_time;
    _trips++;
    _running = false;
  }

  // --- get the number of elapsed seconds that passed between the last
  //     start() and stop() calls.
  //
  //  note: the code will assert that the timer is not currently running.
  //
  double elapsedSeconds() const
  {
    assert(_running == false);
    return _lap_time.count();
  }

  // --- get the total number of accumulated seconds that the timer has
  //     recorded (this is the sum of the total time measured between
  //     start-stop calls.
  //
  //  note: the code will assert that the time is not currently running.
  //
  double accumulatedSeconds() const
  { return _total_time.count(); }

  // --- get the average number of seconds over a sequence of start and
  //     stop invocations.  The return value is simply:
  //
  //         accumulatedSeconds() / # of trips.
  //
  double averageSeconds() const
  {
    assert(_trips > 0);
    return accumulatedSeconds() / _trips;
  }

  // --- get the current number of start+stop sequences the timer was
  //     invoked for.
  unsigned int tripCount() const
  { return _trips; }

  // --- reset the timer to its initial (un-started) state.
  //
  void reset()
  {
    assert(_running == false);
    _trips = 0;
    _running = false;
  }
  // --- get the time's name
  //
  const std::string &name() const
  { return _name; }

 private:
  typedef std::chrono::high_resolution_clock clock;
  typedef std::chrono::high_resolution_clock::time_point time_point;
  typedef std::chrono::duration<double> duration;

  std::string   _name;                  // user-specified name for time.
  bool          _running = false;       // is the timer running?
  time_point    _start, _stop;          // most recent start and stop points in time.
  unsigned int  _trips;                 // number of trips (times )
  duration      _lap_time, _total_time; // last start & stop sequence time (lap) and overall
                                        // accumulated time duration.
};

#endif // _TIMER_H_
