/* This file is gerated by pins_to_c.py script, do not modify. */
#include "pins.h"

/* Parameters for inputs */
static os_ushort pins_inputs_gazerbeam_prm[]= {PIN_RV, PIN_RV, PIN_INTERRUPT_ENABLED, 1};
PINS_INTCONF_STRUCT(pin_gazerbeam_intconf)

/* Parameters for outputs */
static os_ushort pins_outputs_led_builtin_prm[]= {PIN_RV, PIN_RV};
static os_ushort pins_outputs_RIGHT_prm[]= {PIN_RV, PIN_RV};
static os_ushort pins_outputs_LEFT_prm[]= {PIN_RV, PIN_RV};
static os_ushort pins_outputs_FORWARD_prm[]= {PIN_RV, PIN_RV};
static os_ushort pins_outputs_BACKWARD_prm[]= {PIN_RV, PIN_RV};

/* DEMOCAR IO configuration structure */
OS_FLASH_MEM pins_t pins =
{
  {{1, &pins.inputs.gazerbeam}, /* inputs */
    {PIN_INPUT, 0, 39, pins_inputs_gazerbeam_prm, sizeof(pins_inputs_gazerbeam_prm)/sizeof(os_ushort), OS_NULL, OS_NULL PINS_INTCONF_PTR(pin_gazerbeam_intconf)} /* gazerbeam */
  },

  {{5, &pins.outputs.led_builtin}, /* outputs */
    {PIN_OUTPUT, 0, 2, pins_outputs_led_builtin_prm, sizeof(pins_outputs_led_builtin_prm)/sizeof(os_ushort), OS_NULL, OS_NULL PINS_INTCONF_NULL}, /* led_builtin */
    {PIN_OUTPUT, 0, 32, pins_outputs_RIGHT_prm, sizeof(pins_outputs_RIGHT_prm)/sizeof(os_ushort), OS_NULL, &democar.exp.RIGHT PINS_INTCONF_NULL}, /* RIGHT */
    {PIN_OUTPUT, 0, 33, pins_outputs_LEFT_prm, sizeof(pins_outputs_LEFT_prm)/sizeof(os_ushort), OS_NULL, &democar.exp.LEFT PINS_INTCONF_NULL}, /* LEFT */
    {PIN_OUTPUT, 0, 25, pins_outputs_FORWARD_prm, sizeof(pins_outputs_FORWARD_prm)/sizeof(os_ushort), OS_NULL, &democar.exp.FORWARD PINS_INTCONF_NULL}, /* FORWARD */
    {PIN_OUTPUT, 0, 26, pins_outputs_BACKWARD_prm, sizeof(pins_outputs_BACKWARD_prm)/sizeof(os_ushort), OS_NULL, &democar.exp.BACKWARD PINS_INTCONF_NULL} /* BACKWARD */
  },
};

/* List of pin type groups */
static OS_FLASH_MEM PinGroupHdr * OS_FLASH_MEM pins_group_list[] =
{
  &pins.inputs.hdr,
  &pins.outputs.hdr
};

/* DEMOCAR IO configuration top header structure */
OS_FLASH_MEM IoPinsHdr pins_hdr = {pins_group_list, sizeof(pins_group_list)/sizeof(PinGroupHdr*)};
