/* Utility to calibrate the debounce delay for a mechanical switch (e.g., a
 * tactile/button switch).
 *
 * Circuit
 * =======
 * This program expects an Arduino-compatible board with internal pull-up
 * resistors (i.e., digital pin mode `INPUT_PULLUP` supported), with the
 * following connections:
 *
 *     Digital pin `kPinSwitch` ---- switch ---- ground
 *
 * How to use
 * ==========
 * Slowly toggle the switch, and inspect the resulting serial printout.  I'd
 * recommend waiting at least 2 seconds between toggles.  (Printouts are held
 * in memory for 1 second before printing, so if the toggle time is less than
 * that, printouts will be lost.)
 *
 * We define a "streak" to be a span of at least `kSteadyThresholdUsec`
 * microseconds during which the digital input has consistently read the same
 * value.  Shortly after the end of each streak, a message like the following
 * is printed to serial:
 *
 *     Streak with value 0 started at timestamp 19496560 usec and ended at time 21879656 usec
 *     Time between streaks = 0 usec (+/- approx 8 usec)
 *
 * More example outputs:
 *
 *     Streak with value 0 started at timestamp 19496560 usec and ended at time 21879656 usec
 *     Time between streaks = 0 usec (+/- approx 8 usec)
 *     Streak with value 1 started at timestamp 21880396 usec and ended at time 24318016 usec
 *     Time between streaks = 740 usec (+/- approx 12 usec)
 *     Streak with value 0 started at timestamp 24318016 usec and ended at time 26807016 usec
 *     Time between streaks = 0 usec (+/- approx 12 usec)
 *
 * The error term (`+/- approx __ usec`) is the amount of time between
 * consecutive reads (i.e., consecutive executions of `loop()`).
 */

#include <Arduino.h>
#include <Wire.h>


using MicrosTimestamp = decltype(micros());
using MicrosTimeDelta = decltype((MicrosTimestamp)2 - (MicrosTimestamp)1);
using DigitalPinState = decltype(LOW);

constexpr uint8_t kPinSwitch = 2;

constexpr uint32_t kSteadyThresholdUsec = 1000000;
DigitalPinState cur_streak_state;
MicrosTimestamp cur_streak_start_ts_usec;
MicrosTimestamp prev_read_ts_usec;

// Printing to serial takes longer than the switch latency we're trying to
// measure.  So, instead of printing to serial right at the start or end of a
// streak, we print to serial about a given streak in the middle of the next
// streak.  To facilitate this, we store the to-be-printed information in the
// following variables.
constexpr uint32_t kSerialPrintDelay = 1000000;
bool print_queued = false;
DigitalPinState prev_streak_state;
MicrosTimestamp prev_streak_start_ts_usec;
MicrosTimestamp prev_streak_end_ts_usec;
MicrosTimeDelta time_between_streaks;

void setup() {
    pinMode(kPinSwitch, INPUT_PULLUP);

    cur_streak_state = digitalRead(kPinSwitch);
    cur_streak_start_ts_usec = micros();

    Serial.begin(9600);
}

void loop() {
    const auto cur_state = digitalRead(kPinSwitch);
    const auto cur_ts_usec = micros();

    if (cur_state != cur_streak_state) {
        const auto cur_streak_duration_usec = (cur_ts_usec -
                                               cur_streak_start_ts_usec);
        if  (cur_streak_duration_usec > kSteadyThresholdUsec) {
            print_queued = true;
            time_between_streaks = (cur_streak_start_ts_usec -
                                    prev_streak_end_ts_usec);
            prev_streak_state = cur_streak_state;
            prev_streak_start_ts_usec = cur_streak_start_ts_usec;
            prev_streak_end_ts_usec = cur_ts_usec;
        }
        cur_streak_state = cur_state;
        cur_streak_start_ts_usec = cur_ts_usec;
    } else if (print_queued &&
               cur_ts_usec - cur_streak_start_ts_usec > kSerialPrintDelay) {
        Serial.print("Streak with value ");
        Serial.print(prev_streak_state);
        Serial.print(" started at timestamp ");
        Serial.print(prev_streak_start_ts_usec);
        Serial.print(" usec and ended at time ");
        Serial.print(prev_streak_end_ts_usec);
        Serial.println(" usec");
        Serial.print("Time between streaks = ");
        Serial.print(time_between_streaks);
        Serial.print(" usec (+/- approx ");
        Serial.print(cur_ts_usec - prev_read_ts_usec);
        Serial.println(" usec)");

        print_queued = false;
    }
    prev_read_ts_usec = cur_ts_usec;
}
