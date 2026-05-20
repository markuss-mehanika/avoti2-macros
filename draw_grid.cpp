//   ----columns---->
// r| origin
// o| v
// w| ._______________________________
// s| |               |               |
//  V |               |               |
//    |               |               |
//    |               |               |
//    |_______________|_______________|
//    |               |               |
//    |               |               |
//    |               |               |
//    |               |               |
//    |_______________|_______________|

// gobal variables
// DDO - Dynamic Drawing Object
int LW_DDO_ADDRESS, LB_DDO_CLEAR_ADDRESS
int DDO_WIDTH, DDO_LENGTH
int CONVEYOR_WIDTH_mm, CONVEYOR_LENGTH_mm

sub init_DDO(int LW_payload_address)
  unsigned short payload[6], size = 6, zero = 0 // NOTE: make sure payload[#] and size = # match
  GetData(payload[0], "Local HMI", LW, LW_payload_address, size)
  
  // TODO: add LINE_COLOR and FILL_COLOR
  LW_DDO_ADDRESS = payload[0]
  LB_DDO_CLEAR_ADDRESS = payload[1]
  DDO_WIDTH = payload[2]
  DDO_LENGTH = payload[3]
  CONVEYOR_WIDTH_mm = payload[4]
  CONVEYOR_LENGTH_mm = payload[5]

  // set to 0 to inform orchestrator that payload recieved and can start next async macro
  SetData(zero, "Local HMI", LW, LW_payload_address, 1)
end sub

macro_command main()
  int LW_PAYLOAD_ADDRESS = 1337
  init_DDO(LW_PAYLOAD_ADDRESS)

  int COLOR_BLACK = 0, COLOR_RED = 1, COLOR_GREEN = 2, COLOR_BLUE = 3, COLOR_BROWN = 4
  bool FILL = true, STROKE = false
  
  unsigned short rows, cols, box_width_mm, box_length_mm
  // GetData(destination, device_name, user-defined_tag, amount_of_destination_data_type_elements_to_read)
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  
  TRACE("%d x %d", rows, cols)
  clear(LB_DDO_CLEAR_ADDRESS)

  // don't draw if any value is 0
  if rows * cols * box_width_mm * box_length_mm == 0 then
    return
  end if
  
  int box_width_px, box_length_px
  float mm_to_pixel_width_proportion, mm_to_pixel_length_proportion, f_ONE = 1
  mm_to_pixel_width_proportion = DDO_WIDTH / (CONVEYOR_WIDTH_mm * f_ONE)
  mm_to_pixel_length_proportion = DDO_LENGTH / (CONVEYOR_LENGTH_mm * f_ONE)
  box_width_px = box_width_mm * mm_to_pixel_width_proportion
  box_length_px = box_length_mm * mm_to_pixel_length_proportion

  int origin_x = 0, origin_y = 0
  origin_x = DDO_WIDTH - box_width_px * cols 

  float dwn[2], right[2]
  dwn[0] = 0
  dwn[1] = 1
  right[0] = 1
  right[1] = 0
  // draw big fill and stroke first
  draw_box(LW_DDO_ADDRESS, origin_x, origin_y, box_width_px * cols, box_length_px * rows, FILL, 1, COLOR_BROWN)
  draw_box(LW_DDO_ADDRESS, origin_x, origin_y, box_width_px * cols, box_length_px * rows, STROKE, 1, COLOR_BLACK)
  int i, j, incremented_x, incremented_y
  // draw rows-1 thin vertical lines
  for j = 1 to cols-1 step 1
    draw_line(LW_DDO_ADDRESS, DDO_WIDTH, DDO_LENGTH, origin_x + box_width_px * j, origin_y, dwn[0], dwn[1], box_length_px*rows, 0, COLOR_BLACK)
    next j
  // draw rows-1 thin horizontal lines
  for i = 1 to rows-1 step 1
    draw_line(LW_DDO_ADDRESS, DDO_WIDTH, DDO_LENGTH, origin_x, origin_y + box_length_px * i, right[0], right[1], box_width_px*cols, 0, COLOR_BLACK)
  next i
end macro_command