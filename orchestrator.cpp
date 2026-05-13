macro_command main()
  // TODO: make BASE_PAYLOAD_ADDRESS to be able to check if there's a process running and do an early return
  int LW_payload_address = 1337
  int DDO_BASE_ADDRESS = 8008, CLEAR_BASE_ADDRESS = 9001
  int DDO_WIDTH = 300, DDO_LENGTH = 240
  int WINDOW_ROWS = 2, WINDOW_COLS = 2
  int COLOR_BLACK = 0, COLOR_RED = 1, COLOR_GREEN = 2, COLOR_BLUE = 3

  int macro_ids[4] = {25, 29, 31, 32}, colors[4]
  colors[0] = COLOR_BLACK
  colors[1] = COLOR_RED
  colors[2] = COLOR_GREEN
  colors[3] = COLOR_BLUE

  unsigned short payload[9], size = 9 // NOTE: make sure payload[#] and size = # match
  int i, j, id
  for i = 0 to (WINDOW_ROWS-1) step 1
    for j = 0 to (WINDOW_COLS-1) step 1
      id = i*WINDOW_COLS + j
      payload[0] = WINDOW_ROWS // WINDOW_ROWS
      payload[1] = WINDOW_COLS // WINDOW_COLS
      payload[2] = i // ROW_INDEX
      payload[3] = j // COL_INDEX
      payload[4] = DDO_BASE_ADDRESS + id*10 // LW_DDO_ADDRESS
      payload[5] = CLEAR_BASE_ADDRESS + id // LB_DDO_CLEAR_ADDRESS
      payload[6] = DDO_WIDTH // DDO_WIDTH
      payload[7] = DDO_LENGTH // DDO_LENGTH
      payload[8] = colors[id] // LINE_COLOR

      SetData(payload[0], "Local HMI", LW, LW_payload_address, size)
      ASYNC_TRIG_MACRO(macro_ids[id])
      // wait for the fish to bite the bait
      while payload[0] <> 0
        DELAY(1)
        GetData(payload[0], "Local HMI", LW, LW_payload_address, 1)
      wend
    next j
  next i
end macro_command