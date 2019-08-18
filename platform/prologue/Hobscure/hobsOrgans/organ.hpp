/*
 *  File: waves.hpp
 *
 *  Morphing Wavetable Synthesizer
 *
 */

#include "userosc.h"

struct Horgan{
    enum {
        k_flags_none = 0,
        k_flag_imperfection = 1<<1,
        k_flag_reset = 1<<2
    };

    struct Params{
        float onemix;
        float twomix;
        float fourmix;
        float eightmix;
        float sixteenmix;
        float imperfection;
        float softclip;

        Params(void) :
            onemix(0.f),
            twomix(0.f),
            fourmix(0.f),
            eightmix(0.f),
            sixteenmix(0.f),
            imperfection(0.f),
            softclip(0.f)
        {}
    };

    struct State{
        float phi;
        float w00;

        float imperfection;
        float imperfection1;
        float imperfection2;
        float imperfection4;
        float imperfection8;
        float imperfection16;
        
        uint32_t flags:8;

        State(void) : 
            w00(440.f * k_samplerate_recipf),
            flags(k_flags_none)
            {
                reset();
                imperfection = 0.f;
                imperfection1 = osc_white();
                imperfection2 = osc_white();
                imperfection4 = osc_white();
                imperfection8 = osc_white();
                imperfection16 = osc_white();
            }

        inline void reset(void){
            phi = 0;
        }
    };
    
    Horgan(void){
        init();
    }

    void init(void){
        state = State();
        params = Params();
    }

    inline void updatePitch(float w0){
        state.w00 = w0;
    }

    inline void update(const uint16_t flags){
        
    }
    State state;
    Params params;
};