/* This file is gerated by signals_to_c.py script, do not modify. */
OS_FLASH_MEM struct democar_t democar = 
{
  {
    {"exp", &ioboard_exp, 4, DEMOCAR_EXP_MBLK_SZ, (iocSignal*)&democar.exp.LEFT},
    {0, 1, OS_BOOLEAN|IOC_PIN_PTR, &ioboard_exp, &pins.outputs.LEFT}, /* LEFT */
    {1, 1, OS_BOOLEAN|IOC_PIN_PTR, &ioboard_exp, &pins.outputs.RIGHT}, /* RIGHT */
    {2, 1, OS_BOOLEAN|IOC_PIN_PTR, &ioboard_exp, &pins.outputs.FORWARD}, /* FORWARD */
    {3, 1, OS_BOOLEAN|IOC_PIN_PTR, &ioboard_exp, &pins.outputs.BACKWARD} /* BACKWARD */
  },

  {
    {"imp", &ioboard_imp, 4, DEMOCAR_IMP_MBLK_SZ, (iocSignal*)&democar.imp.LeftTurn},
    {0, 1, OS_BOOLEAN, &ioboard_imp, OS_NULL}, /* LeftTurn */
    {1, 1, OS_BOOLEAN, &ioboard_imp, OS_NULL}, /* RightTurn */
    {2, 1, OS_BOOLEAN, &ioboard_imp, OS_NULL}, /* StraightForward */
    {3, 1, OS_BOOLEAN, &ioboard_imp, OS_NULL} /* ForwardBackward */
  },

  {
    {"conf_exp", &ioboard_conf_exp, 5, DEMOCAR_CONF_EXP_MBLK_SZ, (iocSignal*)&democar.conf_exp.tod_state},
    {0, 1, OS_CHAR, &ioboard_conf_exp, OS_NULL}, /* tod_state */
    {2, 1, OS_INT, &ioboard_conf_exp, OS_NULL}, /* tod_tail */
    {7, 1, OS_CHAR, &ioboard_conf_exp, OS_NULL}, /* frd_state */
    {9, 1, OS_INT, &ioboard_conf_exp, OS_NULL}, /* frd_head */
    {14, 257, OS_UCHAR, &ioboard_conf_exp, OS_NULL} /* frd_buf */
  },

  {
    {"conf_imp", &ioboard_conf_imp, 7, DEMOCAR_CONF_IMP_MBLK_SZ, (iocSignal*)&democar.conf_imp.tod_cmd},
    {0, 1, OS_CHAR, &ioboard_conf_imp, OS_NULL}, /* tod_cmd */
    {2, 1, OS_CHAR, &ioboard_conf_imp, OS_NULL}, /* tod_select */
    {4, 1, OS_INT, &ioboard_conf_imp, OS_NULL}, /* tod_head */
    {9, 257, OS_UCHAR, &ioboard_conf_imp, OS_NULL}, /* tod_buf */
    {267, 1, OS_CHAR, &ioboard_conf_imp, OS_NULL}, /* frd_cmd */
    {269, 1, OS_UCHAR, &ioboard_conf_imp, OS_NULL}, /* frd_select */
    {271, 1, OS_INT, &ioboard_conf_imp, OS_NULL} /* frd_tail */
  }
};

static OS_FLASH_MEM iocMblkSignalHdr * OS_FLASH_MEM democar_mblk_list[] =
{
  &democar.exp.hdr,
  &democar.imp.hdr,
  &democar.conf_exp.hdr,
  &democar.conf_imp.hdr
};

OS_FLASH_MEM iocDeviceHdr democar_hdr = {(iocMblkSignalHdr**)democar_mblk_list, sizeof(democar_mblk_list)/sizeof(iocMblkSignalHdr*)};
