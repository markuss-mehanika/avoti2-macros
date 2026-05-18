macro_command main()
  TRACE("orchestrator start")
  // TODO: make BASE_PAYLOAD_ADDRESS to be able to check if there's a process running and wait for them to finish before starting
  int LW_PAYLOAD_ADDRESS = 1337, LB_IS_SORT_ADDRESS = 1337
  
  int LW_DDO_ADDRESS = 8080, LB_DDO_CLEAR_ADDRESS = 67

  int SORT_DDO_WIDTH = 420, SORT_DDO_LENGTH = 480
  // TODO: replace temporary conveyor size with IRL size
  int SORT_CONVEYOR_WIDTH = 1337, SORT_CONVEYOR_LENGTH = 1738 // in mm
  
  int BUFF_DDO_WIDTH = 300, BUFF_DDO_LENGTH = 240
  // TODO: replace temporary conveyor size with IRL size
  int BUFF_CONVEYOR_WIDTH = 1337, BUFF_CONVEYOR_LENGTH = 1738 // in mm

  int COLOR_BLACK = 0, COLOR_RED = 1, COLOR_GREEN = 2, COLOR_BLUE = 3, COLOR_BROWN = 4

  int ddo_width, ddo_length, conveyor_width, conveyor_length
  bool is_sort, off = false
  GetData(is_sort, "Local HMI", LB, LB_IS_SORT_ADDRESS, 1)
  SetData(off, "Local HMI", LB, LB_IS_SORT_ADDRESS, 1)

  if is_sort then
    ddo_width = SORT_DDO_WIDTH 
    ddo_length = SORT_DDO_LENGTH 
    conveyor_width = SORT_CONVEYOR_WIDTH 
    conveyor_length = SORT_CONVEYOR_LENGTH 
  else
    ddo_width = BUFF_DDO_WIDTH 
    ddo_length = BUFF_DDO_LENGTH 
    conveyor_width = BUFF_CONVEYOR_WIDTH 
    conveyor_length = BUFF_CONVEYOR_LENGTH 
  end if

  int macro_id = 24

  unsigned short payload[6], size = 6 // NOTE: make sure payload[#] and size = # match
  // NOTE: important that payload[0] does not contain 0 
  // because child macro sets payload[0] to 0 signaling that parameters are received
  // TODO: test if it's okay to use the same address for both DDOs
  payload[0] = LW_DDO_ADDRESS
  payload[1] = LB_DDO_CLEAR_ADDRESS
  payload[2] = ddo_width
  payload[3] = ddo_length
  payload[4] = conveyor_width
  payload[5] = conveyor_length

  SetData(payload[0], "Local HMI", LW, LW_PAYLOAD_ADDRESS, size)
  ASYNC_TRIG_MACRO(macro_id)
  // wait for the fish to bite the bait
  while payload[0] <> 0
    DELAY(1)
    GetData(payload[0], "Local HMI", LW, LW_PAYLOAD_ADDRESS, 1)
  wend
  TRACE("orchestrator end")
end macro_command