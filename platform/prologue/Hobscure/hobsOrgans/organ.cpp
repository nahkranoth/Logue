#include "userosc.h"
#include "organ.hpp"

static Horgan s_organ;

void OSC_INIT(uint32_t platform, uint32_t api){
    s_organ = Horgan();
}

void OSC_CYCLE(const user_osc_param_t * const params, 
                int32_t *yn, 
                const uint32_t frames)
{
    Horgan::State &s = s_organ.state;
    const Horgan::Params &p = s_organ.params;

    //Events
    {
        const uint32_t flags = s.flags;
        s.flags = Horgan::k_flags_none;

        s_organ.updatePitch(osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF));
        s_organ.update(flags);

        if (flags & Horgan::k_flag_imperfection) {
            s.imperfection = p.imperfection;
        }

        if(flags & Horgan::k_flag_reset){
            s.reset();
        }
    }

    float phi = s.phi;
    const float onemix = p.onemix;
    const float twomix = p.twomix;
    const float fourmix = p.fourmix;
    const float eightmix = p.eightmix;
    const float sixteenmix = p.sixteenmix;

    const float softClip = p.softclip;

    

    const float imp1 = s.imperfection1 * s.imperfection;
    const float imp2 = s.imperfection2 * s.imperfection;
    const float imp4 = s.imperfection4 * s.imperfection;
    const float imp8 = s.imperfection8 * s.imperfection;
    const float imp16 = s.imperfection16 * s.imperfection;

    q31_t * __restrict y = (q31_t *)yn;
    const q31_t * y_e = y + frames;
    
    for (; y != y_e; ) {
      
        const float sin1 = osc_sinf(phi*4.f+imp1)*onemix;
        const float sin2 = osc_sinf(phi*3.f+imp2)*twomix;
        const float sin3 = osc_sinf(phi*2.f+imp4)*fourmix;
        const float sin4 = osc_sinf(phi+imp8)*eightmix;
        const float sin5 = osc_sinf(phi*.5f+imp16)*sixteenmix;
        const float sins = sin1+sin2+sin3+sin4+sin5;

        float sig = osc_softclipf(0.22f, (softClip+0.5f)*sins);

        *(y++) = f32_to_q31(sig);
        phi += s.w00;
        phi -= (uint32_t)phi;
    }

    s.phi = phi;
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
  s_organ.state.flags |= Horgan::k_flag_reset;
}

void OSC_NOTEOFF(const user_osc_param_t * const params)
{
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{ 
  Horgan::Params &p = s_organ.params;
  Horgan::State &s = s_organ.state;
  
  switch (index) {
  case k_osc_param_id1:
    // sub mix
    // percent parameter
    p.onemix = clip01f(value * 0.01f * 0.90f); // scale in 0.05 - 0.95
    break;

  case k_osc_param_id2:
    p.twomix = clip01f(value * 0.01f * 0.90f); // scale in 0.05 - 0.95
    break;
  
  case k_osc_param_id3:
    p.fourmix = clip01f(value * 0.01f * 0.90f); // scale in 0.05 - 0.95
    break;

  case k_osc_param_id4:
    p.eightmix = clip01f(value * 0.01f * 0.90f); // scale in 0.05 - 0.95
    break;
  
  case k_osc_param_id5:
    p.sixteenmix = clip01f(value * 0.01f * 0.90f); // scale in 0.05 - 0.95
    break;
  
  case k_osc_param_id6:
    p.imperfection = clip01f(value * 0.01f * 0.90f);
    s.flags |= Horgan::k_flag_imperfection;
    break;
    
  case k_osc_param_shape:
    p.softclip = clip01f(value * 0.01f * 0.33f); // scale in 0.05 - 0.95
    break;
    
  case k_osc_param_shiftshape:
    break;
    
  default:
    break;
  }
}