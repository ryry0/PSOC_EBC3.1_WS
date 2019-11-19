/**
* @file CwruStimExe.c
* @brief The CWRU Stimulation Library (CwruStimLib) UECU exe func.
* @author Lu Li - lxl361@case.edu
*
* @version 1.0a
* @date 08/11/2017
*/

#include "CwruStimLib.h"

// Calulate pattern ramping function
uint8_t stim_get_PW_ramping(int channel_i,
                        const uint16_t (*LUT_PP_t)[16][8],
                        const uint8_t (*LUT_PW_t)[16][8],
                        uint16_t cycle_pp_t)
{
  // search in LUT_PP_t[i][:] find where cycle_pp_t belongs to
  // find the upper limit of ramp

  // calulated ramping value, set to 0 for safety
  uint8_t pw_ramping_val = 0;

  // search until found the correct ramping value
  int found_ramp = 0;

  // prevent pp greater than 100%
  if (cycle_pp_t>10000) {
    cycle_pp_t = 10000;
  }

  int j=0;
  while (found_ramp == 0) {
    // LUT resolution step iterator

    // only search 0 to (RES-1)
    if (j < GAIT_LUT_RES) {
      //(*LUT_PP_t)[channel_i][j]
      if (cycle_pp_t == (*LUT_PP_t)[channel_i][j]) {
        // if equal, then use PP to directly find PW value, no need for ramping
        // direct return the ramped PW value;
        pw_ramping_val =  (*LUT_PW_t)[channel_i][j];

        // and exit search loop
        found_ramp = 1;

      } else if (cycle_pp_t < (*LUT_PP_t)[channel_i][j]) {

        // if cycle_pp_t is smaller then this LUT_PP_t
        // then cycle_pp_t belongs to this slot
        // that means the previous LUT_PP_t is the correct lower limit
        // and do calculation in here, then break the search loop

        // Calculate the ramping value
        //                              time_diff
        // pw_ramping_val = pw_diff * (-----------) + PW_low
        //                              time_div
        // then,
        //                                     PP_now - PP_low
        // pw_ramping_val = (PW_up-PW_low) * (-----------------) + PW_low
        //                                     PP_up - PP_low
        uint16_t time_diff = cycle_pp_t - (*LUT_PP_t)[channel_i][j-1];
        uint16_t time_base = (*LUT_PP_t)[channel_i][j] - (*LUT_PP_t)[channel_i][j-1];
        float time_div = (float)time_diff / (float)time_base;
        float pw_diff = (float)((*LUT_PW_t)[channel_i][j] - (*LUT_PW_t)[channel_i][j-1]);
        uint8_t pw_base;
        if (j==0) {
          pw_base = (*LUT_PW_t)[channel_i][j];
        } else {
          pw_base = (*LUT_PW_t)[channel_i][j-1];
        }

        pw_ramping_val = (uint8_t) round(pw_diff * time_div) + pw_base;
        // if has error in PSOC
        // ref to this:
        // https://community.cypress.com/docs/DOC-12152

        // and exit search loop
        found_ramp = 1;

      } else {
        // this means either cycle_pp_t should belong to this slot,
        // or we still need to keep serching.
        // so keep increment j, or keep the search loop
        found_ramp = 0;
        j++;

      } // end if else ..
    }// end if j<
  } // end while (found_ramp)

  // return the ramped PW value;
  return pw_ramping_val;

}

// channel pw gains for dynamic gait control
// related var: float cwru_stim->_current_pw_gains[cwru_stim->STIM_CHANNEL_USED];
int stim_set_chan_pw_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel, float gain)
{
  // fault proof
  if (channel>cwru_stim->STIM_CHANNEL_USED) return -1;
  if (gain<=0) return -1;

  // set command, update the gain array
  cwru_stim->_current_pw_gains[channel] = gain;
  return 1;
}

// get channel pw gain function
float stim_get_chan_pw_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel)
{
  // get command, do nothing here, just return value
  return cwru_stim->_current_pw_gains[channel];
}

