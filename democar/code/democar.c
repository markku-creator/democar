/**

  @file    democar.c
  @brief   Democar IO board example featuring  IoT device.
  @author  Pekka Lehtikoski, Markku Nissinen
  @version 1.0
  @date    6.4.2020

  IOBOARD_CTRL_CON define selects how this IO device connects to control computer. One of
  IOBOARD_CTRL_CONNECT_SOCKET, IOBOARD_CTRL_CONNECT_TLS or IOBOARD_CTRL_CONNECT_SERIAL.

  Serial port can be selected using Windows style using "COM1", "COM2"... These are mapped
  to hardware/operating system in device specific manner. On Linux port names like
  "ttyS30,baud=115200" or "ttyUSB0" can be also used.

  IOBOARD_MAX_CONNECTIONS sets maximum number of connections. IO board needs one connection.

  Notes:

  - ON MULTITHREADING ENVIRONMENT WITH SELECTS LOOP THREAD CAN WAIT FOR TIMEOUT OR EVENT

  Copyright 2020 Pekka Lehtikoski. This file is part of the iocom project and shall only be used,
  modified, and distributed under the terms of the project licensing. By continuing to use, modify,
  or distribute this file you indicate that you have read the license and understand and accept
  it fully.

****************************************************************************************************
*/
/* Select socket, TLS or serial communication before including democar.h.
 */
#define IOBOARD_CTRL_CON IOBOARD_CTRL_CONNECT_TLS
#include "democar.h"

/* The devicedir is here for testing only, take away.
 */
#include "devicedir.h"

/* Use Gazerbeamm library to enable wifi configuration by Android phone's flash light and phototransistor
   connected to microcontroller (0 or 1) ?.
 */
#define DEMOCAR_USE_GAZERBEAM 0
#if DEMOCAR_USE_GAZERBEAM
#include "gazerbeam.h"
static GazerbeamReceiver gazerbeam;
#endif

/* Get controller IP address from UDP multicast (0 or 1) ?.
 */
#define DEMOCAR_USE_LIGHTHOUSE 1
#if DEMOCAR_USE_LIGHTHOUSE
#include "lighthouse.h"
static LighthouseClient lighthouse;
#endif

/* IO console state (for development/testing)
 */
IO_DEVICE_CONSOLE(ioconsole);

/* IO device configuration.
 */
iocNodeConf ioapp_device_conf;

/* Either blink LED by morse code to indicate boot error or use display if we have one.
 */
#if PINS_DISPLAY
    static PinsDisplay pins_display;
#else
    static MorseCode morse;
#endif

/* Device information.
*/
static dinfoNodeConfState dinfo_nc;
static dinfoResMonState dinfo_rm;

/* Maximum number of sockets, etc.
 */
#define IOBOARD_MAX_CONNECTIONS 1

/* Timer for sending
 */
static os_timer send_timer;

/* Use static memory pool
 */
#define MY_POOL_SZ \
    (IOBOARD_POOL_SIZE(IOBOARD_CTRL_CON, IOBOARD_MAX_CONNECTIONS, \
        DEMOCAR_EXP_MBLK_SZ, DEMOCAR_IMP_MBLK_SZ) \
        + IOBOARD_POOL_DEVICE_INFO(IOBOARD_MAX_CONNECTIONS) \
        + IOBOARD_POOL_IMP_EXP_CONF(IOBOARD_MAX_CONNECTIONS, \
            DEMOCAR_CONF_EXP_MBLK_SZ, DEMOCAR_CONF_IMP_MBLK_SZ))

#define ALLOCATE_STATIC_POOL (OSAL_DYNAMIC_MEMORY_ALLOCATION == 0)

#if ALLOCATE_STATIC_POOL
    static os_char ioboard_pool[MY_POOL_SZ];
#endif

/* Streamer for transferring IO device configuration and flash program. The streamer is used
   to transfer a stream using buffer within memory block. This static structure selects which
   signals are used for straming data between the controller and IO device.
 */
static iocStreamerParams ioc_ctrl_stream_params = IOBOARD_DEFAULT_CTRL_STREAM(democar,
    ioapp_network_defaults, sizeof(ioapp_network_defaults));

