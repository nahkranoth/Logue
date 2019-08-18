/*
    BSD 3-Clause License

    Copyright (c) 2018, KORG INC.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//*/

/*
 * File: organ.cpp
 *
 * Naive Organ oscillator test
 *
 */

#include "userosc.h"

typedef struct State {
  float w0;
  float oscTwo;
  float oscFour;
  float oscSix;
  float oscEight;
  float phase;
  uint8_t flags;
} State;

static State s_state;

typedef struct Params {
    float oscTwo;
    float oscFour;
    float oscSix;
    float oscEight;
} Params;

static Params s_params;

enum {
  k_flags_none = 0,
  k_flag_osctwo = 1<<1,
  k_flag_oscfour = 1<<2,
  k_flag_oscsix = 1<<3,
  k_flag_osceight = 1<<4,
  k_flag_reset = 1<<5
};

void OSC_INIT(uint32_t platform, uint32_t api)
{
  s_state.w0    = 0.f;
  s_state.phase = 0.f;
  s_state.oscTwo = 1.f;
  s_state.oscFour = 1.f;
  s_state.oscSix = 1.f;
  s_state.oscEight = 1.f;
  s_state.flags = k_flags_none;
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames)
{  
    const uint8_t flags = s_state.flags;
    s_state.flags = k_flags_none;

    const float w0 = s_state.w0 = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);
    float phase = (flags & k_flag_reset) ? 0.f : s_state.phase;

    if (flags & k_flag_osctwo) {
        s_state.oscTwo = s_params.oscTwo;
    }
    if (flags & k_flag_oscfour) {
        s_state.oscFour = s_params.oscFour;
    }
    if (flags & k_flag_oscsix) {
        s_state.oscSix = s_params.oscSix;
    }
    if (flags & k_flag_osceight) {
        s_state.oscEight = s_params.oscEight;
    }

    q31_t * __restrict y = (q31_t *)yn;
    const q31_t * y_e = y + frames;

    for (; y != y_e; ) {
    // Main signal
    const float sig  = osc_sinf(phase)*s_state.oscTwo;
    const float sig2 = osc_sinf(phase*2.f)*s_state.oscFour;
    const float sig3 = osc_sinf(phase*4.f)*s_state.oscSix;
    const float sig4 = osc_sinf(phase*8.f)*s_state.oscEight;

    const float signal = sig+sig2+sig3+sig4;

    *(y++) = f32_to_q31(signal);

    phase += w0;
    phase -= (uint32_t)phase;

    }
    s_state.phase = phase;
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
  s_state.flags |= k_flag_reset;
}

void OSC_NOTEOFF(const user_osc_param_t * const params)
{
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  const float valf = param_val_to_f32(value);
  //TOOO NAIVE; look at waves example
  switch (index) {
  case k_osc_param_id1:
    s_params.oscTwo = valf;
    s_state.flags |= k_flag_osctwo;
  case k_osc_param_id2:
    s_params.oscFour = valf;
    s_state.flags |= k_flag_oscfour;
  case k_osc_param_id3:
    s_params.oscSix = valf;
    s_state.flags |= k_flag_oscsix;
  case k_osc_param_id4:
    s_params.oscEight = valf;
    s_state.flags |= k_flag_osceight;
  case k_osc_param_id5:
  case k_osc_param_id6:
    break;
  case k_osc_param_shape:
    break;
  case k_osc_param_shiftshape:
    break;
  default:
    break;
  }
}