// execute command, apply the gain to _current_pulse_width[], but not save;
uint8_t stim_exe_chan_pw_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel)
{
  float pw_cal = cwru_stim->_current_pw_gains[channel] *
  								(float)cwru_stim->_current_pulse_width[channel];

  // when result is [0,MAX] publish, otherwise remain the same
  if (pw_cal < 0) {
    return 0; // min limit
  } else if (pw_cal>PW_VAL_MAX) {
    return PW_VAL_MAX; // max limit
  } else {
    return (uint8_t) pw_cal; // output = pw * gain
  }

}

// channel amp gains for dynamic gait control
// related var: float _current_amp_gains[cwru_stim->STIM_CHANNEL_USED];
int stim_set_chan_amp_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel, float gain) {
  // fault proof
  if (channel>cwru_stim->STIM_CHANNEL_USED) return -1;
  if (gain<=0) return -1;

  // set command, update the gain array
  cwru_stim->_current_amp_gains[channel] = gain;
  return 1;
}

// get channel amp gain function
float stim_get_chan_amp_gain(cwru_stim_struct_t *cwru_stim, uint8_t channel)
{
  // get command, do nothing here, just return value
  return cwru_stim->_current_amp_gains[channel];
}

// execute command, apply the gain to _current_amplitude[], but not save;
uint8_t stim_exe_chan_amp_gain(cwru_stim_struct_t *cwru_stim,  uint8_t channel)
{
  float amp_cal = cwru_stim->_current_amp_gains[channel] *
  						(float)cwru_stim->_current_amplitude[channel];

  // when result is [0,MAX] publish, otherwise remain the same
  if (amp_cal < 0) {
    return 0; // min limit
  } else if (amp_cal>AMP_VAL_MAX) {
    return AMP_VAL_MAX; // max limit
  } else {
    return (uint8_t) amp_cal; // output = amp * gain
  }

}