static iocControlStreamState ioc_ctrl_state;

static os_timer mytimer;

/* If needed for the operating system, EOSAL_C_MAIN macro generates the actual C main() function.
 */
EOSAL_C_MAIN

/**
****************************************************************************************************

  @brief Set up the communication.

  Sets up network and Initialize transport
  @return  OSAL_SUCCESS if all fine, other values indicate an error.

****************************************************************************************************
*/
osalStatus osal_main(
    os_int argc,
    os_char *argv[])
{

#if IOBOARD_CTRL_CON & IOBOARD_CTRL_IS_TLS
    osalSecurityConfig *security;
#endif
    iocNetworkInterfaces *nics;
    iocWifiNetworks *wifis;
    iocDeviceId *device_id;
    iocConnectionConfig *connconf;
    ioboardParams prm;
    const osalStreamInterface *iface;
    osPersistentParams persistentprm;
    dinfoNodeConfSignals nc_sigs;
    dinfoSystemSpeSignals si_sigs;
    dinfoResMonSignals rm_sigs;
    OSAL_UNUSED(argc);
    OSAL_UNUSED(argv);

    /* Setup error handling. Here we select to keep track of network state. We could also
       set application specific error handler callback by calling osal_set_error_handler().
     */
    osal_initialize_net_state();

    /* Initialize persistent storage (typically flash is running in micro-controller)
     */
    os_memclear(&persistentprm, sizeof(persistentprm));
    persistentprm.subdirectory = IOBOARD_DEVICE_NAME;
    os_persistent_initialze(&persistentprm);

    /* If we are using devicedir for development testing, initialize.
     */
    io_initialize_device_console(&ioconsole, &ioboard_root);

    /* Setup IO pins.
     */
    pins_setup(&pins_hdr, PINS_DEFAULT);

    /* Load device configuration from peristant storage, or if not available use
       defaults compiled in this code (config/include/<hw>/<device_name>-network-defaults.c, etc).
     */
    ioc_load_node_config(&ioapp_device_conf, ioapp_network_defaults,
        sizeof(ioapp_network_defaults), persistentprm.subdirectory, IOC_LOAD_PBNR_NODE_CONF);
    device_id = ioc_get_device_id(&ioapp_device_conf);
    connconf = ioc_get_connection_conf(&ioapp_device_conf);

    /* Setup network interface configuration for micro-controller environment and initialize
       transport library. This is partyly ignored if network interfaces are managed by operating
       system (Linux/Windows,etc),
     */
    nics = ioc_get_nics(&ioapp_device_conf);
    wifis = ioc_get_wifis(&ioapp_device_conf);

#if IOBOARD_CTRL_CON & IOBOARD_CTRL_IS_TLS

    security = ioc_get_security_conf(&ioapp_device_conf);
    osal_tls_initialize(nics->nic, nics->n_nics, wifis->wifi, wifis->n_wifi, security);

#else
    osal_socket_initialize(nics->nic, nics->n_nics, wifis->wifi, wifis->n_wifi);
#endif

    osal_serial_initialize();

    /* Initialize up device information.
     */
    DINFO_SET_COMMON_NET_CONF_SIGNALS_FOR_WIFI(nc_sigs, democar);
    DINFO_SET_COMMON_RESOURCE_MONITOR_SIGNALS(rm_sigs, democar);
    dinfo_initialize_node_conf(&dinfo_nc, &nc_sigs);
    dinfo_initialize_resource_monitor(&dinfo_rm, &rm_sigs);

    /* Get stream interface by IOBOARD_CTRL_CON define.
     */
    iface = IOBOARD_IFACE;

    /* Set up parameters for the IO board.
     */
    os_memclear(&prm, sizeof(prm));
    prm.iface = iface;
    prm.device_name = IOBOARD_DEVICE_NAME; /* or device_id->device name to allow change */
    prm.device_nr = device_id->device_nr;
    prm.password = device_id->password;
    prm.network_name = device_id->network_name;
    prm.ctrl_type = IOBOARD_CTRL_CON;
    prm.socket_con_str = connconf->connection[0].parameters;
    prm.serial_con_str = connconf->connection[0].parameters;
    prm.max_connections = IOBOARD_MAX_CONNECTIONS;
    prm.exp_mblk_sz = DEMOCAR_EXP_MBLK_SZ;
    prm.imp_mblk_sz = DEMOCAR_IMP_MBLK_SZ;
#if ALLOCATE_STATIC_POOL
    prm.pool = ioboard_pool;
#endif
    prm.pool_sz = MY_POOL_SZ;
    prm.device_info = ioapp_signals_config;
    prm.device_info_sz = sizeof(ioapp_signals_config);
    prm.conf_exp_mblk_sz = DEMOCAR_CONF_EXP_MBLK_SZ;
    prm.conf_imp_mblk_sz = DEMOCAR_CONF_IMP_MBLK_SZ;
#if DEMOCAR_USE_LIGHTHOUSE
    prm.lighthouse = &lighthouse;
    prm.lighthouse_func = ioc_get_lighthouse_connectstr;
#endif
    prm.exp_signal_hdr = &democar.exp.hdr;
    prm.imp_signal_hdr = &democar.imp.hdr;
    prm.conf_exp_signal_hdr = &democar.conf_exp.hdr;
    prm.conf_imp_signal_hdr = &democar.conf_imp.hdr;

    /* Initialize IOCOM and set up memory blocks for the ioboard.
     */
    ioboard_setup_communication(&prm);

    /* Set up device information.
     */
    dinfo_set_node_conf(&dinfo_nc, device_id, connconf, nics, wifis, security);
    DINFO_SET_COMMON_SYSTEM_SPECS_SIGNALS(si_sigs, democar);
    dinfo_set_system_specs(&si_sigs, DEMOCAR_HW);

    /* Set callback to detect received data and connection status changes.
     */
    ioc_add_callback(&ioboard_imp, ioboard_communication_callback, OS_NULL);

    /* Connect PINS library to IOCOM library
     */
    pins_connect_iocom_library(&pins_hdr);

    /* Make sure that control stream state is clear even after soft reboot.
     */
    ioc_init_control_stream(&ioc_ctrl_state, &ioc_ctrl_stream_params);

    /* Listen for UDP broadcasts with server address. Select IPv6 is our socket connection
       string starts with '[' (indicates IPv6 address).
     */
#if DEMOCAR_USE_LIGHTHOUSE
    ioc_initialize_lighthouse_client(&lighthouse, prm.socket_con_str[0] == '[', OS_TRUE, OS_NULL);
#endif

    /* Initialize library to receive wifi configuration by phototransostor.
     */
#if DEMOCAR_USE_GAZERBEAM
    initialize_gazerbeam_receiver(&gazerbeam, &pins.inputs.gazerbeam, GAZERBEAM_DEFAULT);
#endif

    /* Setup to blink LED bat boot errors, etc. Handle network state notifications.
     */
#if PINS_DISPLAY
    initialize_display(&pins_display, &ioboard_root, OS_NULL);
#else
    initialize_morse_code(&morse, &pins.outputs.led_builtin, OS_NULL,
        MORSE_HANDLE_NET_STATE_NOTIFICATIONS);
#endif

    /* Start communication.
     */
    ioboard_start_communication(&prm);

    mytimer = 0;
    os_get_timer(&send_timer);

    /* When emulating micro-controller on PC, run loop. Just save context pointer on
       real micro-controller.
     */
    osal_simulated_loop(OS_NULL);

    return OSAL_SUCCESS;
}


