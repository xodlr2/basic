#ifndef __TMC5041_REG_H__
#define __TMC5041_REG_H__

//registers for TMC5041
#define TMC5041_GENERAL_CONFIG_REGISTER 0x00
#define TMC5041_GENERAL_STATUS_REGISTER 0x01
#define TMC5041_INPUT_REGISTER 0x04

#define TMC5041_RAMP_MODE_REGISTER_1 0x20
#define TMC5041_X_ACTUAL_REGISTER_1 0x21
#define TMC5041_V_ACTUAL_REGISTER_1 0x22
#define TMC5041_V_START_REGISTER_1 0x23
#define TMC5041_A_1_REGISTER_1 0x24
#define TMC5041_V_1_REGISTER_1 0x25
#define TMC5041_A_MAX_REGISTER_1 0x26
#define TMC5041_V_MAX_REGISTER_1 0x27
#define TMC5041_D_MAX_REGISTER_1 0x28
#define TMC5041_D_1_REGISTER_1 0x2a
#define TMC5041_V_STOP_REGISTER_1 0x2b
#define TMC5041_T_ZERO_WAIT_REGISTER_1 0x2c
#define TMC5041_X_TARGET_REGISTER_1 0x2d

#define TMC5041_RAMP_MODE_REGISTER_2 0x40
#define TMC5041_X_ACTUAL_REGISTER_2 0x41
#define TMC5041_V_ACTUAL_REGISTER_2 0x42
#define TMC5041_V_START_REGISTER_2 0x43
#define TMC5041_A_1_REGISTER_2 0x44
#define TMC5041_V_1_REGISTER_2 0x45
#define TMC5041_A_MAX_REGISTER_2 0x46
#define TMC5041_V_MAX_REGISTER_2 0x47
#define TMC5041_D_MAX_REGISTER_2 0x48
#define TMC5041_D_1_REGISTER_2 0x4a
#define TMC5041_V_STOP_REGISTER_2 0x4b
#define TMC5041_T_ZERO_WAIT_REGISTER_2 04c
#define TMC5041_X_TARGET_REGISTER_2 0x4d

#define TMC5041_HOLD_RUN_CURRENT_REGISTER_1 0x30
#define TMC5041_REFERENCE_SWITCH_CONFIG_REGISTER_1 0x34
#define TMC5041_X_LATCH_REGISTER_1 0x36
#define TMC5041_RAMP_STATUS_REGISTER_1 0x35
#define TMC5041_CHOPPER_CONFIGURATION_REGISTER_1 0x6c
#define TMC5041_DRIVER_STATUS_REGISTER_1 0x6f

#define TMC5041_HOLD_RUN_CURRENT_REGISTER_2 0x50
#define TMC5041_REFERENCE_SWITCH_CONFIG_REGISTER_2 0x54
#define TMC5041_X_LATCH_REGISTER_2 0x56
#define TMC5041_RAMP_STATUS_REGISTER_2 0x55
#define TMC5041_CHOPPER_CONFIGURATION_REGISTER_2 0x7c
#define TMC5041_DRIVER_STATUS_REGISTER_2 0x7f

//standard values
#define I_HOLD_DELAY 2
#define V_LOW_SENSE 0.325
#define V_HIGH_SENSE 0.18
#define SQRT_2 (1.414213562373095)
#define TMC_5041_R_SENSE 0.29        // Lars: 270mOhm + 20mOhm from datasheet on electronic


/*************************************************************************************
R : Read only
W : Write only
R/W : Read- and writable register
R+C : Clear upon read
*************************************************************************************/

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long            :3;     // Reserved
        unsigned long POSCMP_EN  :1;     // 0: Outputs INT and PP are tristated, 1: Position compare pulse (PP) and interrupt output (INT) are available
        unsigned long            :3;     // Reserved
        unsigned long GTEST_MODE :1;     // 0: Normal operation, 1: Enable analog test output on pin REFR2 TEST_SEL selects the function of REFR2: 0…4: T120, DAC1, VDDH1, DAC2, VDDH2
        unsigned long SHAFT1     :1;     // 1: Inverse motor 1 direction
        unsigned long SHAFT2     :1;     // 1: Inverse motor 2 direction
        unsigned long LOCK_GCONF :1;     // 1: GCONF is locked against further write access.
        unsigned long            :21;    // Reserved
    }bit;
} reg_rw_gconf;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long RESET      :1;     // 1: Indicates that the IC has been reset since the last read access to GSTAT. All registers have been cleared to reset values.
        unsigned long DRV_ERR1   :1;     // 1: Indicates, that driver 1 has been shut down due to overtemperature or short circuit detection since the last read access. Read DRV_STATUS1 for details. The flag can only be reset when all error conditions are cleared
        unsigned long DRV_ERR2   :1;     // 1: Indicates, that driver 2 has been shut down due to overtemperature or short circuit detection since the last read access. Read DRV_STATUS2 for details. The flag can only be reset when all error conditions are cleared
        unsigned long UV_CP      :1;     // 1: Indicates an undervoltage on the charge pump. The driver is disabled in this case.
        unsigned long            :28;    // Reserved
    }bit;
} reg_rc_gstat;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long            :7;     // Unused, ignore these bits
        unsigned long DRV_EN     :1;     // drv_enn_in: DRV_ENN pin polarity
        unsigned long            :1;     // Unused, ignore this bit
        unsigned long            :15;    // Reserved
        unsigned long VERSION    :8;     // VERSION: 0x10=version of the IC Identical numbers mean full digital compatibility.
    }bit;
} reg_input;

