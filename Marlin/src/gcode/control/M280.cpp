/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if HAS_SERVOS

#include "../gcode.h"
#include "../../module/servo.h"

/**
 * M280: Get or set servo position. P<index> [S<angle>] [T<delay>]
 */
void GcodeSuite::M280() {
  if (!parser.seen('P')) return;

  const int servo_index = parser.value_int();
  if (WITHIN(servo_index, 0, NUM_SERVOS - 1)) {
    if (parser.seen('S')) {
      const int anew = parser.value_int();
      if (anew >= 0) {
        #if ENABLED(POLARGRAPH)
          if (parser.seen('T')) { // (ms) Total duration of servo move
            const int16_t t = constrain(parser.value_int(), 0, 10000);
            const int aold = servo[servo_index].read();
            millis_t now = millis();
            const millis_t start = now, end = start + t;
            while (PENDING(now, end)) {
              safe_delay(50);
              now = _MIN(millis(), end);
              MOVE_SERVO(servo_index, LROUND(aold + (anew - aold) * (float(now - start) / t)));
            }
          }
        #endif // POLARGRAPH
        MOVE_SERVO(servo_index, anew);
      } else
        servo[servo_index].detach();
    }
    else
      SERIAL_ECHO_MSG(" Servo ", servo_index, ": ", servo[servo_index].read());
  }
  else
    SERIAL_ERROR_MSG("Servo ", servo_index, " out of range");

}

#endif // HAS_SERVOS