/**
****************************************************************************************************

  @brief Loop function to be called repeatedly.

  The osal_loop() function maintains communication, reads IO pins (reading forwards input states
  to communication) and runs the IO device functionality.

  @param   app_context Void pointer, to pass application context structure, etc.
  @return  The function returns OSAL_SUCCESS to continue running. Other return values are
           to be interprened as reboot on micro-controller or quit the program on PC computer.

****************************************************************************************************
*/
osalStatus osal_loop(
    void *app_context)
{

    OSAL_UNUSED(app_context);

    os_timer ti;
    osalStatus s;

   /* static os_boolean test_toggle; */

    os_get_timer(&ti);

    /* Run light house.
     */
#if DEMOCAR_USE_LIGHTHOUSE
    ioc_run_lighthouse_client(&lighthouse, OS_NULL);
#endif

    /* Get Wifi configuration messages from Android phone flash light -> phototransistor.
     */
#if DEMOCAR_USE_GAZERBEAM
    gazerbeam_run_configurator(&gazerbeam, GAZERBEAM_DEFAULT);
#endif

    /* Keep the display or morse code LED alive. These indicates boot issues, etc, to user.
     */
#if PINS_DISPLAY
    run_display(&pins_display, &ti);
#else
    blink_morse_code(&morse, &ti);
#endif

    /* Keep the communication alive. If data is received from communication, the
       ioboard_communication_callback() will be called. Move data data synchronously
       to incomong memory block.
     */
    ioc_run(&ioboard_root);
    ioc_receive(&ioboard_imp);
    ioc_receive(&ioboard_conf_imp);
    ioc_run_control_stream(&ioc_ctrl_state, &ioc_ctrl_stream_params);

    /* Read all input pins from hardware into global pins structures. Reading will forward
       input states to communication.
     */
    pins_read_all(&pins_hdr, PINS_DEFAULT);

 /*   if (os_has_elapsed_since(&mytimer, &ti, 1000)) {
        pin_set(&pins.outputs.LEFT,test_toggle);
        pin_set(&pins.outputs.RIGHT,test_toggle);
        pin_set(&pins.outputs.FORWARD,test_toggle);
        pin_set(&pins.outputs.BACKWARD,test_toggle);
        mytimer = ti;
        test_toggle = !test_toggle;
    } */

    int LeftTurn = ioc_get(&democar.imp.LeftTurn);
    int RightTurn = ioc_get(&democar.imp.RightTurn);
    int StraightForward = ioc_get(&democar.imp.StraightForward);
    int ForwardBackward = ioc_get(&democar.imp.ForwardBackward);

    if (StraightForward && ForwardBackward && !LeftTurn && !RightTurn) {
        pin_set(&pins.outputs.LEFT,0);
        pin_set(&pins.outputs.RIGHT,0);
        pin_set(&pins.outputs.FORWARD,1);
        pin_set(&pins.outputs.BACKWARD,0);
    }

    if (StraightForward && !ForwardBackward && !LeftTurn && !RightTurn) {
        pin_set(&pins.outputs.LEFT,0);
        pin_set(&pins.outputs.RIGHT,0);
        pin_set(&pins.outputs.FORWARD,0);
        pin_set(&pins.outputs.BACKWARD,1);
    }

    if (LeftTurn && ForwardBackward && !StraightForward && !RightTurn) {
        pin_set(&pins.outputs.LEFT,1);
        pin_set(&pins.outputs.RIGHT,0);
        pin_set(&pins.outputs.FORWARD,1);
        pin_set(&pins.outputs.BACKWARD,0);
    }

    if (LeftTurn && !ForwardBackward && !StraightForward && !RightTurn) {
        pin_set(&pins.outputs.LEFT,1);
        pin_set(&pins.outputs.RIGHT,0);
        pin_set(&pins.outputs.FORWARD,0);
        pin_set(&pins.outputs.BACKWARD,1);
    }

    if (RightTurn && ForwardBackward && !StraightForward && !LeftTurn) {
        pin_set(&pins.outputs.RIGHT,1);
        pin_set(&pins.outputs.LEFT,0);
        pin_set(&pins.outputs.FORWARD,1);
        pin_set(&pins.outputs.BACKWARD,0);
    }

    if (RightTurn && !ForwardBackward && !StraightForward && !LeftTurn) {
        pin_set(&pins.outputs.RIGHT,1);
        pin_set(&pins.outputs.LEFT,0);
        pin_set(&pins.outputs.FORWARD,0);
        pin_set(&pins.outputs.BACKWARD,1);
    }

    if (!RightTurn && !StraightForward && !LeftTurn) {
        pin_set(&pins.outputs.LEFT,0);
        pin_set(&pins.outputs.RIGHT,0);
        pin_set(&pins.outputs.FORWARD,0);
        pin_set(&pins.outputs.BACKWARD,0);
    }

    /* The call is here for development/testing.
     */
    s = io_run_device_console(&ioconsole);

    /* Send changed data synchronously from outgoing memory blocks every 50 ms. If we need
       very low latency IO in local network we can have interval like 1 ms, or just call send
       unconditionally.
       If we are not in such hurry, we can save network resources by merging multiple changes.
       to be sent together in TCP package and use value like 100 ms.
       Especially in IoT we may want to minimize number of transferred TCP packets to
       cloud server. In this case it is best to use to two timers and flush ioboard_exp and
       ioboard_conf_exp separately. We could evenu use value like 2000 ms or higher for
       ioboard_exp. For ioboard_conf_exp we need to use relatively short value, like 100 ms
       even then to keep software updates, etc. working. This doesn't generate much
       communication tough, conf_export doesn't change during normal operation.
     */
    if (os_timer_hit(&send_timer, &ti, 10))
    {
        ioc_send(&ioboard_exp);
        ioc_send(&ioboard_conf_exp);
        ioc_run(&ioboard_root);
    }

    dinfo_run_node_conf(&dinfo_nc, &ti);
    dinfo_run_resource_monitor(&dinfo_rm, &ti);

    return s;
}