typedef unsigned long reg_w_x_compare;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long RAMPMODE   :2;     // 0: Positioning mode, 1: Velocity mode to positive VMAX, 2: Velocity mode to negative VMAX, 3: Hold mode
        unsigned long            :30;    // Reserved
    }bit;
} reg_rw_rampmode;

typedef long reg_rw_xactual;             //Actual motor position (signed), range : -2^31…+(2^31)-1

typedef union {
    long    VALUE;
    struct {
        long VACTUAL             :24;    // Actual motor velocity from ramp generator (signed) range : +-(2^23)-1[μsteps / t]
        long                     :8;     // Reserved
    }bit;
} reg_r_vactual;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long VSTART     :18;    // Motor start velocity (unsigned), Set VSTOP ≥ VSTART!, range : 0…(2^18)-1[μsteps / t]
        unsigned long            :14;    // Reserved
    }bit;
} reg_w_vstart;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long A1         :16;    // First acceleration between VSTART and V1 (unsigned), range : 0…(2^16)-1[μsteps / ta²]
        unsigned long            :16;    // Reserved
    }bit;
} reg_w_a1;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long V1         :20;    // First acceleration / deceleration phase threshold velocity (unsigned), 0: Disables A1 and D1 phase, use AMAX, DMAX only, range : 0…(2^20)-1[μsteps / t]
        unsigned long            :12;    // Reserved
    }bit;
} reg_w_v1;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long AMAX       :16;    // Second acceleration between V1 and VMAX (unsigned), range : 0…(2^16)-1[μsteps / ta²]
        unsigned long            :16;    // Reserved
    }bit;
} reg_w_amax;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long VMAX       :23;    // Motion ramp target velocity (for positioning ensure VMAX ≥ VSTART) (unsigned), range : 0…(2^23)-512[μsteps / t]
        unsigned long            :9;     // Reserved
    }bit;
} reg_w_vmax;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long DMAX       :16;    // Deceleration between VMAX and V1 (unsigned), range : 0…(2^16)-1[μsteps / ta²]
        unsigned long            :16;    // Reserved
    }bit;
} reg_w_dmax;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long D1         :16;    // Deceleration between V1 and VSTOP (unsigned), Attention: Do not set 0 in positioning mode, even if V1=0!, range : 1…(2^16)-1[μsteps / ta²]
        unsigned long            :16;    // Reserved
    }bit;
} reg_w_d1;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long VSTOP      :18;    // Motor stop velocity (unsigned), Attention: Set VSTOP ≥ VSTART!, Attention: Do not set 0 in positioning mode, minimum 10 recommended!, range : 1…(2^18)-1[μsteps / ta²]
        unsigned long            :14;    // Reserved
    }bit;
} reg_w_vstop;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long TZEROWAIT  :16;    // Waiting time after ramping down to zero velocity before next movement or direction inversion can start and before motor power down starts. Time range is about 0 to 2 seconds. range : 0…(2^16)-1 * 512 tCLK
        unsigned long            :16;    // Reserved
    }bit;
} reg_w_tzerowait;

