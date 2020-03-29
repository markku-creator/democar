/* This file is gerated by pins-to-c.py script, do not modify. */
OSAL_C_HEADER_BEGINS

/* DEMOCAR IO configuration structure */
typedef struct
{
  struct
  {
    PinGroupHdr hdr;
    Pin gazerbeam;
  }
  inputs;

  struct
  {
    PinGroupHdr hdr;
    Pin led_builtin;
    Pin RIGHT;
    Pin LEFT;
    Pin FORWARD;
    Pin BACKWARD;
  }
  outputs;
}
pins_t;

/* DEMOCAR IO configuration top header structure */
extern const IoPinsHdr pins_hdr;

/* Global DEMOCAR IO configuration structure */
extern const pins_t pins;

/* Name defines for pins and application pin groups (use ifdef to check if HW has pin) */
#define PINS_INPUTS_GAZERBEAM "gazerbeam"
#define PINS_OUTPUTS_LED_BUILTIN "led_builtin"
#define PINS_OUTPUTS_RIGHT "RIGHT"
#define PINS_OUTPUTS_LEFT "LEFT"
#define PINS_OUTPUTS_FORWARD "FORWARD"
#define PINS_OUTPUTS_BACKWARD "BACKWARD"

OSAL_C_HEADER_ENDS
