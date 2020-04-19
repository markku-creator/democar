/* This file is gerated by signals-to-c.py script, do not modify. */
OSAL_C_HEADER_BEGINS

typedef struct democar_t
{
  struct 
  {
    iocMblkSignalHdr hdr;
    iocSignal LEFT;
    iocSignal RIGHT;
    iocSignal FORWARD;
    iocSignal BACKWARD;
  }
  exp;

  struct 
  {
    iocMblkSignalHdr hdr;
    iocSignal LeftTurn;
    iocSignal RightTurn;
    iocSignal StraightForward;
    iocSignal ForwardBackward;
  }
  imp;

  struct 
  {
    iocMblkSignalHdr hdr;
    iocSignal tod_state;
    iocSignal tod_tail;
    iocSignal frd_state;
    iocSignal frd_buf;
    iocSignal frd_head;
  }
  conf_exp;

  struct 
  {
    iocMblkSignalHdr hdr;
    iocSignal tod_cmd;
    iocSignal tod_select;
    iocSignal tod_buf;
    iocSignal tod_head;
    iocSignal frd_cmd;
    iocSignal frd_select;
    iocSignal frd_tail;
  }
  conf_imp;
}
democar_t;

#define DEMOCAR_EXP_MBLK_SZ 32
#define DEMOCAR_IMP_MBLK_SZ 32
#define DEMOCAR_CONF_EXP_MBLK_SZ 272
#define DEMOCAR_CONF_IMP_MBLK_SZ 276

extern const democar_t democar;
extern const iocDeviceHdr democar_hdr;


/* Array length defines. */
#define DEMOCAR_CONF_EXP_FRD_BUF_ARRAY_SZ 257
#define DEMOCAR_CONF_IMP_TOD_BUF_ARRAY_SZ 257

/* Defines to check in code with #ifdef to know if signal is configured in JSON. */
#define DEMOCAR_EXP_LEFT
#define DEMOCAR_EXP_RIGHT
#define DEMOCAR_EXP_FORWARD
#define DEMOCAR_EXP_BACKWARD
#define DEMOCAR_IMP_LEFTTURN
#define DEMOCAR_IMP_RIGHTTURN
#define DEMOCAR_IMP_STRAIGHTFORWARD
#define DEMOCAR_IMP_FORWARDBACKWARD
#define DEMOCAR_CONF_EXP_TOD_STATE
#define DEMOCAR_CONF_EXP_TOD_TAIL
#define DEMOCAR_CONF_EXP_FRD_STATE
#define DEMOCAR_CONF_EXP_FRD_BUF
#define DEMOCAR_CONF_EXP_FRD_HEAD
#define DEMOCAR_CONF_IMP_TOD_CMD
#define DEMOCAR_CONF_IMP_TOD_SELECT
#define DEMOCAR_CONF_IMP_TOD_BUF
#define DEMOCAR_CONF_IMP_TOD_HEAD
#define DEMOCAR_CONF_IMP_FRD_CMD
#define DEMOCAR_CONF_IMP_FRD_SELECT
#define DEMOCAR_CONF_IMP_FRD_TAIL

#ifndef IOBOARD_DEVICE_NAME
#define IOBOARD_DEVICE_NAME "democar"
#endif

OSAL_C_HEADER_ENDS