/**
****************************************************************************************************

  @brief Finished with the application, clean up.

  The osal_main_cleanup() function ends IO board communication, cleans up and finshes with the
  socket and serial port libraries.

  On real IO device we may not need to take care about this, since these are often shut down
  only by turning or power or by microcontroller reset.

  @param   app_context Void pointer, to pass application context structure, etc.
  @return  None.

****************************************************************************************************
*/
void osal_main_cleanup(
    void *app_context)
{

    OSAL_UNUSED(app_context);

#if DEMOCAR_USE_LIGHTHOUSE
    ioc_release_lighthouse_client(&lighthouse);
#endif

    ioboard_end_communication();
#if IOBOARD_CTRL_CON & IOBOARD_CTRL_IS_TLS
    osal_tls_shutdown();
#else
    osal_socket_shutdown();
#endif
    osal_serial_shutdown();

    pins_shutdown(&pins_hdr);

    ioc_release_node_config(&ioapp_device_conf);
}


/**
****************************************************************************************************

  @brief Callback function when data has been received from communication.

  The ioboard_communication_callback function reacts to data from communication. Here we treat
  memory block as set of communication signals, and mostly just forward these to IO.

  @param   handle Memory block handle.
  @param   start_addr First changed memory block address.
  @param   end_addr Last changed memory block address.
  @param   flags IOC_MBLK_CALLBACK_WRITE indicates change by local write,
           IOC_MBLK_CALLBACK_RECEIVE change by data received.
  @param   context Callback context, not used by "gina" example.
  @return  None.

****************************************************************************************************
*/
void ioboard_communication_callback(
    struct iocHandle *handle,
    os_int start_addr,
    os_int end_addr,
    os_ushort flags,
    void *context)
{
// #if IOC_DEVICE_PARAMETER_SUPPORT
    const iocSignal *sig;
    os_int n_signals;
    OSAL_UNUSED(context);

    /* If this memory block is not written by communication, no need to do anything.
     */
    if ((handle->flags & IOC_MBLK_DOWN) == 0 ||
        (flags & IOC_MBLK_CALLBACK_RECEIVE) == 0)
    {
        return;
    }

    /* Get range of signals that may have changed. Signals are in order by address.
     */
    sig = ioc_get_signal_range(handle, start_addr, end_addr, &n_signals);

    /* Check if this callback causes change in device info
     */
    dinfo_node_conf_callback(&dinfo_nc, sig, n_signals, flags);

    while (n_signals-- > 0)
    {
        if (sig->flags & IOC_PIN_PTR) {
            forward_signal_change_to_io_pin(sig, IOC_SIGNAL_DEFAULT);
        }
        sig++;
    }

}
