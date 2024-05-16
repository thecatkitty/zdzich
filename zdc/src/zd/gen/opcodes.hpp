#define ModRM(Mode, RegOpcode) (uint8_t)((Mode) | ((RegOpcode) << 3))

#define ModRM_BX_SI        0x00
#define ModRM_BX_DI        0x01
#define ModRM_BP_SI        0x02
#define ModRM_BP_DI        0x03
#define ModRM_SI           0x04
#define ModRM_DI           0x05
#define ModRM_disp16       0x06
#define ModRM_BX           0x07
#define ModRM_BX_SI_disp8  0x40
#define ModRM_BX_DI_disp8  0x41
#define ModRM_BP_SI_disp8  0x42
#define ModRM_BP_DI_disp8  0x43
#define ModRM_SI_disp8     0x44
#define ModRM_DI_disp8     0x45
#define ModRM_BP_disp8     0x46
#define ModRM_BX_disp8     0x47
#define ModRM_BX_SI_disp16 0x80
#define ModRM_BX_DI_disp16 0x81
#define ModRM_BP_SI_disp16 0x82
#define ModRM_BP_DI_disp16 0x83
#define ModRM_SI_disp16    0x84
#define ModRM_DI_disp16    0x85
#define ModRM_BP_disp16    0x86
#define ModRM_BX_disp16    0x87
#define ModRM_A            0xC0
#define ModRM_C            0xC1
#define ModRM_D            0xC2
#define ModRM_B            0xC3
#define ModRM_SPAH         0xC4
#define ModRM_BPCH         0xC5
#define ModRM_SIDH         0xC6
#define ModRM_DIBH         0xC7

#define ADD_r16_rm16 0x03

#define CALL_rel16 0xE8

#define CMP_AL_imm8  0x3C
#define CMP_AX_imm16 0x3D

#define INC_rm16 0xFF
#define INC_r16  0x40

#define JNE_rel8  0x75
#define JNE_rel16 0x850F

#define MOV_rm16_r16    0x89
#define MOV_r8_rm8      0x8A
#define MOV_r16_rm16    0x8B
#define MOV_reg8_imm8   0xB0
#define MOV_reg16_imm16 0xB8
#define MOV_rm16_imm16  0xC7

#define INT_imm8 0xCD