// Update Stim pattern via UART
// stim_update(type, pattern, cycle_percentage)
int stim_update_cycle(cwru_stim_struct_t *cwru_stim,
                      int type, int pattern, uint16_t cycle_percentage) {

  /*
  // Pattern look up table temp holder.
  const uint16_t (*LUT_PP)[12][8] = NULL;
  const uint8_t (*LUT_PW)[12][8] = NULL;
  const uint8_t (*LUT_IPI)[12] = NULL;
  const uint8_t (*LUT_AMP)[12] = NULL;

  // type mask
  int board = type & 0xFF00;
  int param = type & 0x00FF;

  int need_update = 0;

  // Select update type
  switch (param) {
    // if need to update IPI
    case IPI:
      // Select which pattern
      need_update = 1;
      switch (pattern) {
        case PATTERN_NO_STIM:
          need_update = 0;
          break;

        // IPI - Stand
        case PATTERN_STAND:
          if (board == VCK5_BRD1) {
            // #if defined(DEBUG_STIM_UPDATE_IPI) && defined(DEBUG_ON)
            //   //-Serial.print("-- -- VCK5_BRD1 ");
            // #endif
            LUT_IPI = &gait_stand_B1_IPI;
          } else if (board == VCK5_BRD2) {
            // #if defined(DEBUG_STIM_UPDATE_IPI) && defined(DEBUG_ON)
            //   //-Serial.print("-- -- VCK5_BRD2 ");
            // #endif
            LUT_IPI = &gait_stand_B2_IPI;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // IPI - SIT
        case PATTERN_SIT:
          if (board == VCK5_BRD1) {
            LUT_IPI = &gait_sit_B1_IPI;
          } else if (board == VCK5_BRD2) {
            LUT_IPI = &gait_sit_B2_IPI;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // IPI - LSTEP
        case PATTERN_LSETP:
          if (board == VCK5_BRD1) {
            LUT_IPI = &gait_walk_L_B1_IPI;
          } else if (board == VCK5_BRD2) {
            LUT_IPI = &gait_walk_L_B2_IPI;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // IPI - RSTEP
        case PATTERN_RSETP:
          if (board == VCK5_BRD1) {
            LUT_IPI = &gait_walk_R_B1_IPI;
          } else if (board == VCK5_BRD2) {
            LUT_IPI = &gait_walk_R_B2_IPI;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // IPI - MISC1
        case PATTERN_MISC1:
          if (board == VCK5_BRD1) {
            LUT_IPI = &gait_misc1_B1_IPI;
          } else if (board == VCK5_BRD2) {
            LUT_IPI = &gait_misc1_B2_IPI;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // IPI - MISC2
        case PATTERN_MISC2:
          if (board == VCK5_BRD1) {
            LUT_IPI = &gait_misc2_B1_IPI;
          } else if (board == VCK5_BRD2) {
            LUT_IPI = &gait_misc2_B2_IPI;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // IPI - MISC3
        case PATTERN_MISC3:
          if (board == VCK5_BRD1) {
            LUT_IPI = &gait_misc3_B1_IPI;
          } else if (board == VCK5_BRD2) {
            LUT_IPI = &gait_misc3_B2_IPI;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;
      } // end switch (pattern)

      // Now update IPI
      if (need_update == 1) {
        for (int i=0; i<cwru_stim->STIM_CHANNEL_USED; i++) {

          // decide if this event's IPI is changed.
          if (cwru_stim->_current_ipi[i] != (*LUT_IPI)[i]) {
            // need to change IPI of this event
            // update new IPI value.
            cwru_stim->_current_ipi[i] = (*LUT_IPI)[i];

            // fixed scheduler
            switch(cwru_stim->_current_ipi[i]){
              case FIXED_SCHED_ID1_IPI:
                cwru_stim->_schedule_id = 1;
                break;

              case FIXED_SCHED_ID2_IPI:
                cwru_stim->_schedule_id = 2;
                break;

              case FIXED_SCHED_ID3_IPI:
                cwru_stim->_schedule_id = 3;
                break;

              default:
                cwru_stim->_schedule_id = 0; // this will not exe on UECU, since no sche 0
                break;
            } //end switch

            // For fixed scheduler, cannot use stim_cmd_set_sched(), since IPI is fixed.
            // stim_cmd_set_sched(i+1, UECU_SYNC_MSG, cwru_stim->_current_ipi[i]);

            // Need to use ChangeEventSchedMsg
            // stim_cmd_chg_evnt_sched(event_id, sched_id, delay, priority);
            stim_cmd_chg_evnt_sched(cwru_stim, i+1, cwru_stim->_schedule_id, (uint8_t)i*2, 0);
            CyDelay(1); // add 1ms delay // TODO

            //CyDelay(cwru_stim->_current_ipi[i]); //Do not need this delay
          } else {
            // If event's IPI stay the same, do nothing.
          }
        } // end for
      } // end if

      break; // end case IPI

    // if need to update AMP
    case AMP:
      if (board == VCK5_BRD1) {
        LUT_AMP = &gait_B1_AMP;
      } else if (board == VCK5_BRD2) {
        LUT_AMP = &gait_B2_AMP;
      } else {
        cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
      }

      // Now update AMP
      for (int i=0; i<cwru_stim->STIM_CHANNEL_USED; i++) {
        if (pattern == PATTERN_NO_STIM) {
          cwru_stim->_current_amplitude[i] = 0;
        } else {
          cwru_stim->_current_amplitude[i] = (*LUT_AMP)[i];
        }

        // do not set event here, since need to set after PW
        //stim_cmd_set_evnt(i+1, cwru_stim->_current_pulse_width[i], cwru_stim->_current_amplitude[i], 0); // Change Event i for port_chn_id i in sched_id 1
      } // end for
      break; // end case AMP

    // if need to update PW
    case PW:
      // Select which pattern
      need_update = 1;
      switch (pattern) {
        case PATTERN_NO_STIM:
          // do nothing here, set all 0 at the stim_cmd_set_evnt in the end
          break;

        // PW - Stand
        case PATTERN_STAND:
          if (board == VCK5_BRD1) {
            LUT_PP = &gait_stand_B1_PP;
            LUT_PW = &gait_stand_B1_PW;
          } else if (board == VCK5_BRD2) {
            LUT_PP = &gait_stand_B2_PP;
            LUT_PW = &gait_stand_B2_PW;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // PW - SIT
        case PATTERN_SIT:
          if (board == VCK5_BRD1) {
            LUT_PP = &gait_sit_B1_PP;
            LUT_PW = &gait_sit_B1_PW;
          } else if (board == VCK5_BRD2) {
            LUT_PP = &gait_sit_B2_PP;
            LUT_PW = &gait_sit_B2_PW;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // PW - LSTEP
        case PATTERN_LSETP:
          if (board == VCK5_BRD1) {
            LUT_PP = &gait_walk_L_B1_PP;
            LUT_PW = &gait_walk_L_B1_PW;
          } else if (board == VCK5_BRD2) {
            LUT_PP = &gait_walk_L_B2_PP;
            LUT_PW = &gait_walk_L_B2_PW;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // PW - RSTEP
        case PATTERN_RSETP:
          if (board == VCK5_BRD1) {
            LUT_PP = &gait_walk_R_B1_PP;
            LUT_PW = &gait_walk_R_B1_PW;
          } else if (board == VCK5_BRD2) {
            LUT_PP = &gait_walk_R_B2_PP;
            LUT_PW = &gait_walk_R_B2_PW;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // PW - MISC1
        case PATTERN_MISC1:
          if (board == VCK5_BRD1) {
            LUT_PP = &gait_misc1_B1_PP;
            LUT_PW = &gait_misc1_B1_PW;
          } else if (board == VCK5_BRD2) {
            LUT_PP = &gait_misc1_B2_PP;
            LUT_PW = &gait_misc1_B2_PW;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // PW - MISC2
        case PATTERN_MISC2:
          if (board == VCK5_BRD1) {
            LUT_PP = &gait_misc2_B1_PP;
            LUT_PW = &gait_misc2_B1_PW;
          } else if (board == VCK5_BRD2) {
            LUT_PP = &gait_misc2_B2_PP;
            LUT_PW = &gait_misc2_B2_PW;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;

        // PW - MISC3
        case PATTERN_MISC3:
          if (board == VCK5_BRD1) {
            LUT_PP = &gait_misc3_B1_PP;
            LUT_PW = &gait_misc3_B1_PW;
          } else if (board == VCK5_BRD2) {
            LUT_PP = &gait_misc3_B2_PP;
            LUT_PW = &gait_misc3_B2_PW;
          } else {
            cwru_stim->_stim_error |= STIM_ERROR_UPDATE_PATTERN_ERROR;
          }
          break;
      } // end switch (pattern)

      // Now update PW
      if (need_update == 1) {
        for (int i=0; i<cwru_stim->STIM_CHANNEL_USED; i++) {
          // save the previous PW value
          uint8_t _last_pulse_width = cwru_stim->_current_pulse_width[i];

          if (pattern == PATTERN_NO_STIM) {
            cwru_stim->_current_pulse_width[i] = 0;
          } else {
            // ramping function
            cwru_stim->_current_pulse_width[i] = stim_get_PW_ramping(i, LUT_PP, LUT_PW, cycle_percentage);
            //cwru_stim->_current_pulse_width[i] = (*LUT_PW)[i][cwru_stim->STIM_CHANNEL_USED-1];
          }
          // publish if only it is diff than the previous value
          // if ((_last_pulse_width != cwru_stim->_current_pulse_width[i]) || (cycle_percentage == 0)) {
          if (_last_pulse_width != cwru_stim->_current_pulse_width[i]) {
            // Change Event i+1 for port_chn_id i
            stim_cmd_set_evnt( cwru_stim,
                                i+1,
                                stim_exe_chan_pw_gain(cwru_stim, i), // calulate gain * cwru_stim->_current_pulse_width[i]
                                stim_exe_chan_amp_gain(cwru_stim, i), // calulate gain * cwru_stim->_current_amplitude[i]
                                 0);
          }
        } // end for
      } // end if

      break; // end case PW

    // Error
    default:
      cwru_stim->_stim_error |= STIM_ERROR_UPDATE_TYPE_ERROR;
      return -1;
      break;
  } // end switch (type)

  */
  return 1;
}
