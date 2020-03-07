#include "actuators.h"
#include "pwm.h"

#include "setup_pwm.h"

const uint8_t marmot1_pins[PWM_CHANNELS] = {21, 22, 23, 2, 3, 4, 5, 6};
const uint8_t aura2_pins[PWM_CHANNELS] = {6, 5, 4, 3, 23, 22, 21, 20};
static uint8_t servoPins[PWM_CHANNELS];

// define if a channel is symmetrical or not (i.e. mapped to [0,1] for
// throttle, flaps, spoilers; [-1,1] for aileron, elevator, rudder
bool pwm_symmetrical[PWM_CHANNELS] = {0, 1, 1, 1, 1, 0, 0, 0};

void pwm_t::setup(int board) {
    Serial.print("PWM: ");
    if ( board == 0 ) {
        Serial.println("Marmot v1 pin mapping.");
        for ( int i = 0; i < PWM_CHANNELS; i++ ) {
            servoPins[i] = marmot1_pins[i];
        }
    } else if ( board == 1 ) {
        Serial.println("Aura v2 pin mapping.");
        for ( int i = 0; i < PWM_CHANNELS; i++ ) {
            servoPins[i] = aura2_pins[i];
        }
    } else {
        Serial.println("No valid PWM pin mapping defined");
    }

    analogWriteResolution(16);
    for ( int i = 0; i < PWM_CHANNELS; i++ ) {
        analogWrite(servoPins[i], 0); // zero signal to avoid surprises
        analogWriteFrequency(servoPins[i], servoFreq_hz);
    }
    
    // set default safe values for actuator outputs (should already at
    // a higher level, but this is important enough to do it again
    // just in case someone changed the higher level and messed up the
    // init order without realizing)
    actuators.setup();
    update();
}

// compute raw pwm values from normalized command values.  (handle
// actuator reversing here.)
void pwm_t::norm2pwm( float *norm, uint16_t *pwm ) {
    for ( int i = 0; i < PWM_CHANNELS; i++ ) {
        // convert to pulse length (special case ch6 when in flaperon mode)
        if ( pwm_symmetrical[i] || (i == 4 && actuators.config.mix_flaperon) ) {
            // i.e. aileron, rudder, elevator
            // Serial1.println(i);
            // Serial1.println(config_actuators.act_rev[i]);
            pwm[i] = PWM_CENTER + (int)(PWM_HALF_RANGE * norm[i] * actuators.config.act_gain[i]);
        } else {
            // i.e. throttle, flaps
            if ( actuators.config.act_gain[i] > 0.0 ) {
                pwm[i] = PWM_MIN + (int)(PWM_RANGE * norm[i] * actuators.config.act_gain[i]);
            } else {
                pwm[i] = PWM_MAX + (int)(PWM_RANGE * norm[i] * actuators.config.act_gain[i]);
            }
        }
        if ( pwm[i] < PWM_MIN ) {
            pwm[i] = PWM_MIN;
        }
        if ( pwm[i] > PWM_MAX ) {
            pwm[i] = PWM_MAX;
        }
    }
}


// write the raw actuator values to the RC system
void pwm_t::update(uint8_t test_pwm_channel) {
    // hook for testing servos
    if ( test_pwm_channel >= 0 && test_pwm_channel < PWM_CHANNELS ) {
        actuator_pwm[test_pwm_channel] = gen_pwm_test_value();
    }

    // sending servo pwm commands
    for ( uint8_t i = 0; i < PWM_CHANNELS; i++ ) {
        analogWrite(servoPins[i], actuator_pwm[i] / ((1/((float) servoFreq_hz)) * 1000000.0f )*65535.0f);
    }
}

// test drive a servo channel (sine wave)
uint16_t pwm_t::gen_pwm_test_value() {
    return sin((float)millis() / 500.0) * PWM_HALF_RANGE + PWM_CENTER;
}

// make a global instance
pwm_t pwm;