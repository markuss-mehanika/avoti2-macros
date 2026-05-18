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

macro_command main()
  // DDO - Dynamic Drawing Object
  int DDO_ADDRESS = 70, DDO_CLEAR_ADDRESS = 70
  int DDO_WIDTH = 420, DDO_LENGTH = 480
  // TODO: replace temporary conveyor size with IRL size
  int CONVEYOR_WIDTH_mm = 1337, CONVEYOR_LENGTH_mm = 1738

  int COLOR_BLACK = 0, COLOR_BROWN = 9
  bool FILL = true, STROKE = false
  
  unsigned short rows, cols, box_width_mm, box_length_mm
  // GetData(destination, device_name, user-defined_tag, amount_of_destination_data_type_elements_to_read)
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  
  TRACE("%d x %d", rows, cols)
  clear(DDO_CLEAR_ADDRESS)

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
  draw_box(DDO_ADDRESS, origin_x, origin_y, box_width_px * cols, box_length_px * rows, FILL, 1, COLOR_BROWN)
  draw_box(DDO_ADDRESS, origin_x, origin_y, box_width_px * cols, box_length_px * rows, STROKE, 1, COLOR_BLACK)
  int i, j, incremented_x, incremented_y
  // draw rows-1 thin vertical lines
  for j = 1 to cols-1 step 1
    draw_line(DDO_ADDRESS, DDO_WIDTH, DDO_LENGTH, origin_x + box_width_px * j, origin_y, dwn[0], dwn[1], box_length_px*rows, 0, COLOR_BLACK)
    next j
  // draw rows-1 thin horizontal lines
  for i = 1 to rows-1 step 1
    draw_line(DDO_ADDRESS, DDO_WIDTH, DDO_LENGTH, origin_x, origin_y + box_length_px * i, right[0], right[1], box_width_px*cols, 0, COLOR_BLACK)
  next i
end macro_command