typedef long reg_rw_xtarget;             //Target position for ramp mode (signed). Write a new target position to this register in order to activate the ramp generator positioning in RAMPMODE=0. Initialize all velocity, acceleration and deceleration parameters before, range : -2^31…+(2^31)-1

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long IHOLD      :5;     // Standstill current (0=1/32…31=32/32), In combination with stealthChop mode, setting IHOLD=0 allows to choose freewheeling or coil short circuit for motor stand still.
        unsigned long IRUN       :5;     // Motor run current (0=1/32…31=32/32), Hint: Choose sense resistors in a way, that normal IRUN is 16 to 31 for best microstep performance.
        unsigned long IHOLDDELAY :4;     // Controls the number of clock cycles for motor power down after a motion as soon as TZEROWAIT has expired. The smooth transition avoids a motor jerk upon power down., 0: instant power down, 1..15: Delay per current reduction step in multiple of 2^18 clocks
        unsigned long            :18;    // Reserved
    }bit;
} reg_w_ihold_irun;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long VCOOLTHRS  :23;     // This is the lower threshold velocity for switching on smart energy coolStep and stallGuard feature. Further it is the upper operation velocity for stealthChop. (unsigned)
        unsigned long            :9;      // Reserved
    }bit;
} reg_w_vcoolthrs;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long VHIGH      :23;     // This velocity setting allows velocity dependent switching into a different chopper mode and fullstepping to maximize torque. (unsigned)
        unsigned long            :9;      // Reserved
    }bit;
} reg_w_vhigh;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long STOP_L_ENABLE     :1;       // 1: Enables automatic motor stop during active left reference switch input
        unsigned long STOP_R_ENABLE     :1;       // 1: Enables automatic motor stop during active right reference switch input
        unsigned long POL_STOP_L        :1;       // Sets the active polarity of the left reference switch input
        unsigned long POL_STOP_R        :1;       // Sets the active polarity of the right reference switch input
        unsigned long SWAP_LR           :1;       // 1: Swap the left and the right reference switch input REFL and REFR
        unsigned long LATCH_L_ACTIVE    :1;       // 1: Activates latching of the position to XLATCH upon an active going edge on the left reference switch input REFL.
        unsigned long LATCH_L_INACTIVE  :1;       // 1: Activates latching of the position to XLATCH upon an inactive going edge on the left reference switch input REFL. The active level is defined by pol_stop_l.
        unsigned long LATCH_R_ACTIVE    :1;       // 1: Activates latching of the position to XLATCH upon an active going edge on the right reference switch input REFR.
        unsigned long LATCH_R_INACTIVE  :1;       // 1: Activates latching of the position to XLATCH upon an inactive going edge on the right reference switch input REFR. The active level is defined by pol_stop_r.
        unsigned long                   :1;       // Reserved
        unsigned long SG_STOP           :1;       // 1: Enable stop by stallGuard2. Disable to release motor after stop event.
        unsigned long EN_SOFT_STOP      :1;       // 0: Hard stop, 1: Soft stop
        unsigned long                   :20;      // Reserved
    }bit;
} reg_rw_sw_mode;

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long STATUS_STOP_L      :1;
        unsigned long STATUS_STOP_R      :1;
        unsigned long STATUS_LATCH_L     :1;
        unsigned long STATUS_LATCH_R     :1;
        unsigned long EVENT_STOP_L       :1;
        unsigned long EVENT_STOP_R       :1;
        unsigned long EVENT_STOP_SG      :1;
        unsigned long EVENT_STOP_REACHED :1;
        unsigned long VELOCITY_REACHED   :1;
        unsigned long POSITION_REACHED   :1;
        unsigned long VZERO              :1;
        unsigned long T_ZEROWAIT_ACTIVE  :1;
        unsigned long SECOND_MOVE        :1;
        unsigned long STATUS_SG          :1;
        unsigned long                    :18;
    }bit;
} reg_rc_ramp_stat;


typedef unsigned long reg_r_xlatch;     // Ramp generator latch position, latches XACTUAL upon a programmable switch event (see SW_MODE).

typedef union {
    unsigned long    VALUE;
    struct {
        unsigned long TOFF       :4;     // Off time/MOSFET disable
        unsigned long HSTRT      :3;     // Hysteresis start value or Fast decay time setting
        unsigned long HEND       :4;     // Hysteresis end (low) value or Sine wave offset
        unsigned long FD3        :1;     // chm=1: MSB of fast decay time setting TFD
        unsigned long DISFDCC    :1;     // fast decay mode, chm=1:disfdcc=1 disables current comparator usage for termi-nation of the fast decay cycle
        unsigned long RNDTF      :1;     // Random TOFF time
        unsigned long CHM        :1;     // Chopper mode(0: Standard mode, 1: Constant tOFF with fast decay time.)
        unsigned long TBL        :2;     // Blanking time select
        unsigned long VSENSE     :1;     // Bsense resistor voltage based current scaling
        unsigned long VHIGHFS    :1;     // high velocity fullstep selection
        unsigned long VHIGHCHM   :1;     // high velocity chopper mode
        unsigned long            :4;     // Reserved
        unsigned long MRES       :4;     // MRES micro step resolution
        unsigned long            :2;     // Reserved
        unsigned long DISS2G     :1;     // short to GND protection disable
        unsigned long            :1;     // Reserved
    }bit;
} reg_chopconf;

#endif  //__TMC5041_REG_H__
