macro_command main()
  int LW_payload_address = 1337, macro_ids[2] = {25, 32}
  
  unsigned short payload[6], size = 6 // NOTE: make sure payload[#] and size = # match
  payload[0] = 80 // LW_DDO_ADDRESS
  payload[1] = 80 // LB_DDO_CLEAR_ADDRESS
  payload[2] = 300 // DDO_WIDTH
  payload[3] = 480 // DDO_LENGTH
  payload[4] = 0 // COLOR_BLACK
  payload[5] = 9 // COLOR_BROWN

  SetData(payload[0], "Local HMI", LW, LW_payload_address, size)
  ASYNC_TRIG_MACRO(macro_ids[0])
  while payload[0] <> 0
    DELAY(1)
    GetData(payload[0], "Local HMI", LW, LW_payload_address, 1)
  wend

  payload[0] = 90 // LW_DDO_ADDRESS
  payload[1] = 90 // LB_DDO_CLEAR_ADDRESS
  payload[2] = 300 // DDO_WIDTH
  payload[3] = 480 // DDO_LENGTH
  payload[4] = 0 // COLOR_BLACK
  payload[5] = 9 // COLOR_BROWN

  SetData(payload[0], "Local HMI", LW, LW_payload_address, size)
  ASYNC_TRIG_MACRO(macro_ids[1])
  while payload[0] <> 0
    DELAY(1)
    GetData(payload[0], "Local HMI", LW, LW_payload_address, 1)
  wend
  
end macro_command