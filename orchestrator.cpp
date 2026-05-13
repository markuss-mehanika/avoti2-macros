macro_command main()
  int LW_payload_address = 1337, macro_ids[2] = {25, 32}
  
  unsigned short payload[9], size = 9 // NOTE: make sure payload[#] and size = # match
  payload[0] = 1 // WINDOW_ROWS
  payload[1] = 2 // WINDOW_COLS
  payload[2] = 0 // ROW_INDEX
  payload[3] = 0 // COL_INDEX
  payload[4] = 80 // LW_DDO_ADDRESS
  payload[5] = 80 // LB_DDO_CLEAR_ADDRESS
  payload[6] = 300 // DDO_WIDTH
  payload[7] = 480 // DDO_LENGTH
  payload[8] = 0 // LINE_COLOR

  SetData(payload[0], "Local HMI", LW, LW_payload_address, size)
  ASYNC_TRIG_MACRO(macro_ids[0])
  while payload[0] <> 0
    DELAY(1)
    GetData(payload[0], "Local HMI", LW, LW_payload_address, 1)
  wend

  payload[0] = 1 // WINDOW_ROWS
  payload[1] = 2 // WINDOW_COLS
  payload[2] = 0 // ROW_INDEX
  payload[3] = 1 // COL_INDEX
  payload[4] = 90 // LW_DDO_ADDRESS
  payload[5] = 90 // LB_DDO_CLEAR_ADDRESS
  payload[6] = 300 // DDO_WIDTH
  payload[7] = 480 // DDO_LENGTH
  payload[8] = 6 // LINE_COLOR

  SetData(payload[0], "Local HMI", LW, LW_payload_address, size)
  ASYNC_TRIG_MACRO(macro_ids[1])
  while payload[0] <> 0
    DELAY(1)
    GetData(payload[0], "Local HMI", LW, LW_payload_address, 1)
  wend
  
end macro_